/**
 * @file
 *     EXTI_Interface.h
 * @brief
 *     EXTI driver interface APIs.
 * @details
 *     Provides INT4 (Ignition, PE4) and INT5 (Fault Reset, PE5) control and event flag access.
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *     Reviewer:  Beshoy Esmat
 * @date
 *     2026-10-17
 * @copyright
 *    Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef EXTI_INTERFACE_H
#define EXTI_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include "EXTI_Private.h"
#include "EXTI_Config.h"
#include "../../Common/Definition.h"

/**
 * @fn void mEXTI_Init(void)
 * @brief Initializes INT4 and INT5 with the configured trigger mode.
 * @details Resets event flags and callbacks, configures EICRB,
 *          clears pending EIFR flags, then enables EIMSK masks.
 * @note - Call before global interrupts are enabled (GIE_Enable()).
 *       - The line is also gated by the I-bit: no interrupt fires until GIE_Enable().
 *       - PE4 / PE5 must be inputs with pull-up (DIO driver).
 */
void mEXTI_Init(void);

/**
 * @fn void mEXTI_EnableLine(uint8_t Line)
 * @brief Enables the interrupt request of a line.
 * @details Clears the pending flag before unmasking, so an old edge does not fire.
 * @param Line Interrupt line (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @note  Invalid line values are ignored.
 */
void mEXTI_EnableLine(uint8_t Line);

/**
 * @fn void mEXTI_DisableLine(uint8_t Line)
 * @brief Disables the interrupt request of a line.
 * @param Line Interrupt line (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @note  Invalid line values are ignored.
 */
void mEXTI_DisableLine(uint8_t Line);

/**
 * @fn void mEXTI_SetSenseControl(uint8_t Line, uint8_t SenseMode)
 * @brief Configures the trigger mode of a line in EICRB.
 * @details The line is masked while its sense bits change, its flag is cleared,
 *          then the previous mask state is restored. The other line is not touched.
 * @param Line      Interrupt line (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @param SenseMode Trigger mode (EXTI_SenseControl_t).
 * @note  Invalid line or sense values (including reserved 0x01) are ignored.
 */
void mEXTI_SetSenseControl(uint8_t Line, uint8_t SenseMode);

/**
 * @fn void mEXTI_ClearPendingFlag(uint8_t Line)
 * @brief Clears the hardware interrupt flag of a line in EIFR.
 * @param Line Interrupt line (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @note  Invalid line values are ignored.
 */
void mEXTI_ClearPendingFlag(uint8_t Line);

/**
 * @fn bool mEXTI_GetIgnitionFlag(void)
 * @brief Reads the Ignition event flag latched by the INT4 ISR.
 * @return true if an event is pending, false otherwise.
 */
bool mEXTI_GetIgnitionFlag(void);

/**
 * @fn void mEXTI_ClearIgnitionFlag(void)
 * @brief Clears the Ignition event flag (single 8-bit write, atomic).
 */
void mEXTI_ClearIgnitionFlag(void);

/**
 * @fn bool mEXTI_GetResetFlag(void)
 * @brief Reads the Fault Reset event flag latched by the INT5 ISR.
 * @return true if an event is pending, false otherwise.
 */
bool mEXTI_GetResetFlag(void);

/**
 * @fn void mEXTI_ClearResetFlag(void)
 * @brief Clears the Fault Reset event flag (single 8-bit write, atomic).
 */
void mEXTI_ClearResetFlag(void);

/**
 * @fn void mEXTI_SetCallback(uint8_t Line, EXTI_Callback_t CallbackPtr)
 * @brief Registers a callback executed inside the ISR of a line.
 * @param Line        Interrupt line (EXTI_Line_INT4 or EXTI_Line_INT5).
 * @param CallbackPtr Pointer to a void(void) function, or Null to remove it.
 * @warning The callback runs in interrupt context: keep it very short,
 *          no delay, UART or LCD access.
 * @note  - Invalid line values are ignored.
 *        - No effect when EXTI_CALLBACK_ENABLE is Disable.
 */
void mEXTI_SetCallback(uint8_t Line, EXTI_Callback_t CallbackPtr);


#endif /* EXTI_INTERFACE_H */