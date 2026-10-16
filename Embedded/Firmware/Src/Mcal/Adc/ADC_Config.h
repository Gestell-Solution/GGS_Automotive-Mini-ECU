/**
 * @file      ADC_Config.h
 * @brief     ADC driver pre-compile configuration parameters.
 * @details   Contains user-configurable options like clock prescalers, resolution, and conversion modes.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Rouaa Ayman
 * @date      2026-10-12
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _ADC_CONFIG_H_
 #define _ADC_CONFIG_H_

#include "Embedded\Firmware\Src\Mcal\ADC\ADC_Private.h"

/**
 * @def ADC_REFERENCE
 * @brief Default ADC voltage reference.
 *
 * @details Available values:
 *          ADC_Ref_AREF
 *          ADC_Ref_AVCC
 *          ADC_Ref_Internal
 */
#define ADC_REFERENCE  ADC_Ref_AVCC

/**
 * @def ADC_PRESCALER
 * @brief ADC clock prescaler.
 *
 * @details ADC_Prescaler_128 produces a 125 kHz ADC clock
 *          when F_CPU = 16 MHz.
 */
#define ADC_PRESCALER  ADC_Prescaler_128

/**
 * @def ADC_CHANNEL
 * @brief Default ADC input channel selected during initialization.
 */
#define ADC_CHANNEL    ADC_Channel0  

/**
 * @def ADC_TIMEOUT_LIMIT
 * @brief Maximum polling-loop iterations for a bounded ADC operation.
 *
 * @warning This is an iteration count, not a time value.
 *          The actual elapsed time depends on CPU frequency,
 *          compiler optimization, and generated machine instructions.
 */
#define ADC_TIMEOUT_LIMIT 10000UL


#endif