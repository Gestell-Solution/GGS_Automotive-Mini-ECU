/**
* 
* @file       LED_Program.c
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Rouaa Ayman )
* @brief      Program file for the LED driver
* @details    This source file contains the implementation of the LED driver functions
* @version    1.0.0
* @date       19/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/
#include "../../../Common/Definition.h"
#include "../../../Common/Config.h"

#if (ModuleLedEnabled == Enable) 

#include "LED_Interface.h"


/**
 * @brief Static runtime control block for all managed LED channels.
 * @note  Statically allocated in .bss (< 35 bytes total, zero malloc).
 */
static LedControlBlock_t g_ledControl[LED_COUNT];

/**
 * @fn void hLED_Init(void)
 * @brief Initializes all LED pins as outputs (GroupD Pin4, Pin5, Pin6) and sets them OFF.
 */

void hLED_Init(void)
{
    uint8_t count = 0U ;
    /* define the group and pins for the leds */
    g_ledControl[LED_POWER_GREEN].group = LED_GREEN_PORT;
    g_ledControl[LED_POWER_GREEN].pin   = LED_GREEN_PIN;

    g_ledControl[LED_STATUS_BLUE].group = LED_BLUE_PORT;
    g_ledControl[LED_STATUS_BLUE].pin   = LED_BLUE_PIN;

    g_ledControl[LED_WARNING_YELLOW].group = LED_YELLOW_PORT;
    g_ledControl[LED_WARNING_YELLOW].pin   = LED_YELLOW_PIN;


    for (count = 0U ; count < LED_COUNT ; count++ )
    {
        /* we set all the pareametrs of led to off */
        g_ledControl[count].physicalState = LED_STATE_OFF;
        g_ledControl[count].mode = LED_MODE_STATIC;
        g_ledControl[count].halfPeriod_ms = 0U;
        g_ledControl[count].tickCounter_ms = 0U;
        /* we init the group each time calling the group and pin and setting it to high */
        mDIO_InitPin
        ((uint8_t)g_ledControl[count].group , (uint8_t)g_ledControl[count].pin, DIO_Output);
        /* we write low on the pins */
        mDIO_WritePin  
        ((uint8_t)g_ledControl[count].group , (uint8_t)g_ledControl[count].pin, Dio_Low);       
    }

}

/**
 * @fn Std_ReturnType hLED_SetState(LedId_t ledId, LedState_t state)
 * @brief Sets an LED to a static ON or OFF state. Cancels any active blinking.
 * @param ledId Target LED identifier.
 * @param state Desired state (LED_STATE_OFF or LED_STATE_ON).
 * @return Std_ReturnType EOk if valid, ENotOk if parameter out of range.
 */
Std_ReturnType hLED_SetState(LedId_t ledId, LedState_t state)
{
    /* check if the ledid we gave is more than the number of acutal leds*/
    if (ledId >= LED_COUNT)
    {
        return ENotOk ;
    }

    /* we set the state */
    g_ledControl[ledId].mode = LED_MODE_STATIC;
    g_ledControl[ledId].tickCounter_ms = 0U;
    g_ledControl[ledId].physicalState = state;

    /* we test and write the pin and see if it return on */
    mDIO_WritePin
    (g_ledControl[ledId].group, g_ledControl[ledId].pin, (state == LED_STATE_ON) ? Dio_High : Dio_Low) ;

    return EOk ;

}

/**
 * @fn Std_ReturnType hLED_Toggle(LedId_t ledId)
 * @brief Toggles the current physical state of the specified LED.
 * @param ledId Target LED identifier.
 * @return Std_ReturnType EOk if valid, ENotOk if parameter out of range.
 */
Std_ReturnType hLED_Toggle(LedId_t ledId)
{
    /* check if the ledid we gave is more than the number of acutal leds*/
    if (ledId >= LED_COUNT)
    {
        return ENotOk ;
    }

    /* we set the state */
    if (g_ledControl[ledId].physicalState == LED_STATE_ON)
    {
        g_ledControl[ledId].physicalState = LED_STATE_OFF;
    }
    else 
    {
        g_ledControl[ledId].physicalState = LED_STATE_ON ;
    }

    /* we test and write the pin and see if it return on */
    mDIO_TogglePin
    (g_ledControl[ledId].group, g_ledControl[ledId].pin) ;

    return EOk ;

}


/**
 * @fn Std_ReturnType hLED_Blink(LedId_t ledId, uint8_t freq_hz)
 * @brief Configures an LED to blink at a given frequency in Hz without blocking.
 * @param ledId Target LED identifier.
 * @param freq_hz Flashing frequency in Hz (e.g. 1 for 1 Hz, 2 for 2 Hz).
 * @return Std_ReturnType EOk if valid, ENotOk if invalid id or freq_hz is 0.
 */
