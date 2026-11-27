/**
 * @file      State_RUN.c
 * @brief     Implementation of ECU STATE_RUN closed-loop control and supervision.
 * @details   Coordinates active engine operation in StateRun, including cooling
 *            fan proportional PWM calculation, periodic LCD/telemetry task enablement,
 *            battery supervision, and clean transitions to StateOff or StateDiagnostic.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-21
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"

#if ModuleFsmEnabled == Enable
#include "State_RUN.h"
#include "../../../Hal/Actuators/Led/LED_Interface.h"
#include "../../../OS/Schedular/Scheduler_Interface.h"
#include "../../../Hal/Lcd/Lcd_Interface.h"
#include "../../../OS/EventFlags/EventFlags_Interface.h"
#include "../../../App/FSM/CORE/FSM_Interface.h"
#include "../../../Hal/Actuators/Fan/FAN_Interface.h"
#include "../../../Hal/Sensors/LM35/LM35_Interface.h"
#include "../../../Hal/Sensors/Batt/BATT_Interface.h"
#include "../../../App/DTC/DTC_Manager_Interface.h"

/** @brief Bitmask storing consumed events during STATE_RUN execution. */
static EventMask_t evt = (EventMask_t)EVENT_NONE;

static uint32_t g_lastHeartbeatToggle_ms = 0U;

/**
 * @brief  Entry callback for StateRun.
 * @details Turns ON the blue status LED, enables cyclic application tasks,
 *          and updates the LCD mode status line.
 * @return void
 */
void State_RUN_OnEntry(void)
{
    /* Turn ON Green Heartbeat / Power LED (Physical PD4) */
    g_lastHeartbeatToggle_ms = Scheduler_GetUptime_ms();
    hLED_SetState(LED_POWER_GREEN, LED_STATE_ON);

    /* Turn ON Blue Blue Engine / Telemetry Status LED (PD5) */
    hLED_SetState(LED_STATUS_BLUE, LED_STATE_ON);

    /* Enable cyclic application tasks */
    Scheduler_EnableTask("ADC");
    Scheduler_EnableTask("LCD");
    Scheduler_EnableTask("TELE");

    /* Update LCD status line */
    hLCD_WriteString(FirstLine, 0, "MODE: RUN | IGN: ON");
}

/**
 * @brief  Cyclic execution callback for StateRun.
 * @details Regulates cooling fan speed from engine temperature, checks battery
 *          voltage limits, and handles transitions to StateOff or StateDiagnostic.
 * @return void
 */
void State_RUN_OnRun(void)
{
    uint32_t now_ms = Scheduler_GetUptime_ms();

    /* Toggle every 1000 ms -> 1.0 Hz square wave on PD4 */
    if ((now_ms - g_lastHeartbeatToggle_ms) >= (RUN_HEARTBEAT_PERIOD_MS))
    {
        g_lastHeartbeatToggle_ms = now_ms;
        hLED_Toggle(LED_POWER_GREEN);
    }

    float temp = hLM35_ReadTemp_C();
    hFAN_SetSpeedFromTemp(temp);

    float volt = hBATT_GetLastVoltage_V();
    if (volt < BATT_VOLTAGE_MIN_V || volt > BATT_VOLTAGE_MAX_V)
    {
        if (!DTC_IsStored(DtcF002BatteryFault))
        {
            DTC_Log(DtcF002BatteryFault, Scheduler_GetUptime_s(), hBATT_GetLastRawADC());
            FSM_Transition(StateFault);
        }
    }
    
    /* Check for shutdown or diagnostic transition requests */
    evt = Event_Consume(EVENT_IGNITION_OFF | EVENT_DIAG_REQ);
    if (evt & EVENT_IGNITION_OFF)
    {
        FSM_Transition(STATE_OFF);
        return;
    }
    else if (evt & EVENT_DIAG_REQ)
    {
        FSM_Transition(STATE_DIAGNOSTIC);
        return;
    }
    else
    {
        /* Other transitions are illegal in STATE_RUN */
    }

    return;
}

/**
 * @brief  Exit callback for StateRun.
 * @details Disables cyclic driving tasks, turns off the status LED, and clears
 *          the LCD before leaving STATE_RUN.
 * @return void
 */
void State_RUN_OnExit(void)
{
    Scheduler_DisableTask("ADC");
    Scheduler_DisableTask("LCD");
    Scheduler_DisableTask("TELE");
    hLED_SetState(LED_STATUS_BLUE, LED_STATE_OFF);
    hLED_SetState(LED_POWER_GREEN, LED_STATE_OFF);
    hLCD_Clear();
}
#endif