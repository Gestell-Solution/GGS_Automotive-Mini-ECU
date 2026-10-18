/**
 * @file    Timer0_Program.c
 * @brief   Implements the Timer0 Fast PWM driver functions.
 * @details Configures Timer0 registers for 8-bit Fast PWM operation,
 *          controls the OC0 output and PWM duty cycle, and provides
 *          timer start/stop functionality. Timer0 interrupts remain disabled.
 * @version 1.0.1
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @date    2026-10-13
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "Timer0_Interface.h"
#include "Timer0_Private.h"
#include "Timer0_Config.h"
#include "Atmega128Register.h"
#include "MacroFunction.h"

/**
 * @fn mTimer0_Init_FastPWM
 * @brief Initializes Timer0 in 8-bit Fast PWM mode.
 * @details Configures Timer0 with a prescaler of 64, sets PB4 (OC0)
 *          as an output, disables Timer0 interrupts, and initializes
 *          the output to LOW with a duty cycle of 0%.
 * @note The nominal PWM frequency is approximately 977 Hz.
 */

void mTimer0_Init_FastPWM(void)
{
/* Stop Timer0 before changing its configuration */
ClearBit(TCCR0_Reg, 2);
ClearBit(TCCR0_Reg, 1);
ClearBit(TCCR0_Reg, 0);

/* Disconnect OC0 from the timer output */
ClearBit(TCCR0_Reg, 5);
ClearBit(TCCR0_Reg, 4);

/* Configure PB4/OC0 as output and force it LOW */
SetBit(DDRB_Reg, 4);
ClearBit(PORTB_Reg, 4);

/* Initial Duty Cycle = 0% */
OCR0_Reg = 0U; 

/* Disable Timer0 interrupts */
ClearBit(TIMSK_Reg, 0); 
ClearBit(TIMSK_Reg, 1); 

/* Set Timer0 to Fast PWM Mode */
SetBit(TCCR0_Reg, 6);
SetBit(TCCR0_Reg, 3);

/* Start Timer0 with a prescaler of 64 */
SetBit(TCCR0_Reg, 2);
ClearBit(TCCR0_Reg, 1);
ClearBit(TCCR0_Reg, 0);
}

/**
 * @fn mTimer0_SetDutyCycle
 * @brief Sets the Timer0 PWM duty cycle using an 8-bit compare value.
 * @details Updates OCR0 with the requested compare value. For a value
 *          of 0, disconnects OC0 and forces PB4 LOW. For a value of 255,
 *          disconnects OC0 and forces PB4 HIGH. For values from 1 to 254,
 *          enables non-inverting PWM on OC0.
 * @param[in] DutyValue The requested compare value, ranging from 0 to 255.
 * @warning Timer0 must be initialized before using this function.
 */
void mTimer0_SetDutyCycle(uint8_t DutyValue)
{
if (DutyValue == 0U)
    {
    /* Disconnect OC0 and force the output LOW */
    OCR0_Reg = DutyValue;
    ClearBit(TCCR0_Reg, 4);
    ClearBit(TCCR0_Reg, 5);
    ClearBit(PORTB_Reg, 4);
    }

else if (DutyValue==255U)
    {
    /* Disconnect OC0 and force the output HIGH */
    OCR0_Reg=DutyValue; 
    ClearBit(TCCR0_Reg,4);
    ClearBit(TCCR0_Reg,5);
    SetBit(PORTB_Reg,4);
    }
    else
    {
    /* Enable non-inverting PWM on OC0 */
    OCR0_Reg = DutyValue;
    SetBit(TCCR0_Reg, 5);
    ClearBit(TCCR0_Reg, 4);
    }
}
/**
 * @fn mTimer0_SetDutyPercentage
 * @brief Sets the Timer0 PWM duty cycle as a percentage.
 * @details Limits the requested percentage to 100%, converts it to an
 *          8-bit compare value, and applies it using mTimer0_SetDutyCycle().
 * @param[in] DutyPercent The requested duty cycle percentage.
 * @note Values above 100% are clamped to 100%.
 * @warning Timer0 must be initialized before using this function.
 */
