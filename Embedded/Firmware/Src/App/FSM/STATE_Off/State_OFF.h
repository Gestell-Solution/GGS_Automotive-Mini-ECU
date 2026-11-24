/**
 * @file      State_OFF.h
 * @brief     FSM STATE_OFF enums, control block data types, and public API.
 * @details   Defines ignition trigger sources, debounce lockout timing, control block
 *            state tracking, and lifecycle hooks for the STATE_OFF handler.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef _STATE_OFF_H_
#define _STATE_OFF_H_

#include "../../../Common/Definition.h"
#include <stdint.h>
#include <stdbool.h>

/** @brief Minimum lockout window (in milliseconds) between ignition toggles to suppress contact bounce. */
#define IGNITION_DEBOUNCE_LOCKOUT_MS    500U

/**
 * @brief Identifies the origin of an ignition request.
 */
typedef enum {
    IGN_SOURCE_NONE      = 0U, /**< No trigger source / uninitialized. */
    IGN_SOURCE_BUTTON    = 1U, /**< Physical push button on INT4 (PE4). */
    IGN_SOURCE_BLUETOOTH = 2U, /**< Remote command over Bluetooth (USART0). */
    IGN_SOURCE_WIFI      = 3U  /**< Remote command over Wi-Fi (USART1). */
} IgnitionSource_t;

/**
 * @brief Internal state and arbitration tracking block for ignition commands.
 */
typedef struct {
    bool             ignitionState;          /**< Current ignition state flag (true when active/requested). */
    IgnitionSource_t lastTriggerSource;      /**< Source that triggered the last accepted ignition toggle. */
    uint32_t         lastToggleTimestamp_ms; /**< System uptime (ms) at the last accepted toggle. */
} IgnitionControlBlock_t;

/**
 * @brief Entry hook executed when entering STATE_OFF.
 * @details Silences actuators, updates the LCD standby message, and resets ignition state.
 */
void State_OFF_OnEntry(void);

/**
 * @brief Periodic execution hook while the ECU is in STATE_OFF.
 * @details Consumes pending ignition events and forwards valid start requests to arbitration.
 */
void State_OFF_OnRun(void);

/**
 * @brief Exit hook executed when leaving STATE_OFF.
 * @details Clears pending ignition event flags before transitioning to the next state.
 */
void State_OFF_OnExit(void);

/**
 * @brief Arbitrates and processes an ignition trigger request.
 * @param[in] source         Originating ignition source (`IgnitionSource_t`).
 * @param[in] startRequested True to request starting ignition/transitioning to STATE_START.
 * @return Std_ReturnType    `EOk` if accepted and transition triggered, `ENotOk` otherwise.
 */
Std_ReturnType State_OFF_TriggerIgnition(IgnitionSource_t source, bool startRequested);

/**
 * @brief Returns whether ignition is currently marked active in the control block.
 * @return bool `true` if ignition is active, `false` otherwise.
 */
bool State_OFF_IsIgnitionActive(void);

#endif /* _STATE_OFF_H_ */