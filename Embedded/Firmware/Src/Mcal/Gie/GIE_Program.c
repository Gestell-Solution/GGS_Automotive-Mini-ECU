/**
 * @file
 *     GIE_Program.c
 * @brief
 *     Implementation of the Global Interrupt Enable (GIE) MCAL driver.
 * @version
 *     1.1.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *     Reviewer:  Beshoy Esmat
 * @date
 *     2026-11-16
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

/**
 * @brief Saves the current SREG and clears the I-bit (enters a critical section).
 * @return Full SREG snapshot taken before the I-bit was cleared.
 */
uint8_t GIE_SaveAndDisable(void)
{
    uint8_t state = SREG_Reg;

    SREG_Reg = (uint8_t)(state & (uint8_t)(~GIE_IBitMask));

    return state;
}

/**
 * @brief Writes a saved SREG snapshot back (leaves a critical section).
 * @param State Full SREG snapshot returned by the matching GIE_SaveAndDisable().
 */
void GIE_Restore(uint8_t State)
{
    SREG_Reg = State;
}
