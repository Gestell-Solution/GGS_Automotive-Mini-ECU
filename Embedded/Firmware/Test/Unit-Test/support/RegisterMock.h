/**
 * @file
 *     RegisterMock.h
 * @brief
 *     Fake register bank for host unit tests.
 * @details
 *     256-byte RAM array indexed by the real ATmega128 data-space address
 *     (0x00..0xFF). With the register seam active, every MCAL register macro
 *     (SREG_Reg, EIFR_Reg, ...) reads and writes this array instead of silicon.
 *     Linked into every test; call RegMock_Reset() from setUp().
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *     2026-11-26
 * @copyright
 *     Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef REGISTER_MOCK_H
#define REGISTER_MOCK_H
#include <stdint.h>
#define REGMOCK_BANK_SIZE   0x100U        /* Data space 0x00..0xFF covers all ATmega128 I/O */
extern volatile uint8_t g_fakeRegisterBank[REGMOCK_BANK_SIZE];
void RegMock_Reset(void);                 /* All registers = 0x00, call from setUp() */
#endif /* REGISTER_MOCK_H */
