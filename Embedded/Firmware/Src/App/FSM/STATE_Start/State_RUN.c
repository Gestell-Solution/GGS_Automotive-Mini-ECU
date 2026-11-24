/**
 * @file      State_RUN.c
 * @brief     Implementation of ECU STATE_RUN closed-loop control and supervision.
 * @details   Coordinates active engine operation in StateRun, including cooling
 *            fan proportional PWM calculation, periodic LCD/telemetry task enablement,
 *            heartbeat supervision, and clean transitions to StateOff or StateDiagnostic.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-21
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"

#if ModuleFsmEnabled == Enable
#include "State_RUN.h"

/**
 * @brief  Entry callback for StateRun; enables cyclic tasks and updates LCD mode line.
 * @return void
 */
void State_RUN_OnEntry(void){
    //implementation
}

/**
 * @brief  Cyclic execution callback for StateRun; supervises events and run-mode transitions.
 * @return void
 */
void State_RUN_OnRun(void){
    //implementation
}

/**
 * @brief  Exit callback for StateRun; disables cyclic driving tasks and cleans up state.
 * @return void
 */
void State_RUN_OnExit(void){
    //implementation
}

#endif