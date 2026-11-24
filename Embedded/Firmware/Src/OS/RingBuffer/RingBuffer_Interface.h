/**
 * @file
 *     RingBuffer_Interface.h
 * @brief
 *     Circular RingBuffer (byte FIFO) interface APIs.
 * @details
 *     Provides a fixed-capacity, zero-allocation, single-producer
 *     single-consumer byte FIFO used to decouple USART ISRs from the
 *     cooperative scheduler tasks. The caller owns both the descriptor
 *     (RingBuffer_t) and the storage array. Push and Pop are protected by
 *     a GIE critical section, so either side may run in ISR or task context
 *     (RX: ISR producer, task consumer. TX: task producer, ISR consumer).
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

#ifndef RINGBUFFER_INTERFACE_H
#define RINGBUFFER_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Common/Definition.h"
#include "RingBuffer_Private.h"
#include "RingBuffer_Config.h"

/* Compile-time validation of RingBuffer_Config.h */
#if (RING_BUFFER_DEFAULT_CAPACITY < RING_BUFFER_MIN_CAPACITY) || \
    (RING_BUFFER_DEFAULT_CAPACITY > RING_BUFFER_MAX_CAPACITY) || \
    ((RING_BUFFER_DEFAULT_CAPACITY & (RING_BUFFER_DEFAULT_CAPACITY - 1U)) != 0U)
#error "RING_BUFFER_DEFAULT_CAPACITY must be a power of two in the range 1 .. 128"
#endif

#if RING_BUFFER_INDEX_MASK != (RING_BUFFER_DEFAULT_CAPACITY - 1U)
#error "RING_BUFFER_INDEX_MASK must equal RING_BUFFER_DEFAULT_CAPACITY - 1"
#endif

/**
 * @struct RingBuffer_t
 * @brief Circular byte FIFO descriptor (control block, 7 bytes).
 * @details The storage array is supplied by the caller. head is advanced only
 *          by the producer, tail only by the consumer, count by both inside a
 *          critical section. Members are volatile because one side runs in
 *          interrupt context.
 */
typedef struct {
    uint8_t*            storage;      /**< Caller-owned byte array (Null = invalid descriptor) */
    volatile uint8_t    head;         /**< Write index, advanced by the producer */
    volatile uint8_t    tail;         /**< Read index, advanced by the consumer */
    volatile uint8_t    count;        /**< Number of unread bytes (0 .. capacity) */
    uint8_t             capacity;     /**< Storage size in bytes (power of two, 1 .. 128) */
    volatile bool       overrunFlag;  /**< Latched when a push was dropped on a full buffer */
} RingBuffer_t;

/**
 * @fn void RingBuf_Init(RingBuffer_t* rb, uint8_t* storageBuffer, uint8_t capacity)
 * @brief Binds a descriptor to a caller-owned storage array and empties it.
 * @details Sets storage and capacity, zeroes head, tail and count, and clears
 *          the overrun flag. When storageBuffer is Null, capacity is 0, or
 *          capacity is not a power of two (valid: 1, 2, 4, ... 128), the
 *          descriptor is put into a safe invalid state (storage = Null,
 *          capacity = 0) so that every later call on it fails cleanly.
 * @param[out] rb            Descriptor to initialize.
 * @param[in]  storageBuffer Byte array of at least capacity bytes, owned by the caller.
 * @param[in]  capacity      Size of storageBuffer in bytes (power of two, 1 .. 128).
 * @warning Call before the producer or consumer interrupt is enabled; the
 *          function is not protected against concurrent Push or Pop.
 * @note  - A Null rb is ignored.
 *        - RING_BUFFER_DEFAULT_CAPACITY (64) is the recommended capacity.
 */
void RingBuf_Init(RingBuffer_t* rb, uint8_t* storageBuffer, uint8_t capacity);

/**
 * @fn Std_ReturnType RingBuf_Push(RingBuffer_t* rb, uint8_t dataByte)
 * @brief Appends one byte at the write index (producer side).
 * @details Inside one GIE critical section: when the buffer is full the byte
 *          is dropped, the overrun flag is latched and ENotOk is returned,
 *          leaving the unread bytes intact. Otherwise the byte is stored,
 *          head is advanced with wrap-around and count is incremented.
 * @param[in,out] rb       Initialized descriptor.
 * @param[in]     dataByte Byte to append.
 * @return Operation status.
 * @retval EOk    Byte stored.
 * @retval ENotOk Buffer full (byte dropped, overrun latched), or rb or storage is Null.
 * @warning Interrupts are disabled for the duration of the update (a few
 *          instructions); the previous I-bit state is restored on exit.
 * @note  Safe to call from an ISR or from a task.
 */
Std_ReturnType RingBuf_Push(RingBuffer_t* rb, uint8_t dataByte);

/**
 * @fn Std_ReturnType RingBuf_Pop(RingBuffer_t* rb, uint8_t* outDataByte)
 * @brief Removes the oldest byte at the read index (consumer side).
 * @details Inside one GIE critical section: when the buffer is empty ENotOk is
 *          returned and tail is untouched. Otherwise the byte is copied to
 *          outDataByte, tail is advanced with wrap-around and count is
 *          decremented.
 * @param[in,out] rb          Initialized descriptor.
 * @param[out]    outDataByte Destination of the removed byte.
 * @return Operation status.
 * @retval EOk    One byte removed and written to outDataByte.
 * @retval ENotOk Buffer empty, or rb, storage or outDataByte is Null (outDataByte unchanged).
 * @warning Interrupts are disabled for the duration of the update (a few
 *          instructions); the previous I-bit state is restored on exit.
 * @note  Safe to call from an ISR or from a task.
 */
Std_ReturnType RingBuf_Pop(RingBuffer_t* rb, uint8_t* outDataByte);

/**
 * @fn Std_ReturnType RingBuf_Peek(const RingBuffer_t* rb, uint8_t* outDataByte)
 * @brief Reads the oldest byte without removing it.
 * @details Same as RingBuf_Pop() but tail and count are left unchanged.
 * @param[in]  rb          Initialized descriptor.
 * @param[out] outDataByte Destination of the oldest byte.
 * @return Operation status.
 * @retval EOk    Oldest byte copied to outDataByte.
 * @retval ENotOk Buffer empty, or rb, storage or outDataByte is Null (outDataByte unchanged).
 * @note  Runs inside a GIE critical section; safe from ISR or task context.
 */
Std_ReturnType RingBuf_Peek(const RingBuffer_t* rb, uint8_t* outDataByte);

/**
 * @fn bool RingBuf_IsEmpty(const RingBuffer_t* rb)
 * @brief Reports whether the buffer holds no unread byte.
 * @param[in] rb Descriptor to inspect.
 * @return true when count is 0, when rb is Null, or when the descriptor is invalid.
 * @note  Single 8-bit volatile read (atomic on AVR), no critical section.
 */
bool RingBuf_IsEmpty(const RingBuffer_t* rb);

/**
 * @fn bool RingBuf_IsFull(const RingBuffer_t* rb)
 * @brief Reports whether the buffer cannot accept another byte.
 * @param[in] rb Descriptor to inspect.
 * @return true when count equals capacity; false when rb is Null or the descriptor is invalid.
 * @note  Single 8-bit volatile read (atomic on AVR), no critical section.
 */
bool RingBuf_IsFull(const RingBuffer_t* rb);

/**
 * @fn uint8_t RingBuf_GetCount(const RingBuffer_t* rb)
 * @brief Returns the number of unread bytes.
 * @param[in] rb Descriptor to inspect.
 * @return Unread byte count (0 .. capacity); 0 when rb is Null or the descriptor is invalid.
 * @note  Single 8-bit volatile read (atomic on AVR), no critical section.
 */
uint8_t RingBuf_GetCount(const RingBuffer_t* rb);

/**
 * @fn uint8_t RingBuf_GetFreeSpace(const RingBuffer_t* rb)
 * @brief Returns how many bytes can still be pushed.
 * @param[in] rb Descriptor to inspect.
 * @return capacity minus count; 0 when rb is Null or the descriptor is invalid.
 * @note  Single 8-bit volatile read of count, no critical section.
 */
uint8_t RingBuf_GetFreeSpace(const RingBuffer_t* rb);

/**
 * @fn void RingBuf_Flush(RingBuffer_t* rb)
 * @brief Discards every unread byte.
 * @details Inside one GIE critical section head, tail and count are reset to
 *          0. The overrun flag is left unchanged so that a drop that happened
 *          before the flush is still reported.
 * @param[in,out] rb Descriptor to empty.
 * @note  - A Null rb is ignored.
 *        - Use RingBuf_ClearOverrun() to acknowledge an overrun.
 */
void RingBuf_Flush(RingBuffer_t* rb);

/**
 * @fn bool RingBuf_HasOverrun(const RingBuffer_t* rb)
 * @brief Reports whether a push was dropped since the last RingBuf_ClearOverrun().
 * @param[in] rb Descriptor to inspect.
 * @return true when the overrun flag is latched; false when rb is Null.
 * @note  Single 8-bit volatile read (atomic on AVR), no critical section.
 */
bool RingBuf_HasOverrun(const RingBuffer_t* rb);

/**
 * @fn void RingBuf_ClearOverrun(RingBuffer_t* rb)
 * @brief Acknowledges and clears the overrun flag.
 * @param[in,out] rb Descriptor to update.
 * @note  - Single 8-bit write (atomic on AVR), no critical section.
 *        - A Null rb is ignored.
 */
void RingBuf_ClearOverrun(RingBuffer_t* rb);

#endif /* RINGBUFFER_INTERFACE_H */
