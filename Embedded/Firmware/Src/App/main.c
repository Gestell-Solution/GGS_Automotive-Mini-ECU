/**
 * @file    main.c
 * @brief   Main application entry point with Timer0 Fast PWM radiator fan test.
 * @details Integrates Timer0 Fast PWM driver verification into the main loop.
 * @version 1.0.0
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @date    2026-10-15
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "Timer0_Interface.h"

/**
 * @brief Simple delay function for testing fan speed transitions.
 * @param[in] ms Milliseconds to delay.
 */
static void Main_DelayMs(volatile uint32_t ms)
{
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 1000; j++)
        {
            /* Busy-wait loop for simulation */
        }
    }
}

int main(void)
{
    /* 1. Initialize Timer0 Fast PWM driver for radiator cooling fan (PB4/OC0) */
    mTimer0_Init_FastPWM();
    
    /* 2. Start Timer0 clocking (prescaler 64, ~977 Hz) */
    mTimer0_Start();

    while (1)
    {
        /* Test 0% Duty Cycle (Fan OFF - verifies zero-spike elimination on PB4) */
        mTimer0_SetDutyPercentage(0U);
        Main_DelayMs(2500);

        /* Test 25% Duty Cycle */
        mTimer0_SetDutyPercentage(25U);
        Main_DelayMs(2500);

        /* Test 50% Duty Cycle */
        mTimer0_SetDutyPercentage(50U);
        Main_DelayMs(2500);

        /* Test 75% Duty Cycle */
        mTimer0_SetDutyPercentage(75U);
        Main_DelayMs(2500);

        /* Test 100% Duty Cycle (Max airflow) */
        mTimer0_SetDutyPercentage(100U);
        Main_DelayMs(2500);

        /* Test Stop / Emergency Cutoff */
        mTimer0_Stop();
        Main_DelayMs(3000);

        /* Resume normal PWM operation */
        mTimer0_Start();
    }

    return 0;
}