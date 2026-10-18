/**
 * @file
 *     GIE_Program.c
 * @brief
 *     Implementation of the Global Interrupt Enable (GIE) MCAL driver.
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
#include "../../Common/MacroFunction.h"
#include "../Atmega128Register.h"
#include "GIE_Interface.h"
#include "GIE_Private.h"

/**
 * @brief Sets the I-bit of SREG.
 */
void GIE_Enable(void)
{
    SetBit(SREG_Reg, GIE_Bit);
}

/**
 * @brief Clears the I-bit of SREG.
 */
void GIE_Disable(void)
{
    ClearBit(SREG_Reg, GIE_Bit);
}

