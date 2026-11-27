/**
 * @file
 *     RegisterSeam.h
 * @brief
 *     TEMPORARY test-side register seam for host unit tests.
 * @details
 *     Atmega128Register.h maps every register macro to a fixed silicon
 *     address (SREG_Reg = data-space address 0x5F). On a PC that address is
 *     not valid memory, so a host test that touches a register crashes, and
 *     Atmega128Register.h has no host-test (#ifdef TEST) branch yet.
 *
 *     This header is force-included into every translation unit of the test
 *     build with the compiler option: -include RegisterSeam.h
 *     It includes the real Atmega128Register.h first, which sets that
 *     header's include guard, then redefines only the two address helpers so
 *     they index g_fakeRegisterBank[]. When a driver later includes
 *     Atmega128Register.h itself, the guard skips it, and every register
 *     macro expands into the fake bank at its real datasheet address. The
 *     real register map is reused unchanged; no production file is edited.
 *
 *     DELETE THIS FILE, and remove "-include RegisterSeam.h" from the test
 *     build command, as soon as Atmega128Register.h redirects its address
 *     helpers itself under #ifdef TEST. No test file includes it directly,
 *     so no test needs to change.
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *     2026-11-26
 * @copyright
 *     Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef REGISTER_SEAM_H
#define REGISTER_SEAM_H

#include "RegisterMock.h"
#include "Atmega128Register.h"   /* Real register map; sets its include guard */

/* Redirect both address helpers into the fake register bank */
#undef  _SetAddress8Bit
#undef  _SetAddress16Bit
#define _SetAddress8Bit(Address)    (g_fakeRegisterBank[(Address)])
#define _SetAddress16Bit(Address)   (*((volatile uint16_t *)&g_fakeRegisterBank[(Address)]))

#endif /* REGISTER_SEAM_H */
