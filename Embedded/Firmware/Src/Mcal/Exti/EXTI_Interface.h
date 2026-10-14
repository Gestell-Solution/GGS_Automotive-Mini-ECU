#ifndef _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_INTERFACE_H
#define _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_INTERFACE_H

/**
 * @file
 *      EXTI_Interface.h
 * @brief
 *      Public interface of the External Interrupt (EXTI) MCAL driver.
 * @details
 *      Controls INT4 (PE4, Ignition push-button) and INT5 (PE5, Fault Reset
 *      push-button) of the ATmega128. The ISRs only latch volatile event flags
 *      and optionally call a user callback. Debouncing is not done here; the
 *      OS scheduler validates the flags over two consecutive 10 ms ticks.
 * @version
 *      1.0.0
 * @author
 *      Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *      2026-10-14
 * @copyright
 *      Copyright (c) 2026 Gestell. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * @enum EXTI_Line_t
 * @brief External interrupt lines available on the ATmega128 for this ECU.
 */
typedef enum
{
    EXTI_Line_INT4 = 4, /**< PE4: Cockpit Ignition push-button */
    EXTI_Line_INT5 = 5  /**< PE5: Cockpit Fault Reset push-button */
} EXTI_Line_t;

/**
 * @enum EXTI_SenseControl_t
 * @brief Interrupt sense control modes (EICRB bits ISC4x / ISC5x).
 */
typedef enum
{
    EXTI_Sense_LowLevel = 0x00,    /**< Low level of the pin generates an interrupt */
    EXTI_Sense_FallingEdge = 0x02, /**< Falling edge generates an interrupt (default) */
    EXTI_Sense_RisingEdge = 0x03   /**< Rising edge generates an interrupt */
} EXTI_SenseControl_t;

/**
 * @enum EXTI_Status_t
 * @brief EXTI operation return status codes.
 */
typedef enum
{
    EXTI_Status_Ok = 0,   /**< Operation completed successfully */
    EXTI_Status_Error = 1 /**< Operation failed due to an invalid argument */
} EXTI_Status_t;

/**
 * @typedef EXTI_Callback_t
 * @brief Function pointer type of an ISR callback.
 * @warning The callback runs in interrupt context and must be extremely short.
 */
typedef void (*EXTI_Callback_t)(void);

/*API's*/

/**
 * @fn mEXTI_Init
 * @brief Initializes INT4 and INT5 for the configured trigger mode.
 * @details Resets the event flags and callbacks, writes the sense control bits
 *          of EICRB, flushes stale flags in EIFR, then unmasks the lines
 *          selected in EXTI_Config.h through EIMSK.
 * @warning Must be called before global interrupts are enabled.
 */
void mEXTI_Init(void);

/**
 * @fn mEXTI_EnableLine
 * @brief Enables the hardware interrupt request of a line.
 * @details Clears the pending flag in EIFR first, so an edge latched while the
 *          line was disabled does not fire. Other lines are not affected.
 * @param[in] Line Line index (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @note An invalid line is ignored.
 */
void mEXTI_EnableLine(uint8_t Line);

/**
 * @fn mEXTI_DisableLine
 * @brief Disables the hardware interrupt request of a line.
 * @details Masks the line in EIMSK. Other lines are not affected.
 * @param[in] Line Line index (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @note An invalid line is ignored.
 */
void mEXTI_DisableLine(uint8_t Line);

/**
 * @fn mEXTI_SetSenseControl
 * @brief Configures the trigger sensitivity of a line.
 * @details Follows the datasheet rule: the line is masked while its sense bits
 *          change, the stale flag is cleared, then the previous mask state is
 *          restored. EICRB is changed with a read-modify-write, so the other
 *          line is preserved.
 * @param[in] Line      Line index (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @param[in] SenseMode Trigger mode (EXTI_SenseControl_t).
 * @note An invalid line or the value 0x01 is ignored.
 * @todo For INT7:4 the datasheet defines 0x01 as "any logical change". It is
 *       rejected here because the user story lists 0x01 as reserved.
 */
void mEXTI_SetSenseControl(uint8_t Line, uint8_t SenseMode);

/**
 * @fn mEXTI_ClearPendingFlag
 * @brief Clears the hardware interrupt flag of a line in EIFR.
 * @details On AVR, writing a logical one to a flag bit clears it. Writing zero
 *          leaves the other flags untouched.
 * @param[in] Line Line index (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @note An invalid line is ignored.
 */
void mEXTI_ClearPendingFlag(uint8_t Line);

/**
 * @fn mEXTI_GetIgnitionFlag
 * @brief Reads the Ignition button event flag latched by the INT4 ISR.
 * @return State of the event flag.
 * @retval true  A button event is pending.
 * @retval false No event is pending.
 */
bool mEXTI_GetIgnitionFlag(void);

/**
 * @fn mEXTI_ClearIgnitionFlag
 * @brief Clears the Ignition button event flag.
 * @details A single 8-bit write, therefore atomic on the AVR.
 */
void mEXTI_ClearIgnitionFlag(void);

/**
 * @fn mEXTI_GetResetFlag
 * @brief Reads the Fault Reset button event flag latched by the INT5 ISR.
 * @return State of the event flag.
 * @retval true  A button event is pending.
 * @retval false No event is pending.
 */
bool mEXTI_GetResetFlag(void);

/**
 * @fn mEXTI_ClearResetFlag
 * @brief Clears the Fault Reset button event flag.
 * @details A single 8-bit write, therefore atomic on the AVR.
 */
void mEXTI_ClearResetFlag(void);

/**
 * @fn mEXTI_SetCallback
 * @brief Registers a callback executed inside the ISR of a line.
 * @details Passing Null removes the callback. The line is masked briefly while
 *          the 16-bit pointer is written, so the ISR never reads a half-updated
 *          pointer. An edge during that time stays latched in EIFR.
 * @param[in] Line        Line index (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @param[in] CallbackPtr Pointer to a void(void) function, or Null.
 * @warning The callback runs in interrupt context. Do not delay, block, send
 *          over UART or write to the display inside it.
 * @note Has no effect when ExtiCallbackEnabled is Disable.
 */
void mEXTI_SetCallback(uint8_t Line, EXTI_Callback_t CallbackPtr);

/*ISR*/

/**
 * @def ExtiIsrAttribute
 * @brief Marks a function as an AVR interrupt handler.
 * @details Expands to __attribute__((signal)) only when the compiler really
 *          supports that attribute (avr-gcc). On any other compiler or target
 *          (host unit tests, IDE analysis) it expands to nothing, so the
 *          "unknown attribute 'signal' ignored" warning does not appear.
 */
#if defined(__has_attribute)
#if __has_attribute(signal)
#define ExtiIsrAttribute __attribute__((signal))
#endif
#endif
#ifndef ExtiIsrAttribute
#define ExtiIsrAttribute
#endif
/**
 * @fn __vector_5
 * @brief Interrupt service routine of INT4 (Ignition button).
 * @details Sets the ignition event flag and calls the registered callback.
 * @warning Hardware entry point. Never call it from application code.
 */
void __vector_5(void) ExtiIsrAttribute;

/**
 * @fn __vector_6
 * @brief Interrupt service routine of INT5 (Fault Reset button).
 * @details Sets the reset event flag and calls the registered callback.
 * @warning Hardware entry point. Never call it from application code.
 */
void __vector_6(void) ExtiIsrAttribute;

#endif // _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_INTERFACE_H