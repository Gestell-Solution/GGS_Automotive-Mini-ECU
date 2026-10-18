/**
 * @file      ADC_Program.c
 * @brief     ADC driver core logic implementation.
 * @details   Contains the actual implementation of ADC APIs, channel configurations, and interrupt handlers.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Rouaa Ayman
 * @date      2026-10-12
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../Common/Config.h" 
#include "../../Common/MacroFunction.h"
#include "ADC_Interface.h"
#include "../Atmega128Register.h"

#include <stdint.h>
#include <util/delay.h>
#if ModuleAdcEnabled

/** 
 * @fn      mADC_Init
 * @brief   Implementation of mADC_Init.
 **/
ADC_Status_t     mADC_Init(void) {
    uint16_t counter = 0;

    if ((ADC_REFERENCE != ADC_Ref_AREF) &&
        (ADC_REFERENCE != ADC_Ref_AVCC) &&
        (ADC_REFERENCE != ADC_Ref_Internal))
    {
        return ADC_Status_Error;
    }

    if (((uint8_t)ADC_PRESCALER < 1) ||
        ((uint8_t)ADC_PRESCALER > 7))
    {
        return ADC_Status_Error;
    }

    if ((uint8_t)ADC_CHANNEL > 7)
    {
        return ADC_Status_Error;
    }

    SetBit(ADCSRA_Reg, ADEN_Bit);
    
    mADC_SetReference((uint8_t)ADC_REFERENCE);
    
    mADC_SetChannel((uint8_t)ADC_CHANNEL);


    ADCSRA_Reg &= ADCSRA_PrescalerMask;
    ADCSRA_Reg |= ADC_PRESCALER;
    _delay_us(5);
  
    mADC_StartConversion();    /* Start the dummy conversion. */

    
    uint8_t Conversion_state = 1;

    while ((counter < ADC_TIMEOUT_LIMIT) && (Conversion_state == 1))
    {
        Conversion_state = GetBit(ADCSRA_Reg, ADSC_Bit);
        counter++;
    }
    if (Conversion_state == 1)
    {
        return ADC_Status_Timeout; 
    }
    (void)mADC_GetResult();  //Review note
    return ADC_Status_Ok;
};

/**
 * @brief Configures the voltage reference source in ADMUX (REFS1:0).
 * @param ADC_REFERENCE Voltage reference selection (ADC_Ref_AVCC, etc.).
 */
void mADC_SetReference(uint8_t Refsource)
{
    if ((Refsource != ADC_Ref_AREF) &&
        (Refsource != ADC_Ref_AVCC) &&
        (Refsource != ADC_Ref_Internal))
    {
        return;
    }

    uint8_t Temp = ADMUX_Reg;

    Temp &= 0x3F;       /* 0b 0011 1111*/
    Temp |= Refsource;
    ADMUX_Reg = Temp;
}

/**
 * @brief Selects the analog input channel in ADMUX without corrupting reference bits.
 * @param ChannelNo Input channel index (ADC_Channel0 through ADC_Channel7).
 */
void     mADC_SetChannel(uint8_t ChannelNo) {
    if (ChannelNo >7)
    {
        return;
    }

    uint8_t Temp = ADMUX_Reg;
    Temp &= ADMUX_ChannelMask;
    Temp |= ChannelNo;
    ADMUX_Reg=Temp;
}

/**
 * @brief Starts an analog-to-digital conversion by setting the ADSC bit in ADCSRA.
 */
void     mADC_StartConversion(void)
{
    if ((GetBit(ADCSRA_Reg, ADEN_Bit)) == 0)
    {
        return;
    }

    if ((GetBit(ADCSRA_Reg, ADSC_Bit)) != 0)
    {
        return;
    }

    SetBit(ADCSRA_Reg, ADIF_Bit);
    SetBit(ADCSRA_Reg, ADSC_Bit);
}

/**
 * @brief Checks whether the active conversion has finished.
 * @return uint8_t 1 if complete (ADSC is 0), 0 if still converting.
 */
uint8_t  mADC_IsConversionComplete(void)
{
    uint8_t state = GetBit(ADCSRA_Reg, ADSC_Bit);
    if (state == 0)
    {
        return 1;      /* Completed */
    }
    else 
    {
        return 0;     /* Still Converting */
    }
}

/**
 * @brief Reads the 10-bit conversion result with hardware register locking (ADCL then ADCH).
 * @return uint16_t Quantized 10-bit integer (0 to 1023).
 */

uint16_t mADC_GetResult(void)
{
    uint8_t Low_byte = ADCL_Reg;
    uint8_t High_byte = ADCH_Reg;
    uint16_t Result = (High_byte << 8) | (uint16_t)Low_byte;
    return Result;
}

/**
 * @brief Performs a full synchronous conversion cycle (Channel select -> Convert -> Read).
 * @param ChannelNo Channel index to sample.
 * @return uint16_t Quantized 10-bit integer.
 */
uint16_t mADC_ReadChannel(uint8_t ChannelNo){
    mADC_SetChannel(ChannelNo);
    _delay_us(5);
    mADC_StartConversion();
    while (mADC_IsConversionComplete() == 0)
    {
        
    }
    return(mADC_GetResult());
}

/**
 * @brief Performs a bounded synchronous conversion with timeout guard against hardware lockup.
 * @param ChannelNo  Channel index to sample.
 * @param ResultPtr  Pointer to store the 10-bit result.
 * 
 * @return ADC_Status_Ok if the conversion succeeds,
 *         ADC_Status_Timeout if the polling limit is reached,
 *         ADC_Status_Busy if another conversion is running,
 *         ADC_Status_Error if an argument or ADC state is invalid.
 */
uint8_t  mADC_ReadChannelTimeout(uint8_t ChannelNo, uint16_t *ResultPtr){

    if (ChannelNo > 7)
    {
        return ADC_Status_Error;
    }

    if (ResultPtr == 0)
    {
        return ADC_Status_Error;
    }

    if ((GetBit(ADCSRA_Reg, ADEN_Bit)) == 0)
    {
        return ADC_Status_Error;
    }

    if (GetBit(ADCSRA_Reg, ADSC_Bit) == 1)
    {
        return ADC_Status_Busy;
    }

    mADC_SetChannel(ChannelNo);
    SetBit(ADCSRA_Reg, ADIF_Bit);
    _delay_us(5);                 /* sample and hold settling time */

    mADC_StartConversion();
    uint32_t TimeoutCounter = ADC_TIMEOUT_LIMIT;

    while (GetBit(ADCSRA_Reg, ADSC_Bit) == 1)
    {
        TimeoutCounter--;

        if (TimeoutCounter == 0)
        {
            return ADC_Status_Timeout;
        }
        
    }
    *ResultPtr = mADC_GetResult();
    return ADC_Status_Ok;
}

/**
 * @brief Powers up the ADC circuitry (sets ADEN in ADCSRA).
 */
void     mADC_Enable(void)
{
    SetBit(ADCSRA_Reg, ADEN_Bit);
}

/**
 * @brief Powers down the ADC circuitry to conserve power (clears ADEN in ADCSRA).
 */
void     mADC_Disable(void)
{
    ClearBit(ADCSRA_Reg, ADEN_Bit);
}


#endif