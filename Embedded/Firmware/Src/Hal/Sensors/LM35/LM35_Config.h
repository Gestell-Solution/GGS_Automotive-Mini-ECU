/**
 * @file      LM35_Config.h
 * @brief     Configuration header for the LM35 temperature sensor driver.
 * @details   Contains user-configurable options, channel selections, and threshold limits related to the LM35 sensor.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Salma Mohamed
 * @date      2026-10-19
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _LM35_CONFIG_H_
 #define _LM35_CONFIG_H_
 #include "../../../Mcal/Adc/ADC_Private.h"

/**
 * @def   LM35_ADC_CHANNEL
 * @brief Defines the ADC channel connected to the LM35 sensor (Connected to PF0/ADC0).
 */
#define LM35_ADC_CHANNEL            ADC_Channel0

/**
 * @def   LM35_SAMPLE_WINDOW_SIZE
 * @brief Defines the window size for the rolling average temperature filter.
 */
#define LM35_SAMPLE_WINDOW_SIZE     4U

/**
 * @def   LM35_OVERTEMP_LIMIT_X10
 * @brief Defines the critical over-temperature limit (90.0 °C in scaled x10 format).
 */
#define LM35_OVERTEMP_LIMIT_X10     900

 #endif