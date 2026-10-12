/**
* 
* @file       TIMER2_Interface.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Salma Mohamed)
* @brief      Interface file for the TIMER2 driver
* @details    This header file defines the compile-time interface of the TIMER2 driver
* @version    1.0.0
* @date       17/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#ifndef TIMER2_INTERFACE_H
#define TIMER2_INTERFACE_H

#include <stdint.h>

#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include "../../Common/MacroFunction.h"

#include "TIMER2_private.h"
#include "TIMER2_config.h" 

#include "../Atmega128Register.h"
/**
 * @brief Initializes Timer2 in CTC mode with prescaler 1024 and OCR2=155.
 *        Enables Compare Match Interrupt (OCIE2) to generate 10ms heartbeats.
 */
void            mTimer2_Init_CTC_10ms(void);

/**
 * @brief Returns the system uptime in milliseconds since boot.
 *        Guaranteed 100% atomic (protected against 32-bit register tearing).
 * @return uint32_t Milliseconds elapsed since ECU power-on.
 */
uint32_t        mTimer2_GetTick_ms(void);

/**
 * @brief Returns the system uptime in complete seconds.
 *        Thread-safe atomic read wrapper.
 * @return uint32_t Seconds elapsed since ECU power-on.
 */
uint32_t        mTimer2_GetUptime_Seconds(void);

/**
 * @brief Non-drifting delay function based on SysTick timestamp comparison.
 *        Avoids timer slip and allows background ISR execution.
 * @param Delay_ms Milliseconds to wait.
 */
void            mTimer2_DelayMs(uint32_t Delay_ms);

/**
 * @brief Resets the tick counter to 0 (Used during ECU soft restart / sleep exit).
 */
void            mTimer2_ResetTick(void);

/**
 * @brief Registers a custom OS scheduler tick hook function invoked from ISR.
 * @param CallbackPtr Pointer to void(void) scheduler hook.
 */
void            mTimer2_SetCallback(Timer2_TickHook_t CallbackPtr);

#endif /* TIMER2_INTERFACE_H */

