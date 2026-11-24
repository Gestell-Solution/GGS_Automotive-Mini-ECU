/**
 * @file
 *     GIE_Private.h
 * @brief
 *     GIE driver private definitions.
 * @details
 *     Contains the Global Interrupt Enable bit position and mask inside SREG.
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

#ifndef GIE_PRIVATE_H
#define GIE_PRIVATE_H

/* SREG Register Bits */
#define GIE_Bit       7       /* I-bit: Global Interrupt Enable (bit position) */

/* SREG Register Masks */
#define GIE_IBitMask  0x80U   /* I-bit: Global Interrupt Enable (bit mask, 1U << GIE_Bit) */

#endif /* GIE_PRIVATE_H */
