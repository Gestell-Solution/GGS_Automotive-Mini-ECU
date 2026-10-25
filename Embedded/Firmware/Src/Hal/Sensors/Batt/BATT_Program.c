/**
* 
* @file       BATT_Program.c
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Yassen Ahmed)
* @brief      Implementation file for the BATT driver
* @details    This source file contains the implementation of the BATT driver functions
* @version    1.0.0
* @date       22/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/


#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"

#if (ModuleBattEnabled == Enable )

#include "BATT_Interface.h"

static BattSensor_ControlBlock_t g_battControl;

/**
 * @fn void hBATT_Init(void)
 * @brief Initializes the battery sensor driver, resets the filter, and takes baseline sample.
 */
void     hBATT_Init(void)
{
    uint8_t count_samp = 0 ;
    uint16_t raw = 0;
    uint16_t init_mV = 0 ;
    /* we call the init for adc*/
    mADC_Init();

    /* we just read the values raw */
    /* cause the Vadc = Vbatt * (R2/R1+R2) */
    /* so its Vbatt * 1/3 */
    /* or Vbatt = 3 * Vadc*/
    raw = mADC_ReadChannel((uint8_t)BATT_ADC_CHANNEL);
    init_mV = (uint16_t)(((uint32_t)raw * BATT_SCALING_NUMERATOR) / BATT_SCALING_DENOMINATOR);

    /* we store the last result of raw */

    g_battControl.lastRawADC = raw ;

    /* we fill each slot */
    for (count_samp = 0 ; count_samp < BATT_SAMPLE_WINDOW_SIZE; count_samp++)
    {
        g_battControl.sampleBuffer[count_samp] = init_mV;
    }

    /* we init the valus of each one*/
    g_battControl.sampleIndex = 0U ;
    g_battControl.lastVoltage_mV = init_mV;
    g_battControl.lastVoltage_V = (float)init_mV / 1000.0f ;
    g_battControl.isInitialized = True;

}

/**
 * @fn uint16_t hBATT_ReadVoltage_mV(void)
 * @brief Samples ADC Channel 1, applies moving average filtering, and returns voltage in millivolts.
 *        Avoids floating point operations (e.g. 12400 represents 12.40 V).
 * @return uint16_t Battery voltage in millivolts (0 to 16000 mV).
 */
uint16_t hBATT_ReadVoltage_mV(void)
{
    uint8_t count_samp = 0 ;
    uint16_t raw = 0;
    uint16_t sample_mV = 0 ;
    uint32_t sum = 0UL;
    uint16_t avg_mV = 0 ;
    /* we read the raw value first */
    raw = mADC_ReadChannel((uint8_t)BATT_ADC_CHANNEL);
  
    /* we store the last raw value */
    g_battControl.lastRawADC = raw ;
    
    /* we get 1 sample of that raw value */
    sample_mV = (uint16_t)(((uint32_t)raw * BATT_SCALING_NUMERATOR) / BATT_SCALING_DENOMINATOR);
    
    /* we store the sample buffer of the first index (0) we store in it the sample of mv*/
    g_battControl.sampleBuffer[g_battControl.sampleIndex] = sample_mV;

    /* we move the sample by 1*/
    g_battControl.sampleIndex = (g_battControl.sampleIndex + 1U) % BATT_SAMPLE_WINDOW_SIZE;

    for (count_samp = 0 ; count_samp < BATT_SAMPLE_WINDOW_SIZE; count_samp++)
    {
        sum += g_battControl.sampleBuffer[count_samp];
    }
    /* we devide by 4*/
    avg_mV = (uint16_t)(sum / BATT_SAMPLE_WINDOW_SIZE);

    /* we store the filterd results */
    g_battControl.lastVoltage_mV = avg_mV;
    g_battControl.lastVoltage_V = (float)avg_mV / 1000.0f;

    return avg_mV;
}

/**
 * @fn float hBATT_ReadVoltage_V(void)
 * @brief Samples ADC Channel 1, applies filtering, and returns battery voltage in Volts as float.
 * @return float Battery voltage in Volts (e.g. 12.4f).
 */
float    hBATT_ReadVoltage_V(void)
{
    /* we dont even need to store the value just take it form the g_var*/
    hBATT_ReadVoltage_mV();
    return g_battControl.lastVoltage_V;
}

/**
 * @fn uint8_t hBATT_ReadVoltage_x10(void)
 * @brief Returns battery voltage scaled by 10 (e.g. 124 represents 12.4 V).
 *        Convenient for formatted displays and telemetry transmission.
 * @return uint8_t Scaled battery voltage tenths (0 to 160).
 */
uint8_t  hBATT_ReadVoltage_x10(void)
{
    uint16_t val_mV = hBATT_ReadVoltage_mV();
    return (uint8_t)(val_mV / 100U);
}

/**
 * @brief Returns the last cached battery voltage in millivolts without triggering a new ADC read.
 * @return uint16_t Cached voltage in millivolts.
 */
uint16_t hBATT_GetLastVoltage_mV(void)
{
    return g_battControl.lastVoltage_mV;
}

/**
 * @brief Returns the last cached battery voltage in Volts without triggering a new ADC read.
 * @return float Cached voltage in Volts.
 */
float hBATT_GetLastVoltage_V(void)
{
    return g_battControl.lastVoltage_V;
}

/**
 * @brief Returns the most recent 10-bit raw ADC reading on Channel 1 (0 to 1023).
 * @return uint16_t Raw ADC count.
 */
uint16_t hBATT_GetLastRawADC(void)
{
    return g_battControl.lastRawADC;
}

/**
 * @brief Evaluates whether the battery voltage is in a fault condition (< 10.5 V or > 16.0 V).
 * @return bool true if voltage is out of safe range, false if nominal.
 */
bool hBATT_IsFault(void)
{
    if (g_battControl.lastVoltage_mV < BATT_VOLTAGE_MIN_MV)
    {
        return True;
    }
    if (g_battControl.lastVoltage_mV > BATT_VOLTAGE_MAX_MV)
    {
        return True;
    }
    return False;
}


#endif