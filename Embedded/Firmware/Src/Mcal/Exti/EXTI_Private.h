/**
 * @file
 *     EXTI_Private.h
 * @brief
 *     EXTI driver private definitions.
 * @details
 *    Contains register bit positions, masks, enums and the callback type of the EXTI driver.
 * @version
 *    1.0.0
 * @author
 *    Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *    Reviewer:  Beshoy Esmat
 * @date
 *    2026-10-17
 * @copyright
 *    Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef EXTI_PRIVATE_H
#define EXTI_PRIVATE_H

#include <stdint.h>

/* EICRB Register Bits */
#define ISC40_Bit   0   /* INT4 sense control bit 0 */
#define ISC41_Bit   1   /* INT4 sense control bit 1 */
#define ISC50_Bit   2   /* INT5 sense control bit 0 */
#define ISC51_Bit   3   /* INT5 sense control bit 1 */

/* EIMSK Register Bits */
#define INT4_Bit    4   /* INT4 interrupt enable */
#define INT5_Bit    5   /* INT5 interrupt enable */

/* EIFR Register Bits */
#define INTF4_Bit   4   /* INT4 interrupt flag (write 1 to clear) */
#define INTF5_Bit   5   /* INT5 interrupt flag (write 1 to clear) */

/**
 * @brief Two-bit sense control field (ISCn1:ISCn0) before shifting.
 */
#define EICRB_SenseMask   0x03U   /* 0b 0000 0011 */

/**
 * @brief Number of lines handled by the driver (INT4, INT5).
 */
#define EXTI_LineCount    2U

/**
 * @brief Converts a line (INT4 / INT5) to its callback array index (0 / 1).
 */
#define EXTI_Index(Line)  ((uint8_t)(Line) - (uint8_t)EXTI_Line_INT4)

/**
 * @enum EXTI_Line_t
 * @brief External interrupt lines used by the ECU (value = EIMSK / EIFR bit).
 */
typedef enum {
    EXTI_Line_INT4 = 4,  /* PE4: Cockpit Ignition Push-Button */
    EXTI_Line_INT5 = 5   /* PE5: Cockpit Fault Reset Push-Button */
} EXTI_Line_t;

/**
 * @enum EXTI_SenseControl_t
 * @brief Interrupt sense control modes (EICRB bits ISC4x / ISC5x).
 */
typedef enum {
    EXTI_Sense_LowLevel    = 0x00,  /* Low level generates an interrupt */
                                    /* 0x01 is Reserved */
    EXTI_Sense_FallingEdge = 0x02,  /* Falling edge generates an interrupt (Default) */
    EXTI_Sense_RisingEdge  = 0x03   /* Rising edge generates an interrupt */
} EXTI_SenseControl_t;

/**
 * @enum EXTI_Status_t
 * @brief EXTI operation return status codes.
 */
typedef enum {
    EXTI_Status_Ok    = 0,  /* Operation completed successfully */
    EXTI_Status_Error = 1   /* Invalid argument */
} EXTI_Status_t;

/**
 * @typedef EXTI_Callback_t
 * @brief Function pointer prototype for ISR callbacks.
 */
typedef void (*EXTI_Callback_t)(void);


#endif /* EXTI_PRIVATE_H */