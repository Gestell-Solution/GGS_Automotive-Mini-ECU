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

#if ModuleLedEnabled == Enable 

#include "../../../Common/Config.h"

#include "LED_Interface.h"

/**
 * @fn void hLED_Init(void)
 * @brief Initializes all LED pins as outputs (GroupD Pin4, Pin5, Pin6) and sets them OFF.
 */
void hLED_Init(void)
{
    // remove this commets
    /*
        init the group d pin 4 , 5 , 6
        where 
        4 is green 
        5 is blue
        6 is yellow
        using a for loop
        from i to LED_count-1 in ledId_t enum
        set state -> off
        set mode -> static
        set hp -> 0 
        set tickcounter ->0
        we call initpin -> output
        we call writepin -> low
    */
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
    // remove these comments
    /*
        if ledid > ledcount -> Enotok (from std_returntype in defination.h)
        the .mode -> static
        the .tick = 0 
        the .state = state 
        if we write pin and it return high then we do Eok
    */

}

/**
 * @fn Std_ReturnType hLED_Toggle(LedId_t ledId)
 * @brief Toggles the current physical state of the specified LED.
 * @param ledId Target LED identifier.
 * @return Std_ReturnType EOk if valid, ENotOk if parameter out of range.
 */
Std_ReturnType hLED_Toggle(LedId_t ledId)
{
    // remove
    /*
        same with the setstate but 
        insted of use write pin we use tog pin
    */
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
    //remove
    /*
        ledid or frq = 0 --> Enotok
        halfperiod = calc freq -> 1000/(2*freq_hz)
        .tick = 0
        .mode =blincking
        .state = on
        writepin high
        return Eok
    */
}

/**
 * @fn Std_ReturnType hLED_StopBlink(LedId_t ledId)
 * @brief Stops blinking on the specified LED and turns it OFF.
 * @param ledId Target LED identifier.
 * @return Std_ReturnType EOk if valid, ENotOk if invalid id.
 */
Std_ReturnType hLED_StopBlink(LedId_t ledId)
{
    //remove
    /*
        ledid > ledcount ->Enotok
        all zero
        .state = off
        write low
        return Eok
    */
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
    //remove
    /*
        setsate g , b ,y 
    */
}

/**
 * @fn LedState_t hLED_GetState(LedId_t ledId)
 * @brief Returns the current physical state of the LED.
 * @param ledId Target LED identifier.
 * @return LedState_t Current state (LED_STATE_OFF or LED_STATE_ON).
 */
LedState_t hLED_GetState(LedId_t ledId)
{
    //remove
    /*
        ledid > ledcount -> off
        return .state
    */
}

/**
 * @fn void hLED_UpdatePeriodic(uint16_t step_ms)
 * @brief Periodic handler to be called by the OS scheduler (e.g. every 10 ms).
 * @param step_ms Time elapsed since last call in ms (typically 10 ms).
 */
void hLED_UpdatePeriodic(uint16_t step_ms)
{
    //remove 
    /*
        if setp_ms = 0 --> stop
        for i from 0 to ledcount -1 
        .mode -> blinking
        tick += step_ms 
            if tick > half
            tick -= half
            invert .state
            togpin
    */
}




#endif /* ModuleLedEnabled == Enable */