Std_ReturnType hLED_Blink(LedId_t ledId, uint8_t freq_hz)
{
    /* check if the ledid we gave is more than the number of acutal leds*/
    if (ledId >= LED_COUNT || freq_hz == 0 )
    {
        return ENotOk ;
    }
    /* calcualte the half perodic time */
    uint16_t calchalfperiod_ms = 1000U / (2 * freq_hz);

    /* set the staes */
    g_ledControl[ledId].halfPeriod_ms = calchalfperiod_ms;
    g_ledControl[ledId].tickCounter_ms = 0U ;
    g_ledControl[ledId].mode = LED_MODE_BLINKING;
    g_ledControl[ledId].physicalState= LED_STATE_ON ;


    mDIO_WritePin(g_ledControl[ledId].group, g_ledControl[ledId].pin, Dio_High);

    return EOk;

}

/**
 * @fn Std_ReturnType hLED_StopBlink(LedId_t ledId)
 * @brief Stops blinking on the specified LED and turns it OFF.
 * @param ledId Target LED identifier.
 * @return Std_ReturnType EOk if valid, ENotOk if invalid id.
 */
Std_ReturnType hLED_StopBlink(LedId_t ledId)
{
    /* check if the ledid we gave is more than the number of acutal leds*/
    if (ledId >= LED_COUNT)
    {
        return ENotOk ;
    }

    /* set the staes */
    g_ledControl[ledId].mode           = LED_MODE_STATIC;
    g_ledControl[ledId].halfPeriod_ms  = 0U;
    g_ledControl[ledId].tickCounter_ms = 0U;
    g_ledControl[ledId].physicalState  = LED_STATE_OFF;

    /* stop the blinking by writing zero */
    mDIO_WritePin(g_ledControl[ledId].group, g_ledControl[ledId].pin, Dio_Low);

    return EOk;
}

/**
 * @fn void hLED_SetAll(LedState_t greenState, LedState_t blueState, LedState_t yellowState)
 * @brief Sets static states for all three LEDs in one call.
 * @param greenState State for Green LED.
 * @param blueState State for Blue LED.
 * @param yellowState State for Yellow LED.
 */
void hLED_SetAll(LedState_t greenState, LedState_t blueState, LedState_t yellowState)
{
    hLED_SetState(LED_POWER_GREEN, greenState);
    hLED_SetState(LED_STATUS_BLUE, blueState);
    hLED_SetState(LED_WARNING_YELLOW, yellowState);
}

/**
 * @fn LedState_t hLED_GetState(LedId_t ledId)
 * @brief Returns the current physical state of the LED.
 * @param ledId Target LED identifier.
 * @return LedState_t Current state (LED_STATE_OFF or LED_STATE_ON).
 */
LedState_t hLED_GetState(LedId_t ledId)
{
    /* check if the ledid we gave is more than the number of acutal leds*/
    if ((uint8_t)ledId >= (uint8_t)LED_COUNT)
    {
        return LED_STATE_OFF;
    }

    /* return the physical state of the specified LED */
    return g_ledControl[ledId].physicalState;
}

/**
 * @fn void hLED_UpdatePeriodic(uint16_t step_ms)
 * @brief Periodic handler to be called by the OS scheduler (e.g. every 10 ms).
 * @param step_ms Time elapsed since last call in ms (typically 10 ms).
 */
void hLED_UpdatePeriodic(uint16_t step_ms)
{
    uint8_t ledIdcounter = 0U;
    /* we check if the step ms is zero so we just stop*/
    if (step_ms == 0U)
    {
        return;
    }

    /* for loop to pass the over the leds */
    for (ledIdcounter = 0 ; ledIdcounter < LED_COUNT ; ledIdcounter++)
    {
        /* to see if it is blinking */
        if(g_ledControl[ledIdcounter].mode == LED_MODE_BLINKING)
        {
            /* update the tick */
            g_ledControl[ledIdcounter].tickCounter_ms += step_ms;
            
            /* we check to see if tick > half period*/
            if(g_ledControl[ledIdcounter].tickCounter_ms >= g_ledControl[ledIdcounter].halfPeriod_ms)
            {
                /* if it is her decreases by the half period */
                g_ledControl[ledIdcounter].tickCounter_ms -= g_ledControl[ledIdcounter].halfPeriod_ms ;
                /* invert the state */
                if (g_ledControl[ledIdcounter].physicalState == LED_STATE_ON)
                {
                    g_ledControl[ledIdcounter].physicalState = LED_STATE_OFF;
                }
                else 
                {
                    g_ledControl[ledIdcounter].physicalState = LED_STATE_ON ;
                }
                /* toggle the pin */
                mDIO_TogglePin(g_ledControl[ledIdcounter].group, g_ledControl[ledIdcounter].pin);

            }
        }
    }
}


#endif /* ModuleLedEnabled == Enable */