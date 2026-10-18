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
#define ISC60_Bit   4   /* INT6 sense control bit 0 */
#define ISC61_Bit   5   /* INT6 sense control bit 1 */
#define ISC70_Bit   6   /* INT7 sense control bit 0 */
#define ISC71_Bit   7   /* INT7 sense control bit 1 */

/* EIMSK Register Bits */
#define INT4_Bit    4   /* INT4 interrupt enable */
#define INT5_Bit    5   /* INT5 interrupt enable */
#define INT6_Bit    6   /* INT6 interrupt enable */
#define INT7_Bit    7   /* INT7 interrupt enable */

/* EIFR Register Bits */
#define INTF4_Bit   4   /* INT4 interrupt flag (write 1 to clear) */
#define INTF5_Bit   5   /* INT5 interrupt flag (write 1 to clear) */
#define INTF6_Bit   6   /* INT6 interrupt flag (write 1 to clear) */
#define INTF7_Bit   7   /* INT7 interrupt flag (write 1 to clear) */

/**
 * @brief Two-bit sense control field (ISCn1:ISCn0) before shifting.
 */
#define EICRB_SenseMask   0x03U   /* 0b 0000 0011 */

/**
 * @brief Number of EICRB lines handled by the driver (INT4..INT7).
 */
#define EXTI_LineCount    4U

/**
 * @brief Converts a line (INT4..INT7) to its callback array index (0..3).
 */
#define EXTI_Index(Line)  ((uint8_t)(Line) - (uint8_t)EXTI_Line_INT4)

/**
 * @enum EXTI_Line_t
 * @brief External interrupt lines controlled by EICRB (value = EIMSK / EIFR bit).
 */
typedef enum {
    EXTI_Line_INT4 = 4,  /* PE4: Encoder Feedback ChA / Cockpit Ignition */
    EXTI_Line_INT5 = 5,  /* PE5: Encoder Feedback ChB / Cockpit Fault Reset */
    EXTI_Line_INT6 = 6,  /* PE6: Cockpit Ignition Push-Button (Proteus Hardware) */
    EXTI_Line_INT7 = 7   /* PE7: External Interrupt 7 */
} EXTI_Line_t;

/**
 * @enum EXTI_SenseControl_t
 * @brief Interrupt sense control modes (EICRB bits ISC4x..ISC7x).
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

/* ============================================================
 * Interrupt Vectors (ATmega128 datasheet, Interrupt Vectors table)
 * ============================================================
 * The GCC-AVR linker names vector slot N as __vector_(N-1)
 * (datasheet counts the Reset vector as number 1).
 */

/** @brief INT4 vector: datasheet vector no. 6, address 0x000A */
#define INT4_vector           __vector_5

/** @brief INT5 vector: datasheet vector no. 7, address 0x000C */
#define INT5_vector           __vector_6

/** @brief INT6 vector: datasheet vector no. 8, address 0x000E */
#define INT6_vector           __vector_7

/** @brief INT7 vector: datasheet vector no. 9, address 0x0010 */
#define INT7_vector           __vector_8

/**
 * @brief Defines an Interrupt Service Routine for a vector symbol.
 * @param Vector Vector symbol (INT4_vector .. INT7_vector).
 */
#define EXTI_ISR(Vector) \
    void Vector(void) __attribute__((signal, used)); \
    void Vector(void)


#endif /* EXTI_PRIVATE_H */