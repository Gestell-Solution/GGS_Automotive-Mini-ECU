/**
 * @file    Timer0_Config.h
 * @brief   Configuration header for Timer0 Fast PWM driver.
 * @details Defines compile-time configuration options for Timer0 operational modes and prescalers.
 * @version 1.0.0
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @date    2026-10-15
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

 #ifndef TIMER0_CONFIG_H
 #define TIMER0_CONFIG_H

 /**
 * @def TIMER0_DEFAULT_PRESCALER
 * @brief Defines the default clock prescaler for Timer0 (div 64 for ~977 Hz).
 */
#define TIMER0_DEFAULT_PRESCALER Timer0_Prescaler_64

/**
 * @def TIMER0_PWM_MODE
 * @brief Defines the default waveform generation mode for Timer0.
 */
#define TIMER0_PWM_MODE Timer0_Mode_FastPWM

#endif /* TIMER0_CONFIG_H */