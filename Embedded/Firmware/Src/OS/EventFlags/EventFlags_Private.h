/**
 * @file
 *     EventFlags_Private.h
 * @brief
 *     EventFlags module private definitions.
 * @details
 *     The event flag queue is a single 16-bit bitmask with no hardware
 *     registers, bit positions or internal types of its own: the public
 *     EVENT_* masks live in EventFlags_Interface.h, and every access is
 *     made atomic through the GIE driver. This file is kept for the
 *     four-file module layout (same precedent as DIO_Config.h).
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *     Reviewer:  salma mohamed
 * @date
 *     2026-11-16
 * @copyright
 *     Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef EVENTFLAGS_PRIVATE_H
#define EVENTFLAGS_PRIVATE_H

/* Intentionally empty: no private definitions are required by this module. */

#endif /* EVENTFLAGS_PRIVATE_H */
