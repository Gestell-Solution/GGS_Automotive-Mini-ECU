/**
 * @file    Timer0_Interface.h
 * @brief   Defines the Timer0 Fast PWM driver interface.
 * @details Provides APIs for initializing Timer0 in Fast PWM mode,
 *          configuring PWM duty cycle, controlling the timer, and retrieving
 *          duty cycle and frequency information.
 * @version 1.0.0
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @date    2026-10-13
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef TIMER0_INTERFACE_H
#define TIMER0_INTERFACE_H

#include <stdint.h>

/**
 * @brief Defines Timer0 waveform generation modes.
 */
typedef enum
{
    Timer0_Mode_Normal          = 0x00, /**< Normal operation mode */
    Timer0_Mode_PhaseCorrectPWM = 0x40, /**< Phase Correct PWM mode */
    Timer0_Mode_CTC             = 0x08, /**< Clear Timer on Compare Match mode */
    Timer0_Mode_FastPWM         = 0x48  /**< Fast PWM mode */
} Timer0_Mode_t;

/**
 * @brief Defines Timer0 clock prescaler selections.
 */
typedef enum
{
    Timer0_Prescaler_None   = 0, /**< Timer clock stopped */
    Timer0_Prescaler_1      = 1, /**< Clock divided by 1 */
    Timer0_Prescaler_8      = 2, /**< Clock divided by 8 */
    Timer0_Prescaler_32     = 3, /**< Clock divided by 32 (not supported by Timer0 hardware) */
    Timer0_Prescaler_64     = 4, /**< Clock divided by 64 */
    Timer0_Prescaler_128    = 5, /**< Clock divided by 128 (not supported by Timer0 hardware) */
    Timer0_Prescaler_256    = 6, /**< Clock divided by 256 */
    Timer0_Prescaler_1024   = 7  /**< Clock divided by 1024 */
} Timer0_Prescaler_t;

/**
 * @brief Defines the output compare behavior of pin OC0.
 */
typedef enum
{
    Timer0_OC0_Disconnected = 0x00, /**< Disconnect OC0 from the timer output */
    Timer0_OC0_Toggle       = 0x10, /**< Toggle OC0 on compare match */
    Timer0_OC0_NonInverting = 0x20, /**< Clear on compare match, set at BOTTOM */
    Timer0_OC0_Inverting    = 0x30  /**< Set on compare match, clear at BOTTOM */
} Timer0_OC0_Mode_t;

/**
 * @brief Defines Timer0 operation status codes.
 */
typedef enum
{
    Timer0_Status_Ok    = 0, /**< Operation completed successfully */
    Timer0_Status_Error = 1  /**< An error occurred */
} Timer0_Status_t;
/**
 * @fn mTimer0_Init_FastPWM
 * @brief Initializes Timer0 in 8-bit Fast PWM mode.
 * @details Configures non-inverting Fast PWM with a prescaler of 64,
 * configures PB4 (OC0) as an output, and initializes the output
 * to a safe OFF state. Timer0 interrupts remain disabled.
 * @note PWM waveform generation is handled by hardware.
 */
void mTimer0_Init_FastPWM(void);

/**
 * @fn mTimer0_SetDutyCycle
 * @brief Sets the PWM duty cycle using an 8-bit compare value.
 * @details Updates OCR0 and handles the zero-duty output state
 * to prevent unwanted output spikes.
 * @param[in] DutyValue Compare value from 0 to 255.
 * @note A value of 255 requests full duty.
 */
void mTimer0_SetDutyCycle(uint8_t DutyValue);

/**
 * @fn mTimer0_SetDutyPercentage
 * @brief Sets the PWM duty cycle as a percentage.
 * @details Converts a percentage to an 8-bit compare value.
 * Values above 100 are clamped to 100.
 * @param[in] DutyPercent Requested duty percentage.
 * @note Valid percentage range is 0 to 100.
 */
void mTimer0_SetDutyPercentage(uint8_t DutyPercent);

/**
 * @fn mTimer0_Stop
 * @brief Stops Timer0 and forces the PWM output LOW.
 * @details Disconnects OC0, stops the timer clock, and resets
 * the compare value to zero.
 */
void mTimer0_Stop(void);

/**
 * @fn mTimer0_Start
 * @brief Starts Timer0 using the configured Fast PWM mode.
 * @details Resumes the timer clock with a prescaler of 64
 * and restores the appropriate OC0 output behavior.
 */
void mTimer0_Start(void);

/**
 * @fn mTimer0_GetDutyCycle
 * @brief Retrieves the current OCR0 compare value.
 * @return The 8-bit compare value ranging from 0 to 255.
 */
uint8_t mTimer0_GetDutyCycle(void);

/**
 * @fn mTimer0_GetFrequency_Hz
 * @brief Retrieves the nominal PWM frequency in Hertz.
 * @return The nominal PWM frequency, approximately 977 Hz.
 */
uint16_t mTimer0_GetFrequency_Hz(void);

#endif /* TIMER0_INTERFACE_H */
