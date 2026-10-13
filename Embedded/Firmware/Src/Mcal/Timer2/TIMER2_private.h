/**
* 
* @file       TIMER2_Private.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Salma Mohamed)
* @brief      Private file for the TIMER2 driver
* @details    This header file defines the private implementation details of the TIMER2 driver
* @version    1.0.0
* @date       17/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#ifndef TIMER2_PRIVATE_H
#define TIMER2_PRIVATE_H

/**
 * @enum Timer2_Mode_t
 * @brief Timer2 operational waveform generation modes (TCCR2 bits WGM21:20).
 */
typedef enum {
    Timer2_Mode_Normal          = 0x00,  /* Normal 8-bit counter */
    Timer2_Mode_PhaseCorrectPWM = 0x40,  /* Phase Correct PWM */
    Timer2_Mode_CTC             = 0x08,  /* Clear Timer on Compare Match (Top = OCR2) */
    Timer2_Mode_FastPWM         = 0x48   /* Fast PWM (Top = 0xFF) */
} Timer2_Mode_t;

/**
 * @enum Timer2_Prescaler_t
 * @brief Timer2 clock prescaler division factors (TCCR2 bits CS22:20).
 */
typedef enum {
    Timer2_Prescaler_None = 0,  /* Timer stopped (No clock) */
    Timer2_Prescaler_1    = 1,  /* clk / 1 */
    Timer2_Prescaler_8    = 2,  /* clk / 8 */
    // Timer2_Prescaler_32   = 3,  /* clk / 32 */
    Timer2_Prescaler_64   = 3,  /* clk / 64 */
    // Timer2_Prescaler_128  = 5,  /* clk / 128 */
    Timer2_Prescaler_256  = 4,  /* clk / 256 */
    Timer2_Prescaler_1024 = 5   /* clk / 1024 (Nominal for 10ms SysTick) */
} Timer2_Prescaler_t;

/**
 * @enum Timer2_OC2_Mode_t
 * @brief Compare match output pin (OC2 / PB7) behavior (TCCR2 bits COM21:20).
 */
typedef enum {
    Timer2_OC2_Disconnected = 0x00,  /* Normal port operation, OC2 disconnected */
    Timer2_OC2_Toggle       = 0x10,  /* Toggle OC2 on compare match */
    Timer2_OC2_Clear        = 0x20,  /* Clear OC2 on compare match */
    Timer2_OC2_Set          = 0x30   /* Set OC2 on compare match */
} Timer2_OC2_Mode_t;

/**
 * @enum Timer2_Status_t
 * @brief Timer2 execution status codes.
 */
typedef enum {
    Timer2_Status_Ok    = 0,
    Timer2_Status_Error = 1
} Timer2_Status_t;

/**
 * @typedef Timer2_TickHook_t
 * @brief Function pointer prototype for 10ms OS scheduler tick hook.
 */
typedef void (*Timer2_TickHook_t)(void);

/**
 * @defgroup VectorTable ATmega128 Interrupt Vector Table Mapping
 * @{
 * @brief In ATmega128, Timer/Counter2 Compare Match is Vector #9 in GCC
 *        (Flash address 0x0012, Vector 10 in Datasheet Table 30).
 */
#define TIMER2_COMP_vect_m    __vector_9

/**
 * @brief Standard bare-metal ISR macro using GCC signal attribute (Zero avr-libc library dependency).
 */
#ifndef ISR
#define ISR(vector)         void vector(void) __attribute__((signal, used)); \
                            void vector(void)
#endif
/** @} */


/**
 * @def   OCIE2
 * @brief this is the output compare match interput enable bit
 */

#define OCIE2  7

/**
 * @def   TOIE2
 * @brief this is the overflow match interput enable bit
 */

#define TOIE2  6


/**
 * @def   OCF2
 * @brief this is the  Output Compare Flag 2 bit
 */


 #define OCF2   7

 /**
 * @def   TOV2
 * @brief this is the Timer/Counter2 Overflow Flag bit
 */

 #define TOV2   6


#define _10ms_tick  155


#endif /* TIMER2_PRIVATE_H */