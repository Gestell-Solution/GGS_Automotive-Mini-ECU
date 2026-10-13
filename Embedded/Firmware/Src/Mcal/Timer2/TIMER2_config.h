/**
* 
* @file       TIMER2_Config.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Salma Mohamed)
* @brief      Configuration file for the TIMER2 driver
* @details    This header file defines the compile-time configuration of the TIMER2 driver
* @version    1.0.0
* @date       17/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#ifndef TIMER2_CONFIG_H
#define TIMER2_CONFIG_H
/**
 * @def       Timer2_Mode_Select
 * @brief     Selects the operational waveform generation mode for Timer/Counter2.
 * @details   Configures TCCR2 register bits WGM21:WGM20.
 *            Available Options:
 *            - Timer2_Mode_Normal          : Normal 8-bit up-counting (0x00 to 0xFF)
 *            - Timer2_Mode_PhaseCorrectPWM : Phase Correct PWM mode
 *            - Timer2_Mode_CTC             : Clear Timer on Compare Match (Top = OCR2)
 *            - Timer2_Mode_FastPWM         : Fast Pulse Width Modulation
 * @note      Default for deterministic 10ms SysTick generation is Timer2_Mode_CTC.
 */

#define Timer2_Mode_Select Timer2_Mode_CTC 

/**
 * @def       Timer2_Prescaler_Select
 * @brief     Selects the clock prescaler division factor for Timer/Counter2.
 * @details   Configures TCCR2 register bits CS22:CS20.
 *            Available Options:
 *            - Timer2_Prescaler_None       : Timer stopped (No clock source)
 *            - Timer2_Prescaler_1          : clk_I/O / 1    (No prescaling)
 *            - Timer2_Prescaler_8          : clk_I/O / 8
 *            - Timer2_Prescaler_64         : clk_I/O / 64
 *            - Timer2_Prescaler_256        : clk_I/O / 256
 *            - Timer2_Prescaler_1024       : clk_I/O / 1024
 * @note      Default for 10ms SysTick at 16 MHz with OCR2=155 is Timer2_Prescaler_1024.
 */
#define Timer2_Prescaler_Select Timer2_Prescaler_1024


#endif /* TIMER2_CONFIG_H */