/**
 * @file      State_OFF.c
 * @brief     FSM STATE_OFF driver implementation.
 * @details   Implements standby actuator shutdown, ignition event handling, debounce
 *            filtering, safety fault checks, and state exit cleanup for STATE_OFF.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../Common/Config.h"
#include <stdint.h>
#include "State_OFF.h"

#if ModuleFsmEnabled == Enable

#include "../../Hal/Actuators/Buzzer/Buzzer_Interface.h"
#include "../../Hal/Actuators/Fan/FAN_Interface.h"
#include "../../Hal/Actuators/Led/LED_Interface.h"
#include "../../Hal/Actuators/Led/LED_Private.h"
#include "../../Hal/Lcd/Lcd_Interface.h"

/** @brief Internal ignition control and arbitration block initialized to safe default values. */
static IgnitionControlBlock_t g_ignitionControl = {
    .ignitionState = false,
    .lastTriggerSource = IGN_SOURCE_NONE,
    .lastToggleTimestamp_ms = 0U
};

void State_OFF_OnEntry(void)
{
    /* 1. Silence actuators: stop fan, turn off status/warning LEDs, silence buzzer */
    hFAN_Stop();
    hLED_SetState(LED_STATUS_BLUE, LED_STATE_OFF);
    hLED_SetState(LED_WARNING_YELLOW, LED_STATE_OFF);
    hBUZZER_SetState(BUZZER_STATE_OFF);

    /* 2. Update LCD cockpit layout to standby mode */
    hLCD_WriteString(0, 0, "MODE: OFF | IGN: OFF");

    /* 3. Update shared ignition state to off */
    g_ignitionControl.ignitionState = false;

    Scheduler_DisableTask("ADC");
}

void State_OFF_OnRun(void)
{
    /* 1. Check for pending ignition events from the Event Flag Queue */
    EventMask_t evt = Event_Consume(EVENT_IGNITION_ON);

    /* 2. If ignition event is set, trigger ignition from button source */
    if (evt & EVENT_IGNITION_ON)
    {
        State_OFF_TriggerIgnition(IGN_SOURCE_BUTTON, true);
    }
}

void State_OFF_OnExit(void)
{
    /* Clear pending ignition event flags on exit from STATE_OFF */
    Event_ClearIgnition();
}

Std_ReturnType State_OFF_TriggerIgnition(IgnitionSource_t source, bool startRequested)
{
    /* 1. Capture current uptime */
    uint32_t now = Scheduler_GetUptime_ms();

    /* 2. Check debounce lockout window; reject if within lockout period */
    if ((now - g_ignitionControl.lastToggleTimestamp_ms) < IGNITION_DEBOUNCE_LOCKOUT_MS)
    {
        return ENotOk;
    }

    /* 3. If startRequested is true: verify safety, update control block, transition to STATE_START */
    if (startRequested == true)
    {
        /* Verify safety: reject if an active DTC forbids ignition */        
        
        if (DTC_GetActiveCode() != DTC_NONE)
        {
            return ENotOk;
        }

        /* Update control block */
        g_ignitionControl.ignitionState = true;
        g_ignitionControl.lastTriggerSource = source;
        g_ignitionControl.lastToggleTimestamp_ms = now;
        
        /* Command FSM transition */
        FSM_Transition(STATE_START);
        return EOk;
    }
    /* 4. Return ENotOk if start was not requested */
    return ENotOk;
}
bool State_OFF_IsIgnitionActive(void)
{
    /* Return current ignition active flag from control block */
    return g_ignitionControl.ignitionState;
}

#endif