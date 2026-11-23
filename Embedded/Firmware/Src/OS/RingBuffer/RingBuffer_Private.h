/**
 * @file
 *     RingBuffer_Private.h
 * @brief
 *     RingBuffer module private definitions.
 * @details
 *     Contains the capacity limits and the internal helper macros
 *     (power-of-two check, wrap-around index arithmetic) of the
 *     circular byte FIFO. No hardware access: the module is pure software.
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

#ifndef RINGBUFFER_PRIVATE_H
#define RINGBUFFER_PRIVATE_H

#include <stdint.h>

/**
 * @def RING_BUFFER_MIN_CAPACITY
 * @brief Smallest capacity accepted by RingBuf_Init() (bytes).
 */
#define RING_BUFFER_MIN_CAPACITY   1U

/**
 * @def RING_BUFFER_MAX_CAPACITY
 * @brief Largest capacity accepted by RingBuf_Init() (bytes).
 * @details Indices and the occupancy counter are 8-bit, and the capacity
 *          must be a power of two, so 128 is the largest usable value.
 */
#define RING_BUFFER_MAX_CAPACITY   128U

/**
 * @def RingBuf_IsPowerOfTwo(Capacity)
 * @brief Evaluates to true when Capacity is a power of two inside the accepted range.
 * @details A power of two has exactly one bit set, so (n & (n - 1)) == 0.
 *          The zero case is excluded explicitly because 0 & 0xFF is also 0.
 * @param Capacity Capacity to test (uint8_t).
 */
#define RingBuf_IsPowerOfTwo(Capacity) \
    (((uint8_t)(Capacity) >= (uint8_t)RING_BUFFER_MIN_CAPACITY) && \
     ((uint8_t)(Capacity) <= (uint8_t)RING_BUFFER_MAX_CAPACITY) && \
     (((uint8_t)(Capacity) & (uint8_t)((uint8_t)(Capacity) - 1U)) == 0U))

/**
 * @def RingBuf_NextIndex(Index, Capacity)
 * @brief Advances a head or tail index by one with wrap-around.
 * @details Capacity is a power of two, so the modulo reduces to a bitwise
 *          AND with (Capacity - 1), which is a single-cycle operation on AVR.
 * @param Index    Current index (0 .. Capacity - 1).
 * @param Capacity Buffer capacity (power of two, 1 .. 128).
 */
#define RingBuf_NextIndex(Index, Capacity) \
    ((uint8_t)(((uint8_t)(Index) + 1U) & (uint8_t)((uint8_t)(Capacity) - 1U)))

#endif /* RINGBUFFER_PRIVATE_H */
