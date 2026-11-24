/**
 * @file
 *     RingBuffer_Program.c
 * @brief
 *     Implementation of the circular RingBuffer (byte FIFO) OS module.
 * @details
 *     Push, Pop, Peek and Flush update head, tail and count inside one GIE
 *     critical section each, so the buffer is correct for both the RX
 *     direction (ISR producer, task consumer) and the TX direction (task
 *     producer, ISR consumer). The read-only queries use single 8-bit
 *     volatile reads, which are atomic on AVR. The module owns no buffer
 *     instance and no static data.
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
#include <stdint.h>
#include <stdbool.h>
#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include "RingBuffer_Interface.h"
#include "RingBuffer_Private.h"
#include "RingBuffer_Config.h"
#include "../../Mcal/Gie/GIE_Interface.h"

#if ModuleOsEnabled == Enable

/**
 * @brief Binds a descriptor to a caller-owned storage array and empties it.
 * @param rb            Descriptor to initialize (Null is ignored).
 * @param storageBuffer Caller-owned byte array of at least capacity bytes.
 * @param capacity      Size of storageBuffer in bytes (power of two, 1 .. 128).
 */
void RingBuf_Init(RingBuffer_t* rb, uint8_t* storageBuffer, uint8_t capacity)
{
    if (rb != Null)
    {
        bool isCapacityValid = RingBuf_IsPowerOfTwo(capacity);

        if ((storageBuffer != Null) && isCapacityValid)
        {
            rb->storage  = storageBuffer;
            rb->capacity = capacity;
        }
        else
        {
            /* Safe invalid state: every later call fails cleanly */
            rb->storage  = Null;
            rb->capacity = 0U;
        }

        rb->head        = 0U;
        rb->tail        = 0U;
        rb->count       = 0U;
        rb->overrunFlag = false;
    }
}

/**
 * @brief Appends one byte at the write index (producer side).
 * @param rb       Initialized descriptor.
 * @param dataByte Byte to append.
 * @return EOk when stored, ENotOk when full (overrun latched) or rb / storage is Null.
 */
Std_ReturnType RingBuf_Push(RingBuffer_t* rb, uint8_t dataByte)
{
    Std_ReturnType status = ENotOk;

    if ((rb != Null) && (rb->storage != Null))
    {
        uint8_t sreg = GIE_SaveAndDisable();

        if (rb->count >= rb->capacity)
        {
            /* Full: drop the new byte, keep the unread bytes intact */
            rb->overrunFlag = true;
        }
        else
        {
            rb->storage[rb->head] = dataByte;
            rb->head = RingBuf_NextIndex(rb->head, rb->capacity);
            rb->count++;
            status = EOk;
        }

        GIE_Restore(sreg);
    }

    return status;
}

/**
 * @brief Removes the oldest byte at the read index (consumer side).
 * @param rb          Initialized descriptor.
 * @param outDataByte Destination of the removed byte.
 * @return EOk when a byte was removed, ENotOk when empty or a pointer is Null.
 */
Std_ReturnType RingBuf_Pop(RingBuffer_t* rb, uint8_t* outDataByte)
{
    Std_ReturnType status = ENotOk;

    if ((rb != Null) && (outDataByte != Null) && (rb->storage != Null))
    {
        uint8_t sreg = GIE_SaveAndDisable();

        if (rb->count != 0U)
        {
            *outDataByte = rb->storage[rb->tail];
            rb->tail = RingBuf_NextIndex(rb->tail, rb->capacity);
            rb->count--;
            status = EOk;
        }

        GIE_Restore(sreg);
    }

    return status;
}

/**
 * @brief Reads the oldest byte without removing it.
 * @param rb          Initialized descriptor.
 * @param outDataByte Destination of the oldest byte.
 * @return EOk when a byte was copied, ENotOk when empty or a pointer is Null.
 */
Std_ReturnType RingBuf_Peek(const RingBuffer_t* rb, uint8_t* outDataByte)
{
    Std_ReturnType status = ENotOk;

    if ((rb != Null) && (outDataByte != Null) && (rb->storage != Null))
    {
        uint8_t sreg = GIE_SaveAndDisable();

        if (rb->count != 0U)
        {
            *outDataByte = rb->storage[rb->tail];
            status = EOk;
        }

        GIE_Restore(sreg);
    }

    return status;
}

/**
 * @brief Reports whether the buffer holds no unread byte.
 * @param rb Descriptor to inspect.
 * @return true when count is 0 or rb is Null, false otherwise.
 */
bool RingBuf_IsEmpty(const RingBuffer_t* rb)
{
    bool isEmpty = true;

    if (rb != Null)
    {
        isEmpty = (rb->count == 0U);
    }

    return isEmpty;
}

/**
 * @brief Reports whether the buffer cannot accept another byte.
 * @param rb Descriptor to inspect.
 * @return true when count equals capacity, false when rb is Null or invalid.
 */
bool RingBuf_IsFull(const RingBuffer_t* rb)
{
    bool isFull = false;

    if ((rb != Null) && (rb->storage != Null))
    {
        isFull = (rb->count >= rb->capacity);
    }

    return isFull;
}

/**
 * @brief Returns the number of unread bytes.
 * @param rb Descriptor to inspect.
 * @return Unread byte count, 0 when rb is Null or invalid.
 */
uint8_t RingBuf_GetCount(const RingBuffer_t* rb)
{
    uint8_t count = 0U;

    if (rb != Null)
    {
        count = rb->count;
    }

    return count;
}

/**
 * @brief Returns how many bytes can still be pushed.
 * @param rb Descriptor to inspect.
 * @return capacity minus count, 0 when rb is Null or invalid.
 */
uint8_t RingBuf_GetFreeSpace(const RingBuffer_t* rb)
{
    uint8_t freeSpace = 0U;

    if (rb != Null)
    {
        freeSpace = (uint8_t)(rb->capacity - rb->count);
    }

    return freeSpace;
}

/**
 * @brief Discards every unread byte; the overrun flag is left unchanged.
 * @param rb Descriptor to empty (Null is ignored).
 */
void RingBuf_Flush(RingBuffer_t* rb)
{
    if (rb != Null)
    {
        uint8_t sreg = GIE_SaveAndDisable();

        rb->head  = 0U;
        rb->tail  = 0U;
        rb->count = 0U;

        GIE_Restore(sreg);
    }
}

/**
 * @brief Reports whether a push was dropped since the last RingBuf_ClearOverrun().
 * @param rb Descriptor to inspect.
 * @return true when the overrun flag is latched, false when rb is Null.
 */
bool RingBuf_HasOverrun(const RingBuffer_t* rb)
{
    bool hasOverrun = false;

    if (rb != Null)
    {
        hasOverrun = rb->overrunFlag;
    }

    return hasOverrun;
}

/**
 * @brief Acknowledges and clears the overrun flag.
 * @param rb Descriptor to update (Null is ignored).
 */
void RingBuf_ClearOverrun(RingBuffer_t* rb)
{
    if (rb != Null)
    {
        rb->overrunFlag = false;
    }
}

#endif /* ModuleOsEnabled == Enable */
