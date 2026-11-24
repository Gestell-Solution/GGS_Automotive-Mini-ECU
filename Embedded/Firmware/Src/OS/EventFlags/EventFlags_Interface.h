/**
 * @file
 *     EventFlags_Interface.h
 * @brief
 *     Event Flag Queue interface APIs and system event bit allocation.
 * @details
 *     Provides atomic set, clear, test and consume operations on a single
 *     16-bit system event bitmask. Interrupt service routines (EXTI
 *     push-buttons, USART frame parsers) post events; the FSM core, the
 *     fault manager and the command dispatcher consume them in task
 *     context. Every access is wrapped in a GIE critical section because a
 *     16-bit read or write is not atomic on the 8-bit AVR core.
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

#ifndef EVENTFLAGS_INTERFACE_H
#define EVENTFLAGS_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Common/Definition.h"
#include "EventFlags_Private.h"
#include "EventFlags_Config.h"

/**
 * @def EVENT_NONE
 * @brief No event pending (empty bitmask).
 */
#define EVENT_NONE                      0x0000U

/**
 * @def EVENT_IGNITION_ON
 * @brief Bit 0: driver requested the engine start sequence (INT4 ISR or BT command). Consumer: FSM core.
 */
#define EVENT_IGNITION_ON               0x0001U

/**
 * @def EVENT_IGNITION_OFF
 * @brief Bit 1: driver commanded engine shutdown to OFF (INT4 ISR or BT command). Consumer: FSM core.
 */
#define EVENT_IGNITION_OFF              0x0002U

/**
 * @def EVENT_FAULT_RESET
 * @brief Bit 2: fault reset button pressed or CLEAR_DTC received (INT5 ISR). Consumer: fault and DTC manager.
 */
#define EVENT_FAULT_RESET               0x0004U

/**
 * @def EVENT_DIAG_REQ
 * @brief Bit 3: service tool requested DIAGNOSTIC state entry (BT or Wi-Fi command). Consumer: FSM core.
 */
#define EVENT_DIAG_REQ                  0x0008U

/**
 * @def EVENT_BT_FRAME_READY
 * @brief Bit 4: a complete TELE frame terminated by a line feed is in the USART0 buffer. Consumer: command dispatcher.
 */
#define EVENT_BT_FRAME_READY            0x0010U

/**
 * @def EVENT_WIFI_FRAME_READY
 * @brief Bit 5: a complete JSON or AT response frame is in the USART1 buffer. Consumer: command dispatcher.
 */
#define EVENT_WIFI_FRAME_READY          0x0020U

/**
 * @def EVENT_OVERTEMP_ALERT
 * @brief Bit 6: coolant temperature crossed the EngineTempMax boundary (ADC sampling task). Consumer: safety FSM.
 */
#define EVENT_OVERTEMP_ALERT            0x0040U

/**
 * @def EVENT_BATTERY_ALERT
 * @brief Bit 7: battery voltage left the BatteryVoltageMin .. BatteryVoltageMax window (ADC sampling task). Consumer: safety FSM.
 */
#define EVENT_BATTERY_ALERT             0x0080U

/**
 * @def EVENT_SENSOR_DISC_ALERT
 * @brief Bit 8: raw ADC count indicates a disconnected sensor, open or short (ADC sampling task). Consumer: safety FSM.
 */
#define EVENT_SENSOR_DISC_ALERT         0x0100U

/**
 * @def EVENT_COMMS_TIMEOUT
 * @brief Bit 9: heartbeat lost from the wireless client for longer than the comms watchdog threshold. Consumer: safety FSM.
 */
#define EVENT_COMMS_TIMEOUT             0x0200U

/**
 * @def EVENT_ALL_MASK
 * @brief Union of every defined event bit (bits 0 .. 9). Bits outside this mask are ignored by Event_Set().
 */
#define EVENT_ALL_MASK                  0x03FFU

/**
 * @typedef EventMask_t
 * @brief Bitmask of system events (one bit per EVENT_* definition).
 */
typedef uint16_t EventMask_t;

/**
 * @fn void Event_Init(void)
 * @brief Initializes the event flag queue (clears every pending event).
 * @details Writes EVENT_NONE to the system event bitmask inside a critical section.
 * @note  Call once during system initialization before any producer interrupt
 *        is enabled (GIE_Enable()).
 */
void Event_Init(void);

/**
 * @fn void Event_Set(EventMask_t eventMask)
 * @brief Posts (latches) one or more events.
 * @details ORs eventMask, limited to EVENT_ALL_MASK, into the system event
 *          bitmask inside a GIE critical section, so a set from an ISR cannot
 *          clobber a set or consume in progress in task context.
 * @param[in] eventMask Bitwise OR of EVENT_* values to latch.
 * @warning Interrupts are disabled for a few instructions; the previous
 *          I-bit state is restored on exit.
 * @note  - Bits outside EVENT_ALL_MASK are ignored.
 *        - Safe to call from an ISR or from a task.
 *        - An event already set stays set (no counting).
 */
void Event_Set(EventMask_t eventMask);

/**
 * @fn void Event_Clear(EventMask_t eventMask)
 * @brief Clears one or more events without reading them.
 * @details Clears the requested bits inside a GIE critical section; other bits
 *          are left unchanged.
 * @param[in] eventMask Bitwise OR of EVENT_* values to clear.
 * @note  Safe to call from an ISR or from a task.
 */
void Event_Clear(EventMask_t eventMask);

/**
 * @fn bool Event_IsSet(EventMask_t eventMask)
 * @brief Tests whether any of the requested events is pending.
 * @details Reads the bitmask inside a GIE critical section (a 16-bit read is
 *          not atomic on AVR) and applies any-bit semantics on
 *          eventMask limited to EVENT_ALL_MASK.
 * @param[in] eventMask Bitwise OR of EVENT_* values to test.
 * @return true when at least one requested valid bit is set, false otherwise
 *         (including when eventMask holds no valid bit).
 * @note  The events are not cleared; use Event_Consume() to read and clear.
 */
bool Event_IsSet(EventMask_t eventMask);

/**
 * @fn EventMask_t Event_GetPending(void)
 * @brief Returns a snapshot of every pending event.
 * @details Reads the bitmask inside a GIE critical section.
 * @return Current system event bitmask (EVENT_NONE when nothing is pending).
 * @note  The events are not cleared.
 */
EventMask_t Event_GetPending(void);

/**
 * @fn EventMask_t Event_Consume(EventMask_t eventMask)
 * @brief Reads and clears the requested events in one atomic step.
 * @details Inside a GIE critical section: matched = flags & eventMask, the
 *          matched bits are cleared, and matched is returned. Events not in
 *          eventMask are left unchanged, and an event posted by an ISR
 *          between two consumes is never lost.
 * @param[in] eventMask Bitwise OR of EVENT_* values to consume.
 * @return Subset of eventMask that was pending (EVENT_NONE when none was).
 * @note  This is the intended access for the FSM core and the fault manager.
 */
EventMask_t Event_Consume(EventMask_t eventMask);

/**
 * @fn void Event_ClearAll(void)
 * @brief Clears every pending event.
 * @details Writes EVENT_NONE to the system event bitmask inside a critical section.
 * @warning Events posted by an ISR before this call are discarded.
 */
void Event_ClearAll(void);

/**
 * @fn bool Event_AnyPending(void)
 * @brief Tests whether at least one event is pending.
 * @details Reads the bitmask inside a GIE critical section.
 * @return true when the bitmask is not EVENT_NONE, false otherwise.
 */
bool Event_AnyPending(void);

#endif /* EVENTFLAGS_INTERFACE_H */
