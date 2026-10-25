/**
 * @file Buzzer_Interface.h
 * @author DEV salma mohamed mahmoud fouad(salmafouadothman@gmail.com) 
 * @author REV Abdulrahman Ali 
 * @brief  Driver control the buzzer 
 * @details this driver control the buzzer intilization it being 
 *          on or off it read the buzzer state  it can toggle the buzzer 
 *          we can make the buzzer "beep"at a certain frequency we can set it state
 *          to either on or off 
 * @version 0.1
 * @date 2026-10-21
 * @copyright Copyright (c) 2026 GESTELL
 */
#ifndef BUZZER_INTERFACE_H_
#define BUZZER_INTERFACE_H_

#include"../../Common/Config.h"
#include"Buzzer_Config.h"
#include"Buzzer_Private.h"

/**
 * @fn hBUZZER_Init
 * @brief Initializes GroupD Pin7 as a digital output and sets initial silent state.
 */
void hBUZZER_Init(void);

/**
 * @fn hBUZZER_SetState
 * @brief Sets the static state of the buzzer (ON or OFF).
 * @param state Desired state (BUZZER_STATE_OFF or BUZZER_STATE_ON).
 * @return Std_ReturnType EOk if successful, ENotOk if parameter invalid.
 */
Std_ReturnType hBUZZER_SetState(BuzzerState_t state);

/**
 * @fn hBUZZER_On
 * @brief Activates continuous acoustic alarm tone.
 */
void hBUZZER_On(void);

/**
 * @fn hBUZZER_Off
 * @brief Deactivates and silences the buzzer immediately.
 */
void hBUZZER_Off(void);

/**
 * @fn hBUZZER_Toggle
 * @brief Inverts the physical output pin logic level (used for tone synthesis).
 */
void hBUZZER_Toggle(void);

/**
 * @fn hBUZZER_SetBeep
 * @brief Configures intermittent beep mode at specified frequency.
 * @param beepFreq_hz Beep repetition frequency in Hz (e.g. 2 for 2 Hz beep).
 * @return Std_ReturnType EOk if valid, ENotOk if beepFreq_hz is 0.
 */
Std_ReturnType hBUZZER_SetBeep(uint8_t beepFreq_hz);

/**
 * @fn hBUZZER_GetState
 * @brief Inquires the current operational state of the buzzer.
 * @return BuzzerState_t BUZZER_STATE_ON if active, BUZZER_STATE_OFF if silent.
 */
BuzzerState_t hBUZZER_GetState(void);

/**
 * @fn hBUZZER_UpdatePeriodic
 * @brief Periodic time-slice handler for non-blocking pulsed beeping.
 *        Called by OS scheduler (e.g. every 10 ms).
 * @param step_ms Time elapsed in milliseconds since last call.
 */
void hBUZZER_UpdatePeriodic(uint16_t step_ms);
/* Compatibility macros */
#define Buzzer_Init()                  hBUZZER_Init()
#define Buzzer_SetState(st)            hBUZZER_SetState((st))
#define Buzzer_On(freq)                hBUZZER_On()
#define Buzzer_Off()                   hBUZZER_Off()
#define Buzzer_Toggle()                hBUZZER_Toggle()
#define Buzzer_GetState()              hBUZZER_GetState()
#define Buzzer_Update(step)            hBUZZER_UpdatePeriodic((step))

#endif