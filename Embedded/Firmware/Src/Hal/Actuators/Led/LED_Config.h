/**
* 
* @file       LED_Config.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Rouaa Ayman )
* @brief      Configuration file for the LED driver
* @details    This header file defines the compile-time configuration of the LED driver
* @version    1.0.0
* @date       19/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#ifndef LED_CONFIG_H
#define LED_CONFIG_H

#include "LED_Interface.h"

/*
typedef enum {
    LED_POWER_GREEN    = 0U,  /**< Green Heartbeat LED on GroupD Pin4 */
    LED_STATUS_BLUE    = 1U,  /**< Blue Engine/Status LED on GroupD Pin5 */
    LED_WARNING_YELLOW = 2U,  /**< Yellow Warning LED on GroupD Pin6 */
    LED_COUNT          = 3U   /**< Total number of LEDs */
} LedId_t;
*/

#define LED_POWER_GREEN    Pin4

#endif /* LED_CONFIG_H */