/**
 * @file      State_START.h
 * @brief     Public interface and self-test definitions for ECU STATE_START.
 * @details   Declares boot self-test timing macro,
 *            control block structure, and lifecycle/status APIs for the Power-On
 *            Self-Test (POST) sequence.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: abdelaziz4338-bit
 * @date      2026-11-18
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 * @see       State_START.c, State_RUN.h, FSM_Interface.h
 */

#ifndef STATE_START_H_
#define STATE_START_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Total duration (in milliseconds) of the boot self-test window.
 */
#define START_SELFTEST_DURATION_MS      2000U

/**
 * @brief Control block tracking boot self-test progress, timing, and result.
 */
typedef struct
{
    uint32_t startEntryTimestamp_ms; /**< Timestamp (ms) when StateStart was entered */
    uint8_t  selfTestStep;           /**< Current step/sample counter in POST sequence */
    bool     selfTestComplete;       /**< Flag indicating POST window has completed */
    bool     selfTestPassed;         /**< Flag indicating all POST checks passed */
} StartControlBlock_t;

/**
 * @brief  Entry callback for StateStart; initializes control block and displays boot splash.
 * @return void
 */
void State_START_OnEntry(void);

/**
 * @brief  Cyclic execution callback for StateStart; runs sensor/HW checks and evaluates timeout.
 * @return void
 */
void State_START_OnRun(void);

/**
 * @brief  Exit callback for StateStart; cleans up temporary boot state before leaving STATE_START.
 * @return void
 */
void State_START_OnExit(void);

/**
 * @brief  Returns whether the Power-On Self-Test passed without errors.
 * @return bool True if self-test passed, False otherwise.
 */
bool           State_START_IsSelfTestPassed(void);

#endif /* STATE_START_H_ */