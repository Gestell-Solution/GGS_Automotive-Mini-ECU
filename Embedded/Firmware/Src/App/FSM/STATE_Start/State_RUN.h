/**
 * @file      State_RUN.h
 * @brief     Public interface and timing constants for ECU STATE_RUN.
 * @details   Declares closed-loop operating periods, thermal control thresholds,
 *            and lifecycle callbacks for active driving mode.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-18
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef _STATE_RUN_H
#define _STATE_RUN_H


#define RUN_HEARTBEAT_PERIOD_MS         1000U



/**
 * @brief  Entry callback for StateRun; enables cyclic tasks and updates LCD mode line.
 * @return void
 */
void State_RUN_OnEntry(void);

/**
 * @brief  Cyclic execution callback for StateRun; supervises events and run-mode transitions.
 * @return void
 */
void State_RUN_OnRun(void);

/**
 * @brief  Exit callback for StateRun; disables cyclic driving tasks and cleans up state.
 * @return void
 */
void State_RUN_OnExit(void);



#endif