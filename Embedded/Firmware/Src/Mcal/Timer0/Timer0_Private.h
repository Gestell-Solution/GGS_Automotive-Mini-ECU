/**
 * @file    Timer0_Private.h
 * @brief   Private registers and definitions for Timer0 driver.
 * @details Encapsulates register mappings and hardware-specific constants for ATmega128 Timer0.
 * @version 1.0.0
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @date    2026-10-15
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */
#ifndef TIMER0_PRIVATE_H
#define TIMER0_PRIVATE_H
#include "Atmega128Register.h"
#include "MacroFunction.h"

/**
 * @def TIMER0_MAX_OCR_VALUE
 * @brief Maximum 8-bit compare match value (TOP = 0xFF).
 */
#define TIMER0_MAX_OCR_VALUE        (255U)

#endif /* TIMER0_PRIVATE_H */