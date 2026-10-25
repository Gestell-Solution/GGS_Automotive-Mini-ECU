/**
 * @file      ADC_Interface.h
 * @brief     ADC driver interface APIs and data types.
 * @details   Provides public initialization, configuration, and conversion APIs for the ADC module.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Rouaa Ayman
 * @date      2026-10-12
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _ADC_INTERFACE_H_
 #define _ADC_INTERFACE_H_

 #include "ADC_Config.h"

 #include <stdint.h>

/**
 * @brief Initializes the ADC peripheral with default AVCC reference and ÷128 prescaler.
 *        Executes a dummy conversion to prime the internal analog circuitry.
 * @return    ADC_Status_t Returns ADC_Status_Ok on success, or ADC_Status_Error / ADC_Status_Timeout on failure.
 */
void     mADC_Init(void);

/**
 * @brief Configures the voltage reference source in ADMUX (REFS1:0).
 * @details Validates the reference source and updates ADMUX register bits 7 and 6.
 * @param RefSource Voltage reference selection (ADC_Ref_AVCC, ADC_Ref_AREF, or ADC_Ref_Internal).
 */
void     mADC_SetReference(uint8_t RefSource);

/**
 * @brief Selects the analog input channel in ADMUX without corrupting reference bits.
 * @details Masks the MUX bits and sets the requested channel index.
 * @param ChannelNo Input channel index (ADC_Channel0 through ADC_Channel7).
 * @note - Invalid channel values are ignored.
 *       - Call this function only when no conversion is in progress.
 *       - after calling that function, you must allow ~5 µs for the 
 *         Sample & Hold capacitor to charge to the new channel voltage 
 *         before starting a conversion.
 */
void     mADC_SetChannel(uint8_t ChannelNo);

/**
 * @brief Starts an analog-to-digital conversion by setting the ADSC bit in ADCSRA.
 * @details   Verifies that the ADC is enabled and not currently busy before triggering.
 * @note - Use mADC_ReadChannelTimeout() when error reporting is required.
 *       - The ADC must be enabled and no conversion must be active.
 */
void     mADC_StartConversion(void);

/**
 * @brief Checks whether the active conversion has finished.
 * @details   Reads the ADSC bit status from the ADCSRA register.
 * @return uint8_t 1 if complete (ADSC is 0), 
 *                 0 if still converting.
 * @note Use this function before reading register data; do not read data 
 *       if the conversion is not completed.
 */
uint8_t  mADC_IsConversionComplete(void);

/**
 * @brief Reads the 10-bit conversion result with hardware register locking (ADCL then ADCH).
 * @details Pseudo-code:
 *          - Read ADCL (triggers hardware unlock)
 *          - Read ADCH
 *          - Assemble 10-bit result and return it
 * @return uint16_t Quantized 10-bit integer (0 to 1023).
 * @warning Call only after conversion completion.
 */
uint16_t mADC_GetResult(void);

/**
 * @brief Performs a full synchronous conversion cycle (Channel select -> Convert -> Read).
 * @details   Wraps channel selection, 5us settling delay, conversion start, polling, and result retrieval.
 * @param ChannelNo Channel index to sample.
 * @return uint16_t Quantized 10-bit integer.
 * @return Zero if the operation fails.
 * @note Prefer mADC_ReadChannelTimeout() when error reporting is needed, or timeout protection.
 */
uint16_t mADC_ReadChannel(uint8_t ChannelNo);

/**
 * @brief Performs a bounded synchronous conversion with timeout guard against hardware lockup.
 * @details   Validates arguments and ADC status, selects channel, waits for completion using a decrementing counter.
 * @param ChannelNo  Channel index to sample.
 * @param ResultPtr  Pointer to the destination for the conversion result.
 * @return uint8_t   ADC_Status_Ok on success, ADC_Status_Timeout on failure,
 *                   ADC_Status_Busy if another conversion is active,
 *                   ADC_Status_Error for invalid arguments or disabled ADC.
 * @note - ADC_TIMEOUT_LIMIT is an iteration limit, not a guaranteed timeout duration in 
 *         milliseconds or microseconds
 *       - ResultPtr is updated only when the operation succeeds.
 */
uint8_t  mADC_ReadChannelTimeout(uint8_t ChannelNo, uint16_t *ResultPtr);

/**
 * @brief Powers up the ADC circuitry (sets ADEN in ADCSRA).
 */
void     mADC_Enable(void);

/**
 * @brief Powers down the ADC circuitry to conserve power (clears ADEN in ADCSRA).
 */
void     mADC_Disable(void);


 #endif