#ifndef _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_PRIVATE_H
#define _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_PRIVATE_H

/**
 * @file
 *      EXTI_Private.h
 * @brief
 *      Private definitions of the External Interrupt (EXTI) driver.
 * @details
 *      Register bit positions (from the ATmega128 datasheet), line limits,
 *      callback indexes, sense masks and validation macros. This header is
 *      for use by EXTI_Program.c only and must not be included by upper
 *      layers. It expects EXTI_Interface.h to be included in the same file.
 * @version
 *      1.0.0
 * @author
 *      Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *      2026-10-13
 * @copyright
 *      Copyright (c) 2026 Gestell. All rights reserved.
 */

/**
 * @enum EXTI_EICRBBit_t
 * @brief Bit positions inside EICRB (INT4 and INT5 sense control).
 */
typedef enum
{
    EICRB_ISC40 = 0, /**< INT4 sense control bit 0 */
    EICRB_ISC41 = 1, /**< INT4 sense control bit 1 */
    EICRB_ISC50 = 2, /**< INT5 sense control bit 0 */
    EICRB_ISC51 = 3  /**< INT5 sense control bit 1 */
} EXTI_EICRBBit_t;

/**
 * @enum EXTI_EIMSKBit_t
 * @brief Bit positions inside EIMSK (INT4 and INT5 interrupt mask).
 */
typedef enum
{
    EIMSK_INT4 = 4, /**< INT4 interrupt enable (Ignition button, PE4) */
    EIMSK_INT5 = 5  /**< INT5 interrupt enable (Fault Reset button, PE5) */
} EXTI_EIMSKBit_t;

/**
 * @enum EXTI_EIFRBit_t
 * @brief Bit positions inside EIFR (INT4 and INT5 flags, write 1 to clear).
 */
typedef enum
{
    EIFR_INTF4 = 4, /**< INT4 interrupt flag */
    EIFR_INTF5 = 5  /**< INT5 interrupt flag */
} EXTI_EIFRBit_t;

/**
 * @enum EXTI_LineLimit_t
 * @brief Valid range of the line index handled by the driver.
 */
typedef enum
{
    ExtiLineFirst = 4, /**< First line handled by the driver (INT4) */
    ExtiLineLast = 5   /**< Last line handled by the driver (INT5) */
} EXTI_LineLimit_t;

/**
 * @enum EXTI_CallbackIndex_t
 * @brief Index of each line inside the callback array.
 */
typedef enum
{
    ExtiIndexInt4 = 0, /**< Callback slot of INT4 */
    ExtiIndexInt5 = 1, /**< Callback slot of INT5 */
    ExtiLineCount = 2  /**< Number of lines handled by the driver */
} EXTI_CallbackIndex_t;

/**
 * @enum EXTI_SenseMask_t
 * @brief Masks of the sense control field of each line inside EICRB.
 */
typedef enum
{
    ExtiInt4SenseMask = 0x03, /**< ISC41:ISC40 = bits 1:0 -> 0b00000011 */
    ExtiInt5SenseMask = 0x0C  /**< ISC51:ISC50 = bits 3:2 -> 0b00001100 */
} EXTI_SenseMask_t;

/**
 * @def ExtiIsValidLine
 * @brief Checks that a line index is INT4 or INT5.
 * @param Line Line index to check.
 */
#define ExtiIsValidLine(Line) (((Line) >= (uint8_t)ExtiLineFirst) && ((Line) <= (uint8_t)ExtiLineLast))

/**
 * @def ExtiIsValidSense
 * @brief Checks that a sense mode is low level, falling edge or rising edge.
 * @param Mode Sense mode value to check (0x01 is rejected, see EXTI_Interface.h).
 */
#define ExtiIsValidSense(Mode) (((Mode) == (uint8_t)EXTI_Sense_LowLevel) ||    \
                                ((Mode) == (uint8_t)EXTI_Sense_FallingEdge) || \
                                ((Mode) == (uint8_t)EXTI_Sense_RisingEdge))

/**
 * @def ExtiLineIndex
 * @brief Converts a line index (4 or 5) to a zero-based array index.
 * @param Line Line index (INT4 or INT5).
 */
#define ExtiLineIndex(Line) ((uint8_t)((Line) - (uint8_t)ExtiLineFirst))

/**
 * @def ExtiSenseShift
 * @brief Bit position of a line's sense field inside EICRB (INT4 = ISC40, INT5 = ISC50).
 * @param Line Line index (INT4 or INT5).
 */
#define ExtiSenseShift(Line) (((Line) == (uint8_t)ExtiLineFirst) ? (uint8_t)EICRB_ISC40 : (uint8_t)EICRB_ISC50)

/**
 * @def ExtiSenseMask
 * @brief Selects the EICRB sense mask of a line (INT4 or INT5).
 * @param Line Line index (INT4 or INT5).
 */
#define ExtiSenseMask(Line) (((Line) == (uint8_t)ExtiLineFirst) ? (uint8_t)ExtiInt4SenseMask : (uint8_t)ExtiInt5SenseMask)

#endif /* _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_PRIVATE_H */