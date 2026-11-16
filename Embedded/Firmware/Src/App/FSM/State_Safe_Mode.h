/**
 * @file      State_Safe_Mode_Interface.h
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Beshoy Esmat)
 * @brief     Public interface for the Safe Mode.
 * @details   Safe mode prototypes, hysteresis thresholds, recovery definitions.
 * @version   1.0.0
 * @date      22/11/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */

#ifndef STATE_SAFE_MODE_INTERFACE_H
#define STATE_SAFE_MODE_INTERFACE_H

#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include "../../Common/MacroFunction.h"

/**
 * @details Engine temperature must drop to or below 75.0°C before exiting
 *          STATE_SAFE_MODE is permitted.
 */
#define SAFE_MODE_TEMP_RECOVERY_C 75.0f

/**
 * @details Battery voltage must rise to or above 12.0V before exiting
 *          STATE_SAFE_MODE is permitted.
 */
#define SAFE_MODE_BATT_MIN_RECOVERY_V 12.0f

/**
 * @details Battery voltage must drop to or below 14.5V before exiting
 *          STATE_SAFE_MODE is permitted.
 */
#define SAFE_MODE_BATT_MAX_RECOVERY_V 14.5f

/**
 * @struct  SafeModeControlBlock_t
 * @brief   Runtime control block for Fault and Safe Mode state management.
 * @details Stores the fault entry timestamp, the latched Diagnostic Trouble Code,
 *          and boolean flags tracking recovery threshold validation and reset signals.
 */
typedef struct
{
    uint32_t faultEntryTimestamp_ms;
    DTC_Code_t lockedDTC;
    uint8_t recoveryConditionsMet;
    uint8_t resetSignalReceived;
} SafeModeControlBlock_t;

/**
 * @fn      void State_FAULT_OnEntry(void)
 * @brief   Entry handler for STATE_FAULT.
 * @details Latches the active DTC, captures entry timestamp, enforces emergency
 *          actuator fail-safes, and alerts the user on LCD.
 */
void State_FAULT_OnEntry(void);

/**
 * @fn      void State_FAULT_OnRun(void)
 * @brief   Cyclic execution handler for STATE_FAULT.
 * @details Monitors critical fault persistence and transitions the FSM into
 *          STATE_SAFE_MODE once immediate stabilization completes.
 */
void State_FAULT_OnRun(void);

/**
 * @fn      void State_FAULT_OnExit(void)
 * @brief   Exit handler for STATE_FAULT.
 * @details Performs cleanup actions prior to transitioning out of the immediate fault state.
 */
void State_FAULT_OnExit(void);

/**
 * @fn      void State_SAFE_MODE_OnEntry(void)
 * @brief   Entry handler for STATE_SAFE_MODE.
 * @details Configures degraded limp-home actuator limits and updates LCD status
 *          to indicate Safe Mode operation.
 */
void State_SAFE_MODE_OnEntry(void);

/**
 * @fn      void State_SAFE_MODE_OnRun(void)
 * @brief   Cyclic execution handler for STATE_SAFE_MODE.
 * @details Evaluates sensor recovery thresholds and listens for hardware reset
 *          (PE5 / INT5) or wireless CLEAR_DTC commands to restore normal drive mode.
 */
void State_SAFE_MODE_OnRun(void);

/**
 * @fn      void State_SAFE_MODE_OnExit(void)
 * @brief   Exit handler for STATE_SAFE_MODE.
 * @details Clears latched fault flags in the control block and restores normal
 *          operational LCD indicators upon returning to STATE_RUN.
 */
void State_SAFE_MODE_OnExit(void);

/**
 * @fn      uint8_t State_SAFE_MODE_IsRecoveryAllowed(void)
 * @brief   Checks whether all hardware and environmental conditions allow exiting Safe Mode.
 * @details Verifies that engine temperature is <= SAFE_MODE_TEMP_RECOVERY_C,
 *          battery voltage is within [SAFE_MODE_BATT_MIN_RECOVERY_V, SAFE_MODE_BATT_MAX_RECOVERY_V],
 *          and a valid reset signal (EVENT_FAULT_RESET / CLEAR_DTC) has been received.
 */
uint8_t State_SAFE_MODE_IsRecoveryAllowed(void);

#endif /* STATE_SAFE_MODE_INTERFACE_H */ 