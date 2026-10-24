/**
 * @file      LM35_Program.c
 * @brief     Source implementation for the LM35 temperature sensor driver.
 * @details   Contains the actual function implementations for sensor initialization, multi-sample rolling average filtering, and temperature acquisition APIs.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Salma Mohamed
 * @date      2026-10-19
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../../Common/Config.h"
#include "../../../Common/Definition.h"
#include "LM35_Interface.h"
#include "LM35_Private.h"
#include "../../../Mcal/ADC/ADC_Interface.h"
#include <stdint.h>
#include <stdbool.h>

#if ModuleLm35Enabled


static LM35_ControlBlock_t g_lm35Control;

void hLM35_Init(void) {
    mADC_Init();
    uint16_t raw = mADC_ReadChannel(LM35_ADC_CHANNEL);
    g_lm35Control.lastRawADC = raw;
    int16_t temp10 = (int16_t)(((uint32_t)raw * 625UL) / 128UL);
    uint8_t i=0;
    for(i=0; i<LM35_SAMPLE_WINDOW_SIZE; i++)
    {
        g_lm35Control.sampleBuffer[i] = temp10;
    }  
    g_lm35Control.sampleIndex = 0U;
    g_lm35Control.lastTemp_x10 = temp10;
    g_lm35Control.lastTemp_C = (float)temp10 / 10.0f;
    g_lm35Control.isInitialized = True;
    return;
}

int16_t hLM35_ReadTemp_x10(void){
    uint16_t raw = mADC_ReadChannel(LM35_ADC_CHANNEL);
    g_lm35Control.lastRawADC = raw;
    int16_t temp10 = (int16_t)(((uint32_t)raw * 625UL) / 128UL);

    g_lm35Control.sampleBuffer[g_lm35Control.sampleIndex] = temp10;
    g_lm35Control.sampleIndex = (g_lm35Control.sampleIndex + 1U) % LM35_SAMPLE_WINDOW_SIZE;

    int16_t sum =0;
    uint8_t i =0;
    for  (i=0; i < LM35_SAMPLE_WINDOW_SIZE; i++)
    {
        sum += g_lm35Control.sampleBuffer[i];
    }
    int16_t avg10 = sum / LM35_SAMPLE_WINDOW_SIZE;

    g_lm35Control.lastTemp_x10 = avg10;
    g_lm35Control.lastTemp_C=(float)avg10/10.0f;
    return avg10;
}

float    hLM35_ReadTemp_C(void){
    g_lm35Control.lastTemp_C = (hLM35_ReadTemp_x10())/10.0f;
    return g_lm35Control.lastTemp_C;
}

int8_t   hLM35_ReadTemp_Int(void){
    int16_t Val10 = hLM35_ReadTemp_x10();
    return (int8_t)(Val10 / 10);
}

float    hLM35_GetLastTemp_C(void){
    return (float)g_lm35Control.lastTemp_C;
}

int16_t  hLM35_GetLastTemp_x10(void){
    return (int16_t) g_lm35Control.lastTemp_x10;
}

uint16_t hLM35_GetLastRawADC(void){
    return g_lm35Control.lastRawADC;
}

bool     hLM35_IsOverTemp(void){
    int16_t Last_temp = g_lm35Control.lastTemp_x10;
    if (Last_temp>LM35_OVERTEMP_LIMIT_X10)
    {
        return True;
    }
    else
    {
        return False;
    }
}

#endif