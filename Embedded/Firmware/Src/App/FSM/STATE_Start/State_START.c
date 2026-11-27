/**
 * @file      State_START.c
 * @brief     Implementation of ECU STATE_START boot and hardware self-test logic.
 * @details   Executes the 2000 ms Power-On Self-Test (POST) sequence, validates
 *            LM35 (ADC0) and Battery (ADC1) channels across consecutive samples,
 *            verifies LCD and Timer0 PWM readiness, and transitions to StateRun
 *            on success or StateFault on failure.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-21
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"

#if ModuleFsmEnabled == Enable
#include "State_START.h"
#include "../../../Hal/Lcd/Lcd_Interface.h"
#include "../../../OS/Schedular/Scheduler_Interface.h"
#include "../../../Hal/Sensors/LM35/LM35_Interface.h"
#include "../../../Hal/Sensors/Batt/BATT_Interface.h"
#include "../../../App/FSM/CORE/FSM_Interface.h"
#include "../../../App/DTC/DTC_Manager_Interface.h"
#include "../../../Hal/Actuators/Fan/FAN_Interface.h"
#include "../../../Mcal/Dio/DIO_Interface.h"
#include <util/delay.h>

/**
 * @brief Control block instance managing STATE_START timing, step, and POST status.
 * @note  Make sure LCD strings match the display column width.
 */
static StartControlBlock_t g_startControl;

/** @brief Status returned by LCD cursor positioning check during entry. */
static Std_ReturnType g_lcdStatus = ENotOk;

/** @brief Flag indicating a hardware or ADC self-test fault occurred during POST. */
static bool g_postFaultDetected = False;


/**
 * @brief  Entry callback for STATE_START.
 * @details Initializes sensor, LCD, and fan drivers, resets control block state
 *          and flags, and displays the boot splash screen on the LCD.
 * @todo V2: init HC05 and ESP-01
 * @return void
 */
void State_START_OnEntry(void)
{
    /* Initialize sensors, display, and fan PWM */
    hLM35_Init();
    hBATT_Init();
    hLCD_Init();
    hFAN_Init(); /* Initializes and sets speed to zero */
    Scheduler_Init();

    /* Initialize control block flags and counters */
    g_startControl.startEntryTimestamp_ms = Scheduler_GetUptime_ms();
    g_startControl.selfTestComplete = False;
    g_startControl.selfTestPassed = False;
    g_startControl.selfTestStep = 0U;
    g_postFaultDetected = False;

    /* Check LCD status and render boot splash screen */
    g_lcdStatus = hLCD_SetCursor(FirstLine, 0);
    hLCD_WriteString(FirstLine, 0, "  GESTELL MINI-ECU  ");
    hLCD_WriteString(SecondLine, 0, "SELF-TEST RUNNING...");
    hLCD_WriteString(ThirdLine, 0, "CHECKING SENSORS...");
    hLCD_WriteString(FourthLine, 0, "SYSTEM V1.0.0-OFFLINE");
}

/**
 * @brief  Cyclic execution callback for STATE_START.
 * @details Samples LM35 and battery ADC channels, checks LCD and fan status,
 *          logs corresponding DTCs on out-of-range readings, and handles FSM
 *          transitions to StateFault or StateRun.
 * @return void
 */
void State_START_OnRun(void)
{
    /* Calculate elapsed time since entering STATE_START */
    uint32_t elapsed = Scheduler_GetUptime_ms() - g_startControl.startEntryTimestamp_ms;

    /* Raw ADC sample buffers for LM35 and Battery channels */
    uint16_t RawLM35[START_SAMPLES_TO_LATCH] = {0U};
    uint16_t RawBATT[START_SAMPLES_TO_LATCH] = {0U};

    /* Read LM35 raw ADC 3 times and validate unsaturated range */
    for (uint16_t index = 0; index < START_SAMPLES_TO_LATCH; index++)
    {
        RawLM35[index] = hLM35_GetLastRawADC();

        if (RawLM35[index] < START_LM35_RAW_MIN || RawLM35[index] > START_LM35_RAW_MAX)
        {
            g_postFaultDetected = True;
            if (!DTC_IsStored(DtcF003SensorError))
            {
                DTC_Log(DtcF003SensorError, Scheduler_GetUptime_s(), RawLM35[index]);
            }
        }
        _delay_ms(START_SAMPLE_PERIOD_MS);
    }
    
    /* Read Battery raw ADC 3 times and validate minimum threshold */
    for (uint16_t index = 0; index < START_SAMPLES_TO_LATCH; index++)
    {
        RawBATT[index] = hBATT_GetLastRawADC();

        if (RawBATT[index] < START_BATT_RAW_MIN)
        {
            g_postFaultDetected = true;
            if (!DTC_IsStored(DtcF002BatteryFault))
            {
                DTC_Log(DtcF002BatteryFault, Scheduler_GetUptime_s(), RawBATT[index]);
            }
        }
        _delay_ms(START_SAMPLE_PERIOD_MS);
    }

    /* Verify LCD status and ensure fan PWM is at 0 duty cycle */
    if ((g_lcdStatus != EOk) || (hFAN_GetSpeed() != 0U))
    {
        g_postFaultDetected = true;
    }

    /* Transition to StateFault if active DTC or fault condition is present */
    if (DTC_GetActiveCode() != DtcNone || g_postFaultDetected == True)
    {
        g_startControl.selfTestComplete = True;
        g_startControl.selfTestPassed = False;
        FSM_Transition(StateFault);
    }

    /** Test on schematic, whether it's a problem to not wait until 2 seconds are reached */
    /* Transition to StateRun after self-test duration completes */
    if (elapsed >= START_SELFTEST_DURATION_MS)
    {
        g_startControl.selfTestComplete = True;
        g_startControl.selfTestPassed = True;
        FSM_Transition(StateRun);
    }
}

/**
 * @brief  Exit callback for STATE_START.
 * @details Resets control block fields and clears the LCD screen before leaving
 *          the state.
 * @return void
 */
void State_START_OnExit(void)
{
    g_startControl.startEntryTimestamp_ms = 0U;
    g_startControl.selfTestComplete = False;
    g_startControl.selfTestPassed = False;
    g_startControl.selfTestStep = 0U;

    hLCD_Clear();
}

/**
 * @brief  Returns whether the Power-On Self-Test passed.
 * @return bool True if self-test passed, False otherwise.
 */
bool State_START_IsSelfTestPassed(void)
{
    return g_startControl.selfTestPassed;
}
#endif