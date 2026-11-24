/**
 * @file
 *     GIE_Interface.h
 * @brief
 *     GIE (Global Interrupt Enable) driver interface APIs.
 * @details
 *     Controls the I-bit of SREG. Every interrupt source (EXTI, timers, ...)
 *     also needs its own local enable bit; this is the master switch.
 *     Also provides the save-and-disable / restore pair used to build
 *     nesting-safe critical sections in the upper layers (OS, HAL, APP).
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

#ifndef GIE_INTERFACE_H
#define GIE_INTERFACE_H

#include <stdint.h>
#include "GIE_Private.h"

/**
 * @fn void GIE_Enable(void)
 * @brief Sets the I-bit of SREG (global interrupts enabled).
 * @note  Call once in main() after all drivers are initialized.
 */
void GIE_Enable(void);

/**
 * @fn void GIE_Disable(void)
 * @brief Clears the I-bit of SREG (all interrupts blocked).
 */
void GIE_Disable(void);

/**
 * @fn uint8_t GIE_SaveAndDisable(void)
 * @brief Saves the current SREG and clears the I-bit (enters a critical section).
 * @details Reads SREG into a snapshot, then writes SREG back with the I-bit
 *          cleared. The snapshot records whether interrupts were enabled at
 *          entry, so the matching GIE_Restore() call puts the I-bit back
 *          exactly as it was. The pair is nesting-safe: an inner call sees
 *          I = 0 and its restore keeps I = 0; only the outermost restore
 *          re-enables interrupts.
 * @return Full SREG snapshot (all eight status bits) taken before the I-bit
 *         was cleared. It is opaque to the caller: store it in a local
 *         variable and pass it unmodified to GIE_Restore().
 * @warning - Keep the critical section as short as possible (a few
 *            instructions): every interrupt source, including the system
 *            tick, is blocked until GIE_Restore() is called.
 *          - Save and restore pairs must be strictly nested (last saved,
 *            first restored). Overlapping or crossed pairs leave the I-bit
 *            in an undefined state.
 * @note  - Safe to call from an ISR: the snapshot then has I = 0, so the
 *          matching GIE_Restore() never enables interrupts inside the ISR.
 *        - Every call must be paired with exactly one GIE_Restore() on every
 *          exit path of the protected block.
 */
uint8_t GIE_SaveAndDisable(void);

/**
 * @fn void GIE_Restore(uint8_t State)
 * @brief Writes a saved SREG snapshot back (leaves a critical section).
 * @details Performs a single write of State to SREG, which restores the I-bit
 *          (and the other status flags) to the value captured by
 *          GIE_SaveAndDisable(). It does not force interrupts on: if they were
 *          disabled at the matching save, they stay disabled.
 * @param State Full SREG snapshot returned by the matching GIE_SaveAndDisable(),
 *              passed unmodified.
 * @warning - Passing a value that was not produced by GIE_SaveAndDisable(),
 *            or one that was modified, corrupts the CPU status flags.
 *          - Pairs must be strictly nested: restore the most recent
 *            snapshot first.
 * @note  Calling from an ISR with the snapshot taken inside that ISR never
 *        enables interrupts, so nested ISR execution cannot be triggered.
 */
void GIE_Restore(uint8_t State);

#endif /* GIE_INTERFACE_H */
