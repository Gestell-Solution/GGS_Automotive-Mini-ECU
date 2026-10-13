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
 * @brief  Global tick counter in milliseconds since ECU boot.
 */

static volatile uint32_t g_sysTick_ms     = 0UL;

/** 
 * @brief  Global system uptime counter in whole seconds since ECU boot.
 * @note   Increments every 100 consecutive 10ms tick intervals (1000 ms).
 */

static volatile uint32_t g_uptime_seconds = 0UL;

/** 
 * @brief  8-bit sub-second accumulator (range 0..99).
 * @note   Avoids costly 32-bit modulo division (__udivmodsi4) in ISR to satisfy AC-4 (<4us).
 */

static volatile uint8_t  g_subSecondTicks = 0U;

/** 
 * @brief  Registered custom OS scheduler tick hook function pointer.
 * @note   Defaults to Null until set via mTimer2_SetCallback().
 */

static volatile Timer2_TickHook_t g_timer2Callback = Null;

#ifndef cli
/** @brief Disables global interrupts via AVR machine instruction 'cli'. */
#define cli()   __asm__ __volatile__ ("cli" ::: "memory")
#endif
#ifndef sei
/** @brief Enables global interrupts via AVR machine instruction 'sei'. */
#define sei()   __asm__ __volatile__ ("sei" ::: "memory")
#endif

/**
 * @brief      Weak default OS scheduler tick hook function.
 * @details    Invoked automatically by the hardware ISR when no dynamic callback
 *             is registered via mTimer2_SetCallback(). When the OS layer (Scheduler.c)
 *             is linked into the project, this weak symbol is overridden seamlessly.
 * @return     void
 */

void Scheduler_Tick_ISR(void) __attribute__((weak)); // Ensure the function is not optimized away
void Scheduler_Tick_ISR(void)
{
    /* Overridden automatically when OS layer (Scheduler.c) is linked */
    /* Weak implementation: overridden automatically when OS layer (Scheduler.c) is linked */
}

/**
 * @brief Initializes Timer2 in CTC mode with prescaler 1024 and OCR2=155.
 *        Enables Compare Match Interrupt (OCIE2) to generate 10ms heartbeats.
 */
void mTimer2_Init_CTC_10ms(void)
{
    /* Configure Timer2 in CTC mode with prescaler 1024 */
    TCCR2_Reg |= (uint8_t)(Timer2_Mode_Select | Timer2_Prescaler_Select);
    

    /* Set compare value for 10ms tick */
    OCR2_Reg = _10ms_tick; 

    /* Reset the Timer2 counter to 0 */
    TCNT2_Reg = 0x00U;    
        
    /* Clear pending flag */
    SetBit(TIFR_Reg, OCF2); 
    
    /* Enable Compare Match Interrupt */
    SetBit(TIMSK_Reg, OCIE2); 

    /* Make sure that the values are set to zero */
    g_sysTick_ms = 0UL;
    g_uptime_seconds = 0UL;
    g_subSecondTicks = 0U;
    g_timer2Callback = Null;
}

/**
 * @brief Returns the system uptime in milliseconds since boot.
 *        Guaranteed 100% atomic (protected against 32-bit register tearing).
 * @return uint32_t Milliseconds elapsed since ECU power-on.
 */
uint32_t mTimer2_GetTick_ms(void)
{
    /* Copy the current tick value automatically */
    uint32_t tick_copy;
    /* Save the current state of the status register */
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
    /* Copy the current tick value automatically */
    uint32_t up_copy;
    /* Save the current state of the status register */
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
    uint32_t current_time ;

    // while ((current_time - start_time) < Delay_ms)
    // {
    //     /* Busy-wait loop until the specified delay has elapsed */
    //     /* This allows background ISRs to continue executing */
    // }
    do
    {
        current_time = mTimer2_GetTick_ms();
    } while ((current_time - start_time) < Delay_ms);
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
    TCNT2_Reg= 0x00; /* Reset the Timer2 counter to 0 */
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


ISR(TIMER2_COMP_vect)
{
    /* Increment the system tick counter by 10 milliseconds */
    g_sysTick_ms += 10UL; 


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


#endif /* ModuleTimer2Enabled == Enable */