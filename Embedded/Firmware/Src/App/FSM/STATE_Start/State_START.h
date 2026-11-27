/**
 * @file      State_START.h
 * @brief     Public interface and self-test definitions for ECU STATE_START.
 * @details   Declares boot self-test timing macros, self-test validation
 *            thresholds, control block structure, and lifecycle/status APIs for
 *            the Power-On Self-Test (POST) sequence.
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

/** @brief Total duration (in milliseconds) of the boot self-test window. */
#define START_SELFTEST_DURATION_MS   2000U

/** @brief ADC sampling interval in milliseconds. */
#define START_SAMPLE_PERIOD_MS       10U

/** @brief Number of consecutive faulty samples required before latching error. */
#define START_SAMPLES_TO_LATCH       3U

/** @name Self-Test Matrix (Raw 10-Bit ADC Counts)
 *  @{
 */
/** @brief Minimum unsaturated raw ADC count for LM35 sensor. */
#define START_LM35_RAW_MIN           10U

/** @brief Maximum unsaturated raw ADC count for LM35 sensor. */
#define START_LM35_RAW_MAX           1014U

/** @brief Minimum raw ADC count for battery voltage (> 8.0 V). */
#define START_BATT_RAW_MIN           500U
/** @} */

/**
 * @brief Control block tracking boot self-test progress, timing, and result.
 */
typedef struct
{
    uint32_t startEntryTimestamp_ms; /**< Timestamp (ms) when StateStart was entered. */
    uint8_t  selfTestStep;           /**< Current step or sample counter in POST sequence. */
    bool     selfTestComplete;       /**< Flag indicating POST window has completed. */
    bool     selfTestPassed;         /**< Flag indicating all POST checks passed. */
} StartControlBlock_t;

/**
 * @brief  Entry callback for StateStart.
 * @details Initializes control block state and renders the boot splash screen.
 * @return void
 */
void State_START_OnEntry(void);

/**
 * @brief  Cyclic execution callback for StateStart.
 * @details Executes sensor and hardware checks and evaluates transition timeout.
 * @return void
 */
void State_START_OnRun(void);

/**
 * @brief  Exit callback for StateStart.
 * @details Cleans up temporary boot state and clears display before exiting.
 * @return void
 */
void State_START_OnExit(void);

/**
 * @brief  Returns whether the Power-On Self-Test passed without errors.
 * @return bool True if self-test passed, False otherwise.
 */
bool State_START_IsSelfTestPassed(void);

#endif /* STATE_START_H_ */