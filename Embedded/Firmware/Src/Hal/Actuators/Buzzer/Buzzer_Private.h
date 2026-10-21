/**
 * @file Buzzer_Private.h
 * @author DEV salma mohamed mahmoud fouad(salmafouadothman@gmail.com) 
 * @author REV Abdulrahman Ali 
 * @brief drive that contain all the state and mode
 * @details in this driver you can choose in mode for the buzzer to continue to beep or to 
 *          beep with pulse, you can choose the state whether on or off,in ther
 *          control block you can choose the group the pin the state mode the beep 
 *          period and the number of tick counter.
 * @version 0.1
 * @date 2026-10-21
 * @copyright Copyright (c) 2026 GESTELL
 */
#ifndef BUZZER_PRIVATE_H_
#define BUZZER_PRIVATE_H_

#include"../../../MCAL/DIO/DIO_Interface.h"

/**
 * @enum BuzzerState_t
 * @brief Discrete operational states of the acoustic alarm.
 */
typedef enum {
    BUZZER_STATE_OFF = 0U,   /**< Buzzer silenced (Pin LOW) */
    BUZZER_STATE_ON  = 1U    /**< Buzzer active (Pin HIGH or oscillating) */
} BuzzerState_t;

/**
 * @enum BuzzerMode_t
 * @brief Pattern mode of acoustic alarm.
 */
typedef enum {
    BUZZER_MODE_CONTINUOUS = 0U, /**< Solid continuous alarm tone */
    BUZZER_MODE_BEEP       = 1U  /**< Intermittent pulsed beep (e.g. 2 Hz) */
} BuzzerMode_t;

/**
 * @struct BuzzerControlBlock_t
 * @brief Runtime control descriptor for the buzzer actuator.
 */
typedef struct {
    DIO_GroupName_t group;          /**< Hardware port group (GroupD) */
    DIO_PinNumber_t pin;            /**< Hardware pin index (Pin7) */
    BuzzerState_t   state;          /**< Current state (OFF or ON) */
    BuzzerMode_t    mode;           /**< Operating mode (CONTINUOUS or BEEP) */
    uint16_t        beepPeriod_ms;  /**< Half-period for intermittent beeping */
    uint16_t        tickCounter_ms; /**< Millisecond accumulator for beep timing */
} BuzzerControlBlock_t;
#endif