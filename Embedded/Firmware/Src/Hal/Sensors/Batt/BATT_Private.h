/**
* 
* @file       BATT_Private.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Yassen Ahmed)
* @brief      Private definitions for the BATT driver
* @details    This header file defines the private implementation details of the BATT driver
* @version    1.0.0
* @date       22/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/


#ifndef BATT_PRIVATE_H_
#define BATT_PRIVATE_H_

#include "BATT_Config.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @struct BattSensor_ControlBlock_t
 * @brief Runtime control descriptor and sample buffer for battery monitoring.
 */
typedef struct {
    uint16_t lastRawADC;                          /**< Most recent 10-bit raw ADC reading */
    uint16_t lastVoltage_mV;                      /**< Most recent filtered voltage in millivolts */
    float    lastVoltage_V;                       /**< Most recent filtered voltage in Volts */
    uint16_t sampleBuffer[BATT_SAMPLE_WINDOW_SIZE]; /**< Rolling circular buffer */
    uint8_t  sampleIndex;                         /**< Circular buffer head index */
    bool     isInitialized;                       /**< Driver initialization status */
} BattSensor_ControlBlock_t;

/**
 * @brief Integer scaling factors for 10-bit ADC, 5.0V Vref, and 1:3 divider:
 *        V_batt (mV) = (raw * 15000) / 1024 = (raw * 1875) / 128
 */

#define BATT_SCALING_NUMERATOR      1875UL
#define BATT_SCALING_DENOMINATOR    128UL

#endif 