/**
 * @file
 *     RegisterMock.c
 * @brief
 *     Storage and reset of the fake register bank for host unit tests.
 * @details
 *     Defines g_fakeRegisterBank[] and RegMock_Reset(), which sets every
 *     register to 0x00 before each test.
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *     2026-11-26
 * @copyright
 *     Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "RegisterMock.h"
volatile uint8_t g_fakeRegisterBank[REGMOCK_BANK_SIZE];

void RegMock_Reset(void)
{
    uint16_t i;
    for (i = 0U; i < REGMOCK_BANK_SIZE; i++)
    {
        g_fakeRegisterBank[i] = 0x00U;
    }
}
