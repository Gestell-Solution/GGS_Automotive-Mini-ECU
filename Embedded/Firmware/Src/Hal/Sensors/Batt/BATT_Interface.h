/**
* 
* @file       BATT_Interface.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Yassen Ahmed)
* @brief      Interface file for the BATT driver
* @details    This header file defines the public API of the BATT driver
* @version    1.0.0
* @date       22/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/


#ifndef BATT_INTERFACE_H_
#define BATT_INTERFACE_H_

/* Include necessary headers */
#include <stdint.h>
#include <stdbool.h>

/* common libararys */
#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"
#include "../../../Common/MacroFunction.h"

/* Driver-specific headers */
#include "BATT_Private.h"
#include "BATT_Config.h" 

/* calling lower level layers APIs*/
#include "../../../MCAL/ADC/ADC_Interface.h"


/* =========================================================================
   PUBLIC API FUNCTION PROTOTYPES
   ========================================================================= */

/**
 * @fn void hBATT_Init(void)
 * @brief Initializes the battery sensor driver, resets the filter, and takes baseline sample.
 */
void     hBATT_Init(void);

/**
 * @fn uint16_t hBATT_ReadVoltage_mV(void)
 * @brief Samples ADC Channel 1, applies moving average filtering, and returns voltage in millivolts.
 *        Avoids floating point operations (e.g. 12400 represents 12.40 V).
 * @return uint16_t Battery voltage in millivolts (0 to 16000 mV).
 */
uint16_t hBATT_ReadVoltage_mV(void);

/**
 * @fn float hBATT_ReadVoltage_V(void)
 * @brief Samples ADC Channel 1, applies filtering, and returns battery voltage in Volts as float.
 * @return float Battery voltage in Volts (e.g. 12.4f).
 */
float    hBATT_ReadVoltage_V(void);

/**
 * @fn uint8_t hBATT_ReadVoltage_x10(void)
 * @brief Returns battery voltage scaled by 10 (e.g. 124 represents 12.4 V).
 *        Convenient for formatted displays and telemetry transmission.
 * @return uint8_t Scaled battery voltage tenths (0 to 160).
 */
uint8_t  hBATT_ReadVoltage_x10(void);

/**
 * @fn uint16_t hBATT_GetLastVoltage_mV(void)
 * @brief Returns the last cached battery voltage in millivolts without triggering a new ADC read.
 * @return uint16_t Cached voltage in millivolts.
 */
uint16_t hBATT_GetLastVoltage_mV(void);

/**
 * @brief Returns the last cached battery voltage in Volts without triggering a new ADC read.
 * @return float Cached voltage in Volts.
 */
float    hBATT_GetLastVoltage_V(void);

/**
 * @fn uint16_t hBATT_GetLastRawADC(void)
 * @brief Returns the most recent 10-bit raw ADC reading on Channel 1 (0 to 1023).
 * @return uint16_t Raw ADC count.
 */
uint16_t hBATT_GetLastRawADC(void);

/**
 * @fn bool hBATT_IsFault(void)
 * @brief Evaluates whether the battery voltage is in a fault condition (< 10.5 V or > 16.0 V).
 * @return bool true if voltage is out of safe range, false if nominal.
 */
bool     hBATT_IsFault(void);

/* Compatibility aliases */
#define BattSensor_Init()              hBATT_Init()
#define BattSensor_GetVoltage_mV()     hBATT_ReadVoltage_mV()
#define BattSensor_GetVoltage_V()      hBATT_ReadVoltage_V()
#define BattSensor_GetVoltage_x10()    hBATT_ReadVoltage_x10()
#define BattSensor_GetLastVoltage_mV() hBATT_GetLastVoltage_mV()
#define BattSensor_GetLastVoltage_V()  hBATT_GetLastVoltage_V()
#define BattSensor_IsFault()           hBATT_IsFault()


#endif 