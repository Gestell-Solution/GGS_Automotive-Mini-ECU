/**
* 
* @file       TIMER2_program.c
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Salma Mohamed)
* @brief      Program file for the TIMER2 driver
* @details    This source file contains the implementation of the TIMER2 driver functions
* @version    1.0.0
* @date       17/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/


#include "../../Common/Config.h"

#if ModuleTimer2Enable == Enable 

#include "TIMER2_interface.h"

/**
 * @brief Initializes Timer2 in CTC mode with prescaler 1024 and OCR2=155.
 *        Enables Compare Match Interrupt (OCIE2) to generate 10ms heartbeats.
 */
void mTimer2_Init_CTC_10ms(void)
{
    // we set the timer 2 to CTC mode 
    TCCR2_Reg |= (Timer2_Mode_Select | Timer2_Prescaler_Select);
    
    // then we seclet the prescaler to 1024 only
    // TCCR2_Reg |= Timer2_Prescaler_Select;
    // Set compare value for 10ms tick 
    OCR2_Reg = _10ms_tick; 

    // Reset counter
    TCNT2_Reg = 0;    

    // clear pending flag 
    SetBit(TIFR_Reg, OCF2); 

    // Enable Compare Match Interrupt
    SetBit(TIMSK_Reg, OCIE2); 


    static uint32_t g_sysTick_ms      = 0; // Global tick counter in milliseconds
    static uint32_t g_uptime_seconds  = 0; 
    static void (*g_timer2Callback)(void) = 0; 
}

/**
 * @brief Returns the system uptime in milliseconds since boot.
 *        Guaranteed 100% atomic (protected against 32-bit register tearing).
 * @return uint32_t Milliseconds elapsed since ECU power-on.
 */
uint32_t mTimer2_GetTick_ms(void)
{

}

/**
 * @brief Returns the system uptime in complete seconds.
 *        Thread-safe atomic read wrapper.
 * @return uint32_t Seconds elapsed since ECU power-on.
 */
uint32_t mTimer2_GetUptime_Seconds(void)
{

}

/**
 * @brief Non-drifting delay function based on SysTick timestamp comparison.
 *        Avoids timer slip and allows background ISR execution.
 * @param Delay_ms Milliseconds to wait.
 */
void mTimer2_DelayMs(uint32_t Delay_ms)
{

}

/**
 * @brief Resets the tick counter to 0 (Used during ECU soft restart / sleep exit).
 */
void mTimer2_ResetTick(void)
{

}

/**
 * @brief Registers a custom OS scheduler tick hook function invoked from ISR.
 * @param CallbackPtr Pointer to void(void) scheduler hook.
 */
void mTimer2_SetCallback(Timer2_TickHook_t CallbackPtr)
{

}





#endif