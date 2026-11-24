/**
 * @file      FSM_Config.h
 * @brief     Compile-time configuration parameters for the FSM State Machine Core.
 * @details   Defines FSM capacity macros, boot self-test delay limits, and state
 *            transition timeout thresholds used across the FSM module.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


#ifndef _FSM_CONFIG_H_
#define _FSM_CONFIG_H_
 
/**
 * @brief Total number of discrete operational states managed by the ECU FSM.
 */
#define FSM_NUM_STATES              6U

/**
 * @brief Maximum number of Diagnostic Trouble Codes stored in the DTC history buffer.
 */
#define FSM_MAX_DTC_HISTORY         8U
 
/**
 * @brief Maximum duration (in ms) allowed for the boot self-test sequence in STATE_START.
 */
#define FSM_SELF_TEST_TIMEOUT_MS        2000U

/**
 * @brief Maximum allowed auto-transition delay (in ms) from STATE_FAULT to STATE_SAFE_MODE.
 */
#define FSM_FAULT_TO_SAFE_TIMEOUT_MS    50U

/**
 * @brief Maximum execution latency budget (in us) for atomic FSM_Transition() execution.
 */
#define FSM_TRANSITION_MAX_LATENCY_US   10U

#endif