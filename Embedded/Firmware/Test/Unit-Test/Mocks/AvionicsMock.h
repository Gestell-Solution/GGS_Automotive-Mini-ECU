/**
 * @file      AvionicsMock.h
 * @brief     Host-build compatibility shims for AVR-specific constructs.
 * @details   When compiling MCAL sources on a host PC (Linux/Windows) with
 *            a standard GCC toolchain, several AVR-specific symbols are
 *            unavailable. This header provides minimal stubs so the MCAL
 *            source files compile cleanly without modification.
 *
 *            Include this file BEFORE any MCAL or Atmega8Register.h header
 *            in every test file, immediately after RegisterMock.h.
 *
 *            Provided stubs:
 *            - __attribute__((signal))   — AVR ISR attribute (no-op on host)
 *            - __attribute__((interrupt))— AVR ISR attribute (no-op on host)
 *            - sei() / cli()             — Global interrupt enable/disable
 *            - NULL                      — Null pointer constant
 *            - F_CPU                     — CPU frequency used in baud-rate tests
 *
 * @version   1.0.0
 * @date      2026-10-08
 * @author    Hesham Ahmed <Hisham.ah.hamed@gmail.com>
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef AVIONICS_MOCK_H
#define AVIONICS_MOCK_H

#include <stdint.h>
#include <stddef.h>   /* NULL */

/* --------------------------------------------------------------------------
 *  AVR GCC attributes — silently ignored on host GCC
 * -------------------------------------------------------------------------- */
#ifndef __AVR__
/* __attribute__((signal)) is AVR-specific; host GCC accepts it but ignores it.
   No extra stub needed — leave it as-is so vector ISR definitions compile. */
#endif

/* --------------------------------------------------------------------------
 *  Global Interrupt Helpers (sei / cli)
 *  The GIE driver wraps these in inline asm. On the host we stub them as
 *  empty macros so the driver's .c file compiles without avr/interrupt.h.
 * -------------------------------------------------------------------------- */
#ifndef sei
/**
 * @def   sei
 * @brief Stub for AVR 'set global interrupt enable' instruction.
 *        On the host this expands to nothing; the mock SREG bit is set
 *        by GIE_Enable() itself via the register mock bank.
 */
#define sei()  /* host stub — no-op */
#endif

#ifndef cli
/**
 * @def   cli
 * @brief Stub for AVR 'clear global interrupt enable' instruction.
 */
#define cli()  /* host stub — no-op */
#endif

/* --------------------------------------------------------------------------
 *  F_CPU — used by baud-rate calculation tests
 * -------------------------------------------------------------------------- */
#ifndef F_CPU
/**
 * @def   F_CPU
 * @brief CPU frequency in Hz, matching the ATmega8 hardware configuration.
 */
#define F_CPU  8000000UL
#endif

/* --------------------------------------------------------------------------
 *  Enable / Disable constants (mirror Common/Definitions.h)
 * -------------------------------------------------------------------------- */
#ifndef Enable
#define Enable  1
#endif
#ifndef Disable
#define Disable 0
#endif

#endif /* AVIONICS_MOCK_H */
