/**
 * @file
 *     EventFlags_Program.c
 * @brief
 *     Implementation of the Event Flag Queue OS module.
 * @details
 *     Holds the single 16-bit system event bitmask. Every read and write is
 *     wrapped in a GIE critical section because a 16-bit access takes two
 *     instructions on the 8-bit AVR core and could otherwise be torn by an
 *     interrupt. All functions may be called from ISR or task context.
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
#include "EventFlags_Interface.h"
#include "EventFlags_Private.h"
#include "EventFlags_Config.h"
#include "../../Mcal/Gie/GIE_Interface.h"

#if ModuleOsEnabled == Enable

/**
 * @brief System event bitmask shared between ISRs (producers) and tasks (consumers).
 */
static volatile EventMask_t g_systemEventFlags = (EventMask_t)EVENT_NONE;

/**
 * @brief Initializes the event flag queue (clears every pending event).
 */
void Event_Init(void)
{
    uint8_t sreg = GIE_SaveAndDisable();

    g_systemEventFlags = (EventMask_t)EVENT_NONE;

    GIE_Restore(sreg);
}

/**
 * @brief Posts (latches) one or more events; bits outside EVENT_ALL_MASK are ignored.
 * @param eventMask Bitwise OR of EVENT_* values to latch.
 */
void Event_Set(EventMask_t eventMask)
{
    EventMask_t validMask = (EventMask_t)(eventMask & (EventMask_t)EVENT_ALL_MASK);
    uint8_t     sreg      = GIE_SaveAndDisable();

    g_systemEventFlags = (EventMask_t)(g_systemEventFlags | validMask);

    GIE_Restore(sreg);
}

/**
 * @brief Clears one or more events without reading them.
 * @param eventMask Bitwise OR of EVENT_* values to clear.
 */
void Event_Clear(EventMask_t eventMask)
{
    EventMask_t keepMask = (EventMask_t)(~eventMask);
    uint8_t     sreg     = GIE_SaveAndDisable();

    g_systemEventFlags = (EventMask_t)(g_systemEventFlags & keepMask);

    GIE_Restore(sreg);
}

/**
 * @brief Tests whether any of the requested (valid) events is pending.
 * @param eventMask Bitwise OR of EVENT_* values to test.
 * @return true when at least one requested valid bit is set, false otherwise.
 */
bool Event_IsSet(EventMask_t eventMask)
{
    EventMask_t validMask = (EventMask_t)(eventMask & (EventMask_t)EVENT_ALL_MASK);
    EventMask_t snapshot;
    uint8_t     sreg      = GIE_SaveAndDisable();

    snapshot = g_systemEventFlags;

    GIE_Restore(sreg);

    return ((EventMask_t)(snapshot & validMask) != (EventMask_t)EVENT_NONE);
}

/**
 * @brief Returns a snapshot of every pending event.
 * @return Current system event bitmask.
 */
EventMask_t Event_GetPending(void)
{
    EventMask_t snapshot;
    uint8_t     sreg = GIE_SaveAndDisable();

    snapshot = g_systemEventFlags;

    GIE_Restore(sreg);

    return snapshot;
}

/**
 * @brief Reads and clears the requested events in one atomic step.
 * @param eventMask Bitwise OR of EVENT_* values to consume.
 * @return Subset of eventMask that was pending.
 */
EventMask_t Event_Consume(EventMask_t eventMask)
{
    EventMask_t matched;
    uint8_t     sreg = GIE_SaveAndDisable();

    matched = (EventMask_t)(g_systemEventFlags & eventMask);
    g_systemEventFlags = (EventMask_t)(g_systemEventFlags & (EventMask_t)(~matched));

    GIE_Restore(sreg);

    return matched;
}

/**
 * @brief Clears every pending event.
 */
void Event_ClearAll(void)
{
    uint8_t sreg = GIE_SaveAndDisable();

    g_systemEventFlags = (EventMask_t)EVENT_NONE;

    GIE_Restore(sreg);
}

/**
 * @brief Tests whether at least one event is pending.
 * @return true when the bitmask is not EVENT_NONE, false otherwise.
 */
bool Event_AnyPending(void)
{
    EventMask_t snapshot;
    uint8_t     sreg = GIE_SaveAndDisable();

    snapshot = g_systemEventFlags;

    GIE_Restore(sreg);

    return (snapshot != (EventMask_t)EVENT_NONE);
}

#endif /* ModuleOsEnabled == Enable */
