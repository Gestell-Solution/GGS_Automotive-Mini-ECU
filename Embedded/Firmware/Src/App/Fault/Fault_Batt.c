
/**
 * @file    Fault_Batt.c
 * @brief   Battery voltage fault detection and recovery implementation.
 * @details Monitors battery voltage, detects under-voltage and over-voltage
 *          conditions using consecutive-cycle debounce counters, updates
 *          the active DTC, and requests an FSM transition to SAFE_MODE.
 *          Recovery requires a stable nominal voltage for at least 200 ms.
 * @version 1.3.0
 * @date    2026-11-19
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

/* Includes */
#include "Fault_Batt.h"
#include "../../Common/Definition.h"
#include "../FSM/CORE/FSM_Interface.h"
#include "../../OS/Schedular/Scheduler_Interface.h"

/* Private definitions */

/** @brief Required recovery voltage stability duration in milliseconds. */
#define FAULT_BATT_RECOVERY_STABLE_TIME_MS    200U

/** @brief Indicates that the recovery timer has started. */
#define FAULT_BATT_RECOVERY_ACTIVE_FLAG       0xFFU

/* External dependencies */
extern ECUData_t g_ecuData;
/* Private variables */

/** @brief Battery fault detector control block. */
static FaultBattControlBlock_t g_faultBattControl;


/**
 * @fn Fault_Batt_Init
 * @brief Initializes the battery fault detector.
 * @details Resets voltage, debounce counters, fault status, and timestamp.
 * @return None.
 */
void Fault_Batt_Init(void)
{
    g_faultBattControl.currentVoltage_V      = 12.6f;
    g_faultBattControl.underVoltTripCounter  = 0U;
    g_faultBattControl.overVoltTripCounter   = 0U;
    g_faultBattControl.faultActive           = false;
    g_faultBattControl.faultTripTimestamp_ms = 0U;
}


/**
 * @fn Fault_Batt_Monitor
 * @brief Monitors battery voltage and detects voltage faults.
 * @details Requires three consecutive under-voltage or over-voltage
 *          readings to activate the battery fault. Normal readings
 *          reset both debounce counters. On detection, F002 is set
 *          and the FSM is requested to enter StateSafeMode.
 * @param[in] currentVoltage_V Current battery voltage in volts.
 * @return true if a fault is active or newly detected; otherwise false.
 */
bool Fault_Batt_Monitor(float currentVoltage_V)
{
    g_faultBattControl.currentVoltage_V = currentVoltage_V;

    if (g_faultBattControl.faultActive == true)
    {
        return true;
    }

    if (currentVoltage_V < FAULT_BATT_THRESHOLD_MIN_V)
    {
        if (g_faultBattControl.underVoltTripCounter <
            FAULT_BATT_DEBOUNCE_CYCLES)
        {
            g_faultBattControl.underVoltTripCounter++;
        }

        g_faultBattControl.overVoltTripCounter = 0U;
    }
    else if (currentVoltage_V > FAULT_BATT_THRESHOLD_MAX_V)
    {
        if (g_faultBattControl.overVoltTripCounter <
            FAULT_BATT_DEBOUNCE_CYCLES)
        {
            g_faultBattControl.overVoltTripCounter++;
        }

        g_faultBattControl.underVoltTripCounter = 0U;
    }
    else
    {
        g_faultBattControl.underVoltTripCounter = 0U;
        g_faultBattControl.overVoltTripCounter  = 0U;

        return false;
    }

    if ((g_faultBattControl.underVoltTripCounter >=
         FAULT_BATT_DEBOUNCE_CYCLES) ||
        (g_faultBattControl.overVoltTripCounter >=
         FAULT_BATT_DEBOUNCE_CYCLES))
    {
        g_faultBattControl.faultActive = true;

        g_faultBattControl.faultTripTimestamp_ms =
            Scheduler_GetUptime_ms();

        /* Reserve the over-voltage counter as recovery timer state. */
        g_faultBattControl.overVoltTripCounter = 0U;

        g_ecuData.activeDTC = DtcF002BatteryFault;

        (void)FSM_Transition(StateSafeMode);

        return true;
    }

    return false;
}


