/**
 * @file      LM35_Interface.h
 * @brief     Public interface header for the LM35 temperature sensor driver.
 * @details   Contains public function APIs, configuration macros, and user-facing wrappers for reading temperature values.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Salma Mohamed
 * @date      2026-10-19
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef _LM35_INTERFACE_H_
#define _LM35_INTERFACE_H_

#include <stdint.h>
#include <stdbool.h>

/* =========================================================================
   PUBLIC API FUNCTION PROTOTYPES
   ========================================================================= */

/**
 * @fn      hLM35_Init
 * @brief   Initializes the LM35 sensor driver and resets sample history.
 * @details Configures internal control blocks, clears buffer history, and synchronizes initial ADC readings. 
 *          Requires mADC_Init() to be called prior to or within this function.
 */
void     hLM35_Init(void);

/**
 * @fn      hLM35_ReadTemp_C
 * @brief   Triggers an ADC conversion and returns temperature in Celsius.
 * @details Triggers an ADC conversion on Channel 0, applies the rolling average filter, and returns the filtered temperature as a float.
 * @return  float Filtered temperature in degrees Celsius (e.g. 42.5f).
 */
float    hLM35_ReadTemp_C(void);

/**
 * @fn      hLM35_ReadTemp_x10
 * @brief   Triggers an ADC conversion and returns integer temperature scaled x10.
 * @details Triggers an ADC conversion, applies filtering, and avoids floating point operations by scaling the result by 10 (e.g. 425 represents 42.5 °C).
 * @return  int16_t Filtered temperature scaled by 10.
 */
int16_t  hLM35_ReadTemp_x10(void);

/**
 * @fn      hLM35_ReadTemp_Int
 * @brief   Returns the integer part of the temperature.
 * @details Performs a filtered temperature read and truncates the decimal value for formatted display purposes.
 * @return  int8_t Truncated integer temperature (e.g. 42 for 42.5 °C).
 */
int8_t   hLM35_ReadTemp_Int(void);

/**
 * @fn      hLM35_GetLastTemp_C
 * @brief   Returns the last cached temperature in float.
 * @details Retrieves the most recently calculated temperature in degrees Celsius without triggering a new ADC read.
 * @return  float Last valid temperature in degrees Celsius.
 */
float    hLM35_GetLastTemp_C(void);

/**
 * @fn      hLM35_GetLastTemp_x10
 * @brief   Returns the last cached temperature scaled x10.
 * @details Retrieves the most recently calculated temperature scaled by 10 without triggering a new ADC read.
 * @return  int16_t Last valid temperature scaled x10.
 */
int16_t  hLM35_GetLastTemp_x10(void);

/**
 * @fn      hLM35_GetLastRawADC
 * @brief   Returns the most recent raw 10-bit ADC reading.
 * @details Fetches the latest quantized ADC count stored in the control block (0 to 1023).
 * @return  uint16_t Raw quantized ADC count.
 */
uint16_t hLM35_GetLastRawADC(void);

/**
 * @fn      hLM35_IsOverTemp
 * @brief   Checks if the current temperature exceeds the critical Over-Temperature limit.
 * @details Compares the latest cached temperature against the over-temperature threshold (90.0 °C).
 * @retval  true Temperature exceeds 90.0 °C.
 * @retval  false Temperature is within safe limits.
 */
bool     hLM35_IsOverTemp(void);

/* Compatibility macros */
#define LM35_Init()             hLM35_Init()
#define LM35_ReadTemp()         hLM35_ReadTemp_C()
#define LM35_GetTemp_x10()      hLM35_ReadTemp_x10()
#define LM35_GetLastTemp()      hLM35_GetLastTemp_C()
#define LM35_GetLastTemp_x10()  hLM35_GetLastTemp_x10()
#define LM35_IsOverTemp()       hLM35_IsOverTemp()

#endif