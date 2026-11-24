/**
 * @file
 *     RingBuffer_Config.h
 * @brief
 *     RingBuffer module pre-compile configuration parameters.
 * @details
 *     Contains the default capacity recommended for the USART RX and TX
 *     buffers and its matching index mask. The module itself owns no
 *     buffer instance: every caller passes its own storage and capacity
 *     to RingBuf_Init(), so these values are the project-wide defaults.
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

#ifndef RINGBUFFER_CONFIG_H
#define RINGBUFFER_CONFIG_H

#include "RingBuffer_Private.h"

/**
 * @def RING_BUFFER_DEFAULT_CAPACITY
 * @brief Default byte capacity of one buffer instance (USART0 and USART1, RX and TX).
 * @details Must be a power of two in the range 1 .. 128 (checked at compile time
 *          in RingBuffer_Interface.h). 64 bytes hold about 5.5 ms of traffic
 *          at 115200 bps, enough for the 10 ms command-parser task period.
 */
#define RING_BUFFER_DEFAULT_CAPACITY    64U

/**
 * @def RING_BUFFER_INDEX_MASK
 * @brief Wrap-around mask matching RING_BUFFER_DEFAULT_CAPACITY (capacity - 1).
 * @details Must equal RING_BUFFER_DEFAULT_CAPACITY - 1 (checked at compile time).
 */
#define RING_BUFFER_INDEX_MASK          0x3FU

#endif /* RINGBUFFER_CONFIG_H */
