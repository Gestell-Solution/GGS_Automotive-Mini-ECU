/**
- @file Fault_Temp.c
- @brief Implements the engine over-temperature fault detector.
 *
- @details Monitors engine coolant temperature and detects over-temperature
-          conditions using a configurable threshold and debounce mechanism.
-          When the fault is confirmed, the module latches the fault,
-          records its activation timestamp, updates the active DTC,
-          forces the cooling fan to maximum speed, and requests an FSM
-          transition to StateFault.
 *
- @version 1.0.0
- @date 2026-11-17
- @author Rouaa Ayman <Rouaaayman2602@gmail.com>
- @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "Fault_Temp.h"
#include "Definition.h"
/**
 * @todo Replace temporary extern declarations with official module headers
 *       once integrated:
 *       - Remove temporary struct/extern definitions and include the official
 *         shared ECU data header for g_ecuData.
 *       - Include Scheduler header for Scheduler_GetUptime_ms().
 *       - Include Fan/FSM headers for hFAN_ForceMax() and FSM_Transition().
 */
typedef struct {
    DtcCode_t activeDTC;
} EcuData_t;

extern EcuData_t g_ecuData;
extern uint32_t Scheduler_GetUptime_ms(void);
extern void hFAN_ForceMax(void);
extern void FSM_Transition(EcuState_t state);
/**
- @brief Stores the internal state of the over-temperature fault detector.
 *
- @details This control block is private to the Fault_Temp module and
-          maintains the latest monitored temperature, debounce counter,
-          fault activation status, and fault activation timestamp.
 *
- @note Initialized by Fault_Temp_Init().
 */

static FaultTempControlBlock_t g_faultTempControl;

/**
- @fn Fault_Temp_Init
- @brief Initializes the over-temperature fault detector.
 *
- @details Resets the monitored temperature, debounce counter, fault status,
-          and fault activation timestamp to their initial values.
 *
- @return None.
 *
- @warning Must be called before using the fault detector APIs.
 */

void Fault_Temp_Init(void)
{
    g_faultTempControl.currentTemp_C = 0.0f;
    g_faultTempControl.overTempTripCounter = 0U;
    g_faultTempControl.faultActive = false;
    g_faultTempControl.faultTripTimestamp_ms = 0U;
}

/**
- @fn Fault_Temp_Monitor
- @brief Monitors engine coolant temperature and detects over-temperature faults.
 *
- @details Updates the stored temperature and checks whether it exceeds
-          FAULT_TEMP_THRESHOLD_MAX.
 *
-          The fault is activated only after FAULT_TEMP_DEBOUNCE_CYCLES
-          consecutive over-temperature evaluations. Any evaluation at or
-          below the critical threshold resets the debounce counter.
 *
-          Once activated, the fault is latched, its activation timestamp
-          is recorded, the active DTC is updated, the cooling fan is forced
-          to maximum speed, and an FSM transition to StateFault is requested.
 *
- @param[in] currentTemp_C Current engine coolant temperature in degrees Celsius.
 *
- @return Fault detection status.
- @retval true  An over-temperature fault is active.
- @retval false No over-temperature fault is active.
 *
- @warning Must be called from Task context because the fault activation
-          sequence invokes the fan override interface.
 *
- @note Once activated, the fault remains latched until Fault_Temp_Clear()
-       successfully clears the internal fault state.
 */

bool Fault_Temp_Monitor(float currentTemp_C)
{
    g_faultTempControl.currentTemp_C = currentTemp_C;

    if (g_faultTempControl.faultActive == true)
    {
        return true; /* Fault already latched */
    }

    if (currentTemp_C > FAULT_TEMP_THRESHOLD_MAX)
    {
        if (g_faultTempControl.overTempTripCounter <
    FAULT_TEMP_DEBOUNCE_CYCLES)
{
    g_faultTempControl.overTempTripCounter++;
}

        if (g_faultTempControl.overTempTripCounter >= FAULT_TEMP_DEBOUNCE_CYCLES)
        {
            g_faultTempControl.faultActive = true;
            g_faultTempControl.faultTripTimestamp_ms = Scheduler_GetUptime_ms();
            g_ecuData.activeDTC = DtcF001OverTemp;

            hFAN_ForceMax();

            FSM_Transition(StateFault);

            return true;
        }
    }
    else
    {
        g_faultTempControl.overTempTripCounter = 0U;
    }

    return false;
}

/**
- @fn Fault_Temp_IsRecoveryAllowed
- @brief Checks whether the current temperature permits fault recovery.
 *
- @details Recovery is allowed only when the supplied temperature is
-          strictly below FAULT_TEMP_HYSTERESIS_C.
 *
- @param[in] currentTemp_C Current engine coolant temperature in degrees Celsius.
 *
- @return Recovery eligibility.
- @retval true  Temperature is below the recovery threshold.
- @retval false Temperature is equal to or above the recovery threshold.
 */

bool Fault_Temp_IsRecoveryAllowed(float currentTemp_C)
{
 if (currentTemp_C < FAULT_TEMP_HYSTERESIS_C)
 {
   return true;
 }
 return false;
}

/**
- @fn Fault_Temp_Clear
- @brief Clears the internal over-temperature fault state when recovery is allowed.
 *
- @details Clears the fault activation status, debounce counter, and
-          activation timestamp only if the fault is currently active
-          and the most recently monitored temperature is below
-          FAULT_TEMP_HYSTERESIS_C.
 *
- @return None.
 *
- @note This function clears the internal fault detector state only.
-       It does not independently clear the shared active DTC or request
-       an FSM state transition.
 */

void Fault_Temp_Clear(void)
{
    if ((g_faultTempControl.faultActive == true) &&
        (Fault_Temp_IsRecoveryAllowed(g_faultTempControl.currentTemp_C) == true))
    {
        g_faultTempControl.overTempTripCounter = 0U;
        g_faultTempControl.faultActive = false;
        g_faultTempControl.faultTripTimestamp_ms = 0U;
    }
}

/**
- @fn Fault_Temp_IsActive
- @brief Checks whether the over-temperature fault is currently active.
 *
- @return Current fault activation status.
- @retval true  The fault is active.
- @retval false The fault is inactive.
 */

bool Fault_Temp_IsActive(void)
{
    return g_faultTempControl.faultActive;
}