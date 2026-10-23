/**
* 
* @file       LED_Interface.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Rouaa Ayman )
* @brief      Interface file for the LED driver
* @details    This header file defines the public API for the LED driver
* @version    1.0.0
* @date       19/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#ifndef LED_INTERFACE_H
#define LED_INTERFACE_H

/* Public library */
#include <stdint.h>

/* common library */
#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"
#include "../../../Common/MacroFunction.h"

/* led library */
#include "LED_Private.h"
#include "LED_Config.h" 

/* other dependencies */
#include "../../../Mcal/Dio/DIO_Interface.h"


/* =========================================================================
   PUBLIC API FUNCTION PROTOTYPES
   ========================================================================= */

/**
 * @fn void hLED_Init(void)
 * @brief Initializes all LED pins as outputs (GroupD Pin4, Pin5, Pin6) and sets them OFF.
 */
void           hLED_Init(void);

/**
 * @fn Std_ReturnType hLED_SetState(LedId_t ledId, LedState_t state)
 * @brief Sets an LED to a static ON or OFF state. Cancels any active blinking.
 * @param ledId Target LED identifier.
 * @param state Desired state (LED_STATE_OFF or LED_STATE_ON).
 * @return Std_ReturnType EOk if valid, ENotOk if parameter out of range.
 */
Std_ReturnType hLED_SetState(LedId_t ledId, LedState_t state);

/**
 * @fn Std_ReturnType hLED_Toggle(LedId_t ledId)
 * @brief Toggles the current physical state of the specified LED.
 * @param ledId Target LED identifier.
 * @return Std_ReturnType EOk if valid, ENotOk if parameter out of range.
 */
Std_ReturnType hLED_Toggle(LedId_t ledId);

/**
 * @fn Std_ReturnType hLED_Blink(LedId_t ledId, uint8_t freq_hz)
 * @brief Configures an LED to blink at a given frequency in Hz without blocking.
 * @param ledId Target LED identifier.
 * @param freq_hz Flashing frequency in Hz (e.g. 1 for 1 Hz, 2 for 2 Hz).
 * @return Std_ReturnType EOk if valid, ENotOk if invalid id or freq_hz is 0.
 */
Std_ReturnType hLED_Blink(LedId_t ledId, uint8_t freq_hz);

/**
 * @fn Std_ReturnType hLED_StopBlink(LedId_t ledId)
 * @brief Stops blinking on the specified LED and turns it OFF.
 * @param ledId Target LED identifier.
 * @return Std_ReturnType EOk if valid, ENotOk if invalid id.
 */
Std_ReturnType hLED_StopBlink(LedId_t ledId);

/**
 * @fn void hLED_SetAll(LedState_t greenState, LedState_t blueState, LedState_t yellowState)
 * @brief Sets static states for all three LEDs in one call.
 * @param greenState State for Green LED.
 * @param blueState State for Blue LED.
 * @param yellowState State for Yellow LED.
 */
void           hLED_SetAll(LedState_t greenState, LedState_t blueState, LedState_t yellowState);

/**
 * @fn LedState_t hLED_GetState(LedId_t ledId)
 * @brief Returns the current physical state of the LED.
 * @param ledId Target LED identifier.
 * @return LedState_t Current state (LED_STATE_OFF or LED_STATE_ON).
 */
LedState_t     hLED_GetState(LedId_t ledId);

/**
 * @fn void hLED_UpdatePeriodic(uint16_t step_ms)
 * @brief Periodic handler to be called by the OS scheduler (e.g. every 10 ms).
 * @param step_ms Time elapsed since last call in ms (typically 10 ms).
 */
void           hLED_UpdatePeriodic(uint16_t step_ms);


#endif /* LED_INTERFACE_H */