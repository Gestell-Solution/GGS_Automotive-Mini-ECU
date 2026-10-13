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

static volatile uint32_t g_sysTick_ms     = 0UL;
static volatile uint32_t g_uptime_seconds = 0UL;
static volatile uint8_t  g_subSecondTicks = 0U;
static volatile Timer2_TickHook_t g_timer2Callback = Null;

void Scheduler_Tick_ISR(void) __attribute__((weak)); // Ensure the function is not optimized away
void Scheduler_Tick_ISR(void)
{
    // will be changed using the schudeler.c 
}

/**
 * @brief Initializes Timer2 in CTC mode with prescaler 1024 and OCR2=155.
 *        Enables Compare Match Interrupt (OCIE2) to generate 10ms heartbeats.
 */
void mTimer2_Init_CTC_10ms(void)
{
    
    // Reset counter
    TCNT2_Reg = 0;    

    // Set compare value for 10ms tick 
    OCR2_Reg = _10ms_tick; 
        
    // clear pending flag 
    SetBit(TIFR_Reg, OCF2); 
    
    // Enable Compare Match Interrupt
    SetBit(TIMSK_Reg, OCIE2); 

    // make sure that the values are set to zero
    g_sysTick_ms = 0UL;
    g_uptime_seconds = 0UL;
    g_subSecondTicks = 0U;
    g_timer2Callback = Null;
    
    // we set the timer 2 to CTC mode 
    // then we seclet the prescaler to 1024 only
    TCCR2_Reg |= (uint8_t)(Timer2_Mode_Select | Timer2_Prescaler_Select);
}

/**
 * @brief Returns the system uptime in milliseconds since boot.
 *        Guaranteed 100% atomic (protected against 32-bit register tearing).
 * @return uint32_t Milliseconds elapsed since ECU power-on.
 */
uint32_t mTimer2_GetTick_ms(void)
{
    // this to copy the currect tick
    uint32_t tick_copy;
    // this to save the current state
    uint8_t sreg_save = SREG_Reg;

    /* Critical Section: Disable interrupts to prevent 4-byte tearing */
    cli();
    tick_copy = g_sysTick_ms; 
    SREG_Reg = sreg_save;

    return tick_copy;
}

/**
 * @brief Returns the system uptime in complete seconds.
 *        Thread-safe atomic read wrapper.
 * @return uint32_t Seconds elapsed since ECU power-on.
 */
uint32_t mTimer2_GetUptime_Seconds(void)
{
    uint32_t up_copy;
    uint8_t sreg_save = SREG_Reg;

    /* Critical Section: Disable interrupts to prevent 4-byte tearing */
    cli();
    up_copy = g_uptime_seconds;
    SREG_Reg = sreg_save;

    return up_copy;
}

/**
 * @brief Non-drifting delay function based on SysTick timestamp comparison.
 *        Avoids timer slip and allows background ISR execution.
 * @param Delay_ms Milliseconds to wait.
 */
void mTimer2_DelayMs(uint32_t Delay_ms)
{
    uint32_t start_time = mTimer2_GetTick_ms();
    

    while ((mTimer2_GetTick_ms() - start_time) < Delay_ms)
    {
        // Busy-wait loop until the specified delay has elapsed
        // This allows background ISRs to continue executing
    }

}

/**
 * @brief Resets the tick counter to 0 (Used during ECU soft restart / sleep exit).
 */
void mTimer2_ResetTick(void)
{
    uint8_t sreg_save = SREG_Reg;

    /* Critical Section: Disable interrupts to prevent 4-byte tearing */
    cli();
    g_sysTick_ms = 0UL;
    g_uptime_seconds = 0UL;
    g_subSecondTicks = 0U;
    TCNT2_Reg= 0x00; // Reset the Timer2 counter to 0
    SREG_Reg = sreg_save;

}

/**
 * @brief Registers a custom OS scheduler tick hook function invoked from ISR.
 * @param CallbackPtr Pointer to void(void) scheduler hook.
 */
void mTimer2_SetCallback(Timer2_TickHook_t CallbackPtr)
{
    g_timer2Callback = CallbackPtr;
}

// it triggers every 9.984ms
ISR(TIMER2_COMP_vect)
{
    g_sysTick_ms += 10; // incremnt by 10ms

    if ((g_sysTick_ms % 1000U) == 0U)
    {
        g_uptime_seconds++;
    }

    if (g_timer2Callback != Null)
    {
        g_timer2Callback();
    }
    else 
    {
        Scheduler_Tick_ISR();
    }
}


#endif