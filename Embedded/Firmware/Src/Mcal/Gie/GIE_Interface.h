/**
 * @file
 *     GIE_Interface.h
 * @brief
 *     GIE (Global Interrupt Enable) driver interface APIs.
 * @details
 *     Controls the I-bit of SREG. Every interrupt source (EXTI, timers, ...)
 *     also needs its own local enable bit; this is the master switch.
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *     Reviewer:  Beshoy Esmat
 * @date
 *     2026-10-18
 * @copyright
 *     Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef GIE_INTERFACE_H
#define GIE_INTERFACE_H

#include <stdint.h>
#include "GIE_Private.h"

/**
 * @fn void GIE_Enable(void)
 * @brief Sets the I-bit of SREG (global interrupts enabled).
 * @note  Call once in main() after all drivers are initialized.
 */
void GIE_Enable(void);

/**
 * @fn void GIE_Disable(void)
 * @brief Clears the I-bit of SREG (all interrupts blocked).
 */
void GIE_Disable(void);

#endif /* GIE_INTERFACE_H */
