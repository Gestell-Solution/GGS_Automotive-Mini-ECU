/**
 * @file      State_RUN.h
 * @brief     Public interface and timing constants for ECU STATE_RUN.
 * @details   Declares closed-loop operating timing definitions and state
 *            lifecycle callbacks for active closed-loop driving mode in the
 *            Gestell Automotive Mini-ECU (ATmega128 @ 16 MHz), including thermal
 *            fan regulation, periodic display/telemetry task supervision,
 *            battery monitoring, and transitions to STATE_OFF or STATE_DIAGNOSTIC.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-18
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef _STATE_RUN_H
#define _STATE_RUN_H

/**
 * @brief Period (in milliseconds) for the 1 Hz heartbeat supervision in STATE_RUN.
 * @details Corresponds to a 1000 ms (1 Hz) nominal cycle period used for
 *          system heartbeat timing during active run mode.
 */
#define RUN_HEARTBEAT_PERIOD_MS         1000U

/**
 * @brief  Entry callback for STATE_RUN.
 * @details Activates the blue status indicator (PD5), enables periodic application
 *          scheduler tasks ("ADC", "LCD", and "TELE"), and updates the first line
 *          of the 20x4 LCD to reflect active run mode ("MODE: RUN | IGN: ON").
 * @return void
 */
void State_RUN_OnEntry(void);

/**
 * @brief  Cyclic execution callback for STATE_RUN.
 * @details Reads engine temperature via LM35 to modulate cooling fan PWM output,
 *          supervises battery voltage against allowed limits (logging
 *          DtcF002BatteryFault and transitioning to StateFault if out of bounds),
 *          and consumes EVENT_IGNITION_OFF or EVENT_DIAG_REQ to transition to
 *          STATE_OFF or STATE_DIAGNOSTIC respectively.
 * @return void
 */
void State_RUN_OnRun(void);

/**
 * @brief  Cyclic execution callback for STATE_RUN.
 * @details Reads engine temperature via LM35 to modulate cooling fan PWM output,
 *          supervises battery voltage against allowed limits (logging
 *          DtcF002BatteryFault and transitioning to StateFault if out of bounds),
 *          and consumes EVENT_IGNITION_OFF or EVENT_DIAG_REQ to transition to
 *          STATE_OFF or STATE_DIAGNOSTIC respectively.
 * @return void
 */
void State_RUN_OnExit(void);

#endif