void     mTimer0_SetDutyPercentage(uint8_t DutyPercent)
{
uint16_t DutyValue;
/* Clamp the percentage to the valid range */
    if (DutyPercent > 100U)
    {
        DutyPercent = 100U;
    }
/* Convert percentage to an 8-bit compare value */  
DutyValue=((uint16_t) DutyPercent * 255U / 100U);
/* Apply the calculated duty cycle */
mTimer0_SetDutyCycle((uint8_t) DutyValue);
}

/**
 * @fn mTimer0_Stop
 * @brief Stops Timer0 and forces the PB4 output LOW.
 * @details Disconnects OC0, stops the Timer0 clock, forces PB4 LOW,
 *          and resets OCR0 to zero.
 * @note Timer0 interrupts remain disabled.
 */
void     mTimer0_Stop(void)
{
/* Prepare LOW level before disconnecting OC0 */
ClearBit(PORTB_Reg, 4);
/* Disconnect OC0 from the timer output */
ClearBit(TCCR0_Reg,4);
ClearBit(TCCR0_Reg,5);
/* Stop Timer0 by clearing the clock select bits */
ClearBit(TCCR0_Reg,0);
ClearBit(TCCR0_Reg,1);
ClearBit(TCCR0_Reg,2);
/* Reset the duty cycle */
ClearBit(PORTB_Reg,4);
OCR0_Reg=0U;
}

/**
 * @fn mTimer0_Start
 * @brief Starts Timer0 in Fast PWM mode with a prescaler of 64.
 * @details Restores Fast PWM operation and the clock prescaler.
 *          Configures PB4 according to OCR0: LOW for 0, HIGH for 255,
 *          or non-inverting PWM for values from 1 to 254.
 * @warning Timer0 must be initialized before using this function.
 */
void     mTimer0_Start(void)
{
/* Configure Timer0 for 8-bit Fast PWM mode */
SetBit(TCCR0_Reg,6);
SetBit(TCCR0_Reg,3);

if (OCR0_Reg==0U)
{
/* Force PB4 LOW for 0% duty cycle */
ClearBit(PORTB_Reg,4);
ClearBit(TCCR0_Reg,4);
ClearBit(TCCR0_Reg,5);
}
else if (OCR0_Reg==255U)
{
/* Force PB4 HIGH for 100% duty cycle */
SetBit(PORTB_Reg,4);
ClearBit(TCCR0_Reg, 4);
ClearBit(TCCR0_Reg, 5);
}
else
{
/* Enable non-inverting PWM on OC0 */
ClearBit(TCCR0_Reg,4);
SetBit(TCCR0_Reg,5);
}

/* Start Timer0 with a prescaler of 64 */
SetBit(TCCR0_Reg, 2);
ClearBit(TCCR0_Reg, 1);
ClearBit(TCCR0_Reg, 0);
}
/**
 * @fn mTimer0_GetDutyCycle
 * @brief Retrieves the current Timer0 compare value.
 * @details Reads the value stored in the OCR0 register.
 * @return The current 8-bit compare value, ranging from 0 to 255.
 */
uint8_t  mTimer0_GetDutyCycle(void)
{
/* Return the current Timer0 compare value */
return OCR0_Reg;
}
/**
 * @fn mTimer0_GetFrequency_Hz
 * @brief Retrieves the nominal Timer0 Fast PWM frequency in Hertz.
 * @details Returns the frequency calculated for a 16 MHz CPU clock,
 *          Fast PWM mode, and a prescaler of 64.
 * @return The nominal PWM frequency of 977 Hz.
 */
uint16_t mTimer0_GetFrequency_Hz(void)
{
/* Return the nominal Fast PWM frequency in Hertz */
return 977U;
}
