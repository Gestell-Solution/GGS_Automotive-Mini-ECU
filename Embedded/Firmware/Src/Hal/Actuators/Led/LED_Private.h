/**
* 
* @file       LED_Private.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Rouaa Ayman )
* @brief      Private definitions for the LED driver
* @details    This header file contains private definitions and declarations for the LED driver
* @version    1.0.0
* @date       19/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#ifndef LED_PRIVATE_H
#define LED_PRIVATE_H

#include <stdint.h>

#include "../../../Mcal/Dio/DIO_Private.h"

/* =========================================================================
   TYPE DEFINITIONS & ENUMERATIONS
   ========================================================================= */

/**
 * @enum LedId_t
 * @brief Managed LED channel identifiers.
 */
typedef enum {
    LED_POWER_GREEN    = 0U,  /**< Green Heartbeat LED on GroupD Pin4 */
    LED_STATUS_BLUE    = 1U,  /**< Blue Engine/Status LED on GroupD Pin5 */
    LED_WARNING_YELLOW = 2U,  /**< Yellow Warning LED on GroupD Pin6 */
    LED_COUNT          = 3U   /**< Total number of LEDs */
} LedId_t;

/**
 * @enum LedState_t
 * @brief Physical illumination state.
 */
typedef enum {
    LED_STATE_OFF = 0U,       /**< LED unlit (Pin LOW) */
    LED_STATE_ON  = 1U        /**< LED illuminated (Pin HIGH) */
} LedState_t;

/**
 * @enum LedMode_t
 * @brief Operational mode of an LED channel.
 */
typedef enum {
    LED_MODE_STATIC   = 0U,   /**< Constant ON or OFF state */
    LED_MODE_BLINKING = 1U    /**< Non-blocking periodic flashing */
} LedMode_t;

/**
 * @struct LedControlBlock_t
 * @brief Runtime control descriptor per LED channel.
 */
typedef struct {
    DIO_GroupName_t group;          /**< Hardware port group (GroupD) */
    DIO_PinNumber_t pin;            /**< Hardware pin index (Pin4, Pin5, Pin6) */
    LedState_t      physicalState;  /**< Current output state */
    LedMode_t       mode;           /**< STATIC or BLINKING */
    uint16_t        halfPeriod_ms;  /**< Half-period duration in ms */
    uint16_t        tickCounter_ms; /**< Accumulated timer count in ms */
} LedControlBlock_t;


/* Compatibility macros */
#define LED_Init()                     hLED_Init()
#define LED_SetState(id, st)           hLED_SetState((id), (st))
#define LED_Set(id, st)                hLED_SetState((id), (st))
#define LED_Toggle(id)                 hLED_Toggle((id))
#define LED_Blink(id, freq)            hLED_Blink((id), (freq))
#define LED_StopBlink(id)              hLED_StopBlink((id))
#define LED_SetAll(g, b, y)            hLED_SetAll((g), (b), (y))
#define LED_GetState(id)               hLED_GetState((id))
#define LED_Update(step)               hLED_UpdatePeriodic((step))




#endif /* LED_PRIVATE_H */