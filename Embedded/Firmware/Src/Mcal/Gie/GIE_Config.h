/**
 * @file
 *     GIE_Config.h
 * @brief
 *     GIE driver pre-compile configuration.
 * @details
 *     The GIE driver has no pre-compile options: the I-bit position in SREG is
 *     fixed by the ATmega128 datasheet (GIE_Private.h). Like every other MCAL
 *     module, the driver is switched on and off with ModuleGieEnabled in
 *     Common/Config.h. This file completes the module's Private / Config /
 *     Interface / Program file set and provides Enable / Disable for the switch.
 * @version
 *     1.2.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *     2026-11-26
 * @copyright
 *     Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef GIE_CONFIG_H
#define GIE_CONFIG_H

#include "../../Common/Definition.h"

#endif /* GIE_CONFIG_H */
