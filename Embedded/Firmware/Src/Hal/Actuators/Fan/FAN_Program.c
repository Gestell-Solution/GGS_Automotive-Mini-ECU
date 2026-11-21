/**
 * @file
 *     FAN_Program.c
 * @brief
 *     Actuator Fan PWM HAL driver implementation.
 * @details
 *     Maps engine temperature to cooling fan duty cycle and drives PB4 / OC0
 *     exclusively through the MCAL Timer0 Fast PWM driver (US-003).
 *     No hardware register is accessed in this file (US-012 AC-1).
 *     PB4 direction is configured by mTimer0_Init_FastPWM(), so the DIO
 *     driver is not called.
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *     Reviewer:  Naira Mohammed
 * @date
 *     2026-10-19
 * @copyright
 *    Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"
#include "FAN_Interface.h"
#include "FAN_Private.h"
#include "FAN_Config.h"
#include "../../../Mcal/Timer0/Timer0_Interface.h"

#if ModuleFanEnabled == Enable

/**
 * @brief Runtime control block of the cooling fan (static, .bss, AC-7).
 * @note Accessed from task context only, therefore not volatile.
 */
static FanControlBlock_t g_fanControl = { FAN_DUTY_MIN_PCT, FAN_MODE_AUTO };

/**
 * @brief Stores an automatically computed duty, sets FAN_MODE_AUTO and
 *        applies the duty through Timer0.
 * @param DutyPct Duty cycle percentage (0 to 100).
 */
static void FAN_ApplyAutoDuty(uint8_t DutyPct)
{
    g_fanControl.currentDutyPct = DutyPct;
    g_fanControl.mode           = FAN_MODE_AUTO;
    mTimer0_SetDutyPercentage(DutyPct);
}

/**
 * @brief Initializes the cooling fan actuator (Timer0 Fast PWM, 0%, AUTO).
 */
void hFAN_Init(void)
{
    mTimer0_Init_FastPWM();

    g_fanControl.currentDutyPct = FAN_DUTY_MIN_PCT;
    g_fanControl.mode           = FAN_MODE_AUTO;

    mTimer0_SetDutyPercentage(FAN_DUTY_MIN_PCT);
}

/**
 * @brief Sets a manual fan speed percentage, clamped to 100%.
 * @param percent Target fan speed percentage (0 = OFF, 100 = Full Speed).
 * @return EOk if applied as given, ENotOk if clamped to 100%.
 */
Std_ReturnType hFAN_SetSpeed(uint8_t percent)
{
    Std_ReturnType status = EOk;
    uint8_t duty = percent;

    if (duty > FAN_DUTY_MAX_PCT)
    {
        duty   = FAN_DUTY_MAX_PCT;
        status = ENotOk;
    }

    g_fanControl.currentDutyPct = duty;
    g_fanControl.mode           = FAN_MODE_MANUAL;

    mTimer0_SetDutyPercentage(duty);

    return status;
}

/**
 * @brief Computes and sets the fan speed from the engine temperature in C.
 * @param temp_c Engine temperature in degrees Celsius.
 */
void hFAN_SetSpeedFromTemp(float temp_c)
{
    if (g_fanControl.mode != FAN_MODE_FAILSAFE)
    {
        /* Default 100%: covers temp_c >= max and NaN (all comparisons false) */
        uint8_t duty = FAN_DUTY_MAX_PCT;

        if (temp_c <= FAN_TEMP_THRESHOLD_MIN)
        {
            duty = FAN_DUTY_MIN_PCT;
        }
        else if (temp_c < FAN_TEMP_THRESHOLD_MAX)
        {
            /* 40 < temp_c < 90 gives 0.0 <= dutyFloat < 100.0, safe for uint8_t */
            float dutyFloat = ((temp_c - FAN_TEMP_THRESHOLD_MIN) /
                               (FAN_TEMP_THRESHOLD_MAX - FAN_TEMP_THRESHOLD_MIN)) *
                              FAN_PCT_SCALE_FLOAT;
            duty = (uint8_t)dutyFloat;
        }
        else
        {
            /* temp_c >= FAN_TEMP_THRESHOLD_MAX or NaN: keep full speed */
        }

        FAN_ApplyAutoDuty(duty);
    }
}

/**
 * @brief Computes and sets the fan speed from the engine temperature scaled x10.
 * @param temp_x10 Engine temperature in degrees Celsius scaled x10.
 */
void hFAN_SetSpeedFromTemp_x10(int16_t temp_x10)
{
    if (g_fanControl.mode != FAN_MODE_FAILSAFE)
    {
        uint8_t duty = FAN_DUTY_MAX_PCT;

        if (temp_x10 <= (int16_t)FAN_TEMP_SCALED_MIN)
        {
            duty = FAN_DUTY_MIN_PCT;
        }
        else if (temp_x10 < (int16_t)FAN_TEMP_SCALED_MAX)
        {
            /* 400 < temp_x10 < 900 gives 1..499, divided by 5 gives 0..99 */
            int16_t delta   = (int16_t)(temp_x10 - (int16_t)FAN_TEMP_SCALED_MIN);
            int16_t dutyInt = (int16_t)(delta / FAN_TEMP_SCALED_PER_PCT);
            duty = (uint8_t)dutyInt;
        }
        else
        {
            /* temp_x10 >= FAN_TEMP_SCALED_MAX: keep full speed */
        }

        FAN_ApplyAutoDuty(duty);
    }
}

/**
 * @brief Forces the fan to 100% and latches FAN_MODE_FAILSAFE.
 * @note The output is driven before the control block is updated so PB4
 *       reaches full speed first (AC-5). The control block is only used
 *       from task context, so the order is not observable by any caller.
 */
void hFAN_ForceMax(void)
{
    mTimer0_SetDutyCycle((uint8_t)FAN_PWM_FULL_SCALE_COUNT);

    g_fanControl.currentDutyPct = FAN_DUTY_MAX_PCT;
    g_fanControl.mode           = FAN_MODE_FAILSAFE;
}

/**
 * @brief Stops the fan (0% duty). The operating mode is not changed.
 */
void hFAN_Stop(void)
{
    g_fanControl.currentDutyPct = FAN_DUTY_MIN_PCT;

    mTimer0_SetDutyPercentage(FAN_DUTY_MIN_PCT);
}

/**
 * @brief Returns the currently active fan duty cycle percentage.
 * @return Current duty cycle (0 to 100%).
 */
uint8_t hFAN_GetSpeed(void)
{
    return g_fanControl.currentDutyPct;
}

/**
 * @brief Returns the currently active fan operating mode.
 * @return Current operating mode (AUTO, MANUAL, or FAILSAFE).
 */
FanMode_t hFAN_GetMode(void)
{
    return g_fanControl.mode;
}

#endif /* ModuleFanEnabled == Enable */