/**
 * @fn Fault_Batt_IsRecoveryAllowed
 * @brief Checks recovery voltage stability.
 * @details Starts the recovery timer on the first valid nominal-voltage
 *          reading. Any voltage outside the inclusive 12.0 V to 14.5 V
 *          range resets the timer. Recovery is allowed after 200 ms of
 *          continuous stability.
 * @param[in] currentVoltage_V Current battery voltage in volts.
 * @return true if the recovery voltage has been stable for 200 ms.
 */
bool Fault_Batt_IsRecoveryAllowed(float currentVoltage_V)
{
    uint32_t currentTime_ms;

    g_faultBattControl.currentVoltage_V = currentVoltage_V;

    if (g_faultBattControl.faultActive == false)
    {
        return false;
    }

    if ((currentVoltage_V < FAULT_BATT_NOMINAL_LOW_V) ||
        (currentVoltage_V > FAULT_BATT_NOMINAL_HIGH_V))
    {
        g_faultBattControl.overVoltTripCounter = 0U;
        g_faultBattControl.faultTripTimestamp_ms = 0U;

        return false;
    }

    currentTime_ms = Scheduler_GetUptime_ms();

    if (g_faultBattControl.overVoltTripCounter !=
        FAULT_BATT_RECOVERY_ACTIVE_FLAG)
    {
        g_faultBattControl.faultTripTimestamp_ms = currentTime_ms;
        g_faultBattControl.overVoltTripCounter =
            FAULT_BATT_RECOVERY_ACTIVE_FLAG;

        return false;
    }

    if ((uint32_t)(currentTime_ms -
                   g_faultBattControl.faultTripTimestamp_ms) >=
        FAULT_BATT_RECOVERY_STABLE_TIME_MS)
    {
        return true;
    }

    return false;
}


/**
 * @fn Fault_Batt_Clear
 * @brief Clears the battery fault after successful recovery.
 * @details Clears the fault only when it is active, the recovery timer
 *          has started, the latest voltage is within the nominal range,
 *          and at least 200 ms have elapsed.
 * @return None.
 * @note Must be called from the INT5 reset-button handling path after
 *       Fault_Batt_IsRecoveryAllowed() returns true.
 */
void Fault_Batt_Clear(void)
{
    uint32_t currentTime_ms;

    if (g_faultBattControl.faultActive == false)
    {
        return;
    }

    if (g_faultBattControl.overVoltTripCounter !=
        FAULT_BATT_RECOVERY_ACTIVE_FLAG)
    {
        return;
    }

    if ((g_faultBattControl.currentVoltage_V < FAULT_BATT_NOMINAL_LOW_V) ||
        (g_faultBattControl.currentVoltage_V > FAULT_BATT_NOMINAL_HIGH_V))
    {
        g_faultBattControl.overVoltTripCounter = 0U;
        g_faultBattControl.faultTripTimestamp_ms = 0U;

        return;
    }

    currentTime_ms = Scheduler_GetUptime_ms();

    if ((uint32_t)(currentTime_ms -
                   g_faultBattControl.faultTripTimestamp_ms) <
        FAULT_BATT_RECOVERY_STABLE_TIME_MS)
    {
        return;
    }

    g_faultBattControl.underVoltTripCounter  = 0U;
    g_faultBattControl.overVoltTripCounter   = 0U;
    g_faultBattControl.faultActive           = false;
    g_faultBattControl.faultTripTimestamp_ms = 0U;

    if (g_ecuData.activeDTC == DtcF002BatteryFault)
    {
        g_ecuData.activeDTC = DtcNone;
    }
}


/**
 * @fn Fault_Batt_IsActive
 * @brief Returns the current battery fault status.
 * @return true if a battery fault is active; otherwise false.
 */
bool Fault_Batt_IsActive(void)
{
    return g_faultBattControl.faultActive;
}
