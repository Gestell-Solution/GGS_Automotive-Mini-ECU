/**
 * @file      ADC_Private.h
 * @brief     ADC driver private registers and internal definitions.
 * @details   Contains hardware memory mapping, register definitions, and internal helper macros for ADC.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Rouaa Ayman
 * @date      2026-10-12
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _ADC_PRIVATE_H_
 #define _ADC_PRIVATE_H_

/*ADC Registers' Bits*/

/* ADMUX Register Bits*/
#define MUX0_Bit    0   /*  Input Channel and Gain Selections bit 0 */
#define MUX1_Bit    1   /*  Input Channel and Gain Selections bit 1 */
#define MUX2_Bit    2   /*  Input Channel and Gain Selections bit 2 */
#define MUX3_Bit    3   /*  Input Channel and Gain Selections bit 3 */
#define MUX4_Bit    4   /*  Input Channel and Gain Selections bit 4 */
#define ADLAR_Bit   5   /* Left adjustment register*/
#define REFS0_Bit   6   /* REference voltage selection bit 0*/
#define REFS1_Bit   7   /* REference voltage selection bit 1*/

/* ADCSRA Register Bits*/
#define ADPS0_Bit   0   /* ADC clock prescaler selection bit 0 */
#define ADPS1_Bit   1   /* ADC clock prescaler selection bit 1 */
#define ADPS2_Bit   2   /* ADC clock prescaler selection bit 2 */
#define ADIE_Bit    3   /* Interrupt enable */
#define ADIF_Bit    4   /* interrupt flag */
#define ADFR_Bit    5   /* Free running mode */
#define ADSC_Bit    6   /* ADC Start Conversion */
#define ADEN_Bit    7   /* ADC enable*/


/**
 * @enum ADC_Channel_t
 * @brief ATmega128 ADC multiplexer input channels (GroupF / PORTF pins).
 */
typedef enum {
    ADC_Channel0 = 0,  /* PF0: LM35 Engine Temperature Sensor */
    ADC_Channel1 = 1,  /* PF1: Battery Voltage Divider (3:1) */
    ADC_Channel2 = 2,  /* PF2: Spare Analog Channel */
    ADC_Channel3 = 3,  /* PF3: Spare Analog Channel */
    ADC_Channel4 = 4,  /* PF4: Spare Analog Channel */
    ADC_Channel5 = 5,  /* PF5: Spare Analog Channel */
    ADC_Channel6 = 6,  /* PF6: Spare Analog Channel */
    ADC_Channel7 = 7   /* PF7: Spare Analog Channel */
} ADC_Channel_t;

/**
 * @brief Preserves ADMUX register bits while clearing MUX2:0.
 * @note  ADMUX & 0xF8 preserves bits 7, 6, 5, 4, and 3.
 */
#define ADMUX_ChannelMask 0xF8    /* 0b 1111 1000 */

/**
 * @enum ADC_Reference_t
 * @brief Voltage reference selection bits (ADMUX bits REFS1:0).
 */
typedef enum {
    ADC_Ref_AREF     = 0x00,  /* External AREF Pin */
    ADC_Ref_AVCC     = 0x40,  /* AVCC (5.0V) with external capacitor at AREF */
    ADC_Ref_Internal = 0xC0   /* Internal 2.56V Voltage Reference */
} ADC_Reference_t;


/**
 * @enum ADC_Prescaler_t
 * @brief Clock prescaler division factors (ADCSRA bits ADPS2:0).
 * @note  The ADC clock is derived from the CPU clock.
 *        For F_CPU = 16 MHz and division by 128:
 *        F_ADC = 16 MHz / 128 = 125 kHz.
 */
typedef enum {
    ADC_Prescaler_2   = 1,
    ADC_Prescaler_4   = 2,
    ADC_Prescaler_8   = 3,
    ADC_Prescaler_16  = 4,
    ADC_Prescaler_32  = 5,
    ADC_Prescaler_64  = 6,
    ADC_Prescaler_128 = 7  
} ADC_Prescaler_t;

/**
 * @brief Preserves the non-prescaler fields in ADCSRA.
 * @note  ADCSRA & 0xF8 clears ADPS2:0.
 */
#define ADCSRA_PrescalerMask 0xF8    /* 0b11111000*/

/**
 * @enum ADC_Status_t
 * @brief ADC operation return status codes.
 */
typedef enum {
    ADC_Status_Ok      = 0,  /* Operation completed successfully */
    ADC_Status_Timeout = 1,  /* Conversion exceeded polling limit */
    ADC_Status_Busy    = 2,  /* Convrsion is in progress */
    ADC_Status_Error   = 3   /* Invalid argument or configuration */
} ADC_Status_t;


 #endif