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


/** @brief Green Heartbeat / Power LED (Physical PD4) */
#define LED_GREEN_PORT      GroupD
#define LED_GREEN_PIN       Pin4

/** @brief Blue Engine / Telemetry Status LED (Physical PD5) */
#define LED_BLUE_PORT       GroupD
#define LED_BLUE_PIN        Pin5

/** @brief Yellow Warning / Diagnostic Fault LED (Physical PD6) */
#define LED_YELLOW_PORT     GroupD
#define LED_YELLOW_PIN      Pin6

#endif /* LED_CONFIG_H */