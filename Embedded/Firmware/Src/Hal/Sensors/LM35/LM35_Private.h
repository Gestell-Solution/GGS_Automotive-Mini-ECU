/**
 * @file      LM35_Private.h
 * @brief     Private header for the LM35 temperature sensor driver.
 * @details   Contains private definitions, control blocks, macros, and internal structures used by the LM35 implementation.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Salma Mohamed
 * @date      2026-10-19
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _LM35_PRIVATE_H_
 #define _LM35_PRIVATE_H_

#include <stdint.h>
#include "LM35_Config.h"
 /* =========================================================================
   CONSTANTS & DEFINITIONS
   ========================================================================= */

/**
 * @struct LM35_ControlBlock_t
 * @brief Runtime control and sample history for the LM35 driver.
 */

typedef struct {
    uint16_t lastRawADC;                     /**< Most recent 10-bit raw ADC reading */
    int16_t  lastTemp_x10;                   /**< Most recent calculated temp x10 (e.g. 425) */
    float    lastTemp_C;                     /**< Most recent temperature in float */
    int16_t  sampleBuffer[LM35_SAMPLE_WINDOW_SIZE];  /**< Rolling circular history */
    uint8_t  sampleIndex;                    /**< Circular buffer head index */
    bool     isInitialized;                  /**< Driver initialization status flag */
} LM35_ControlBlock_t;

 #endif