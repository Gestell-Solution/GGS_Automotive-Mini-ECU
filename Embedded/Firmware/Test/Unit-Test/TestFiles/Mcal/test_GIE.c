/**
 * @file
 *     test_GIE.c
 * @brief
 *     Host unit tests for the MCAL GIE driver (SREG I-bit control).
 * @details
 *     Unit under test: GIE_Program.c (GIE_Enable, GIE_Disable,
 *     GIE_SaveAndDisable, GIE_Restore). Specification: UT-MCAL-007.
 *     SREG_Reg is redirected to g_fakeRegisterBank[0x5F] (RegisterMock.h).
 *     Until Atmega128Register.h has its own host-test (#ifdef TEST) branch,
 *     the test build must pass "-include RegisterSeam.h" so that
 *     GIE_Program.c is redirected too.
 *     No test doubles: GIE has no lower-layer dependency.
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *     2026-11-26
 * @copyright
 *     Copyright (c) 2026 Gestell. All rights reserved.
 */

/* ============================== Test List ============================= */
/*  SREG bits: I(7) T(6) H(5) S(4) V(3) N(2) Z(1) C(0)
 *
 *  GIE_Enable
 *  [x] TC-GIE-001  I clear: sets bit 7 only, no other register touched   (AC-2)
 *  [x] TC-GIE-002  Flags 0x7F set: keeps all seven status flags          (AC-3)
 *  [x] TC-GIE-003  Second consecutive call keeps I set                   (AC-4)
 *  [x] TC-GIE-009  I already set: SREG unchanged                         (AC-6)
 *  GIE_Disable
 *  [x] TC-GIE-004  I set: clears bit 7 only, no other register touched   (AC-2)
 *  [x] TC-GIE-005  All bits set: keeps all seven status flags            (AC-3)
 *  [x] TC-GIE-006  Second consecutive call keeps I clear                 (AC-4)
 *  [x] TC-GIE-008  I already clear: SREG unchanged                       (AC-6)
 *  GIE_SaveAndDisable
 *  [x] TC-GIE-010  Returns the full SREG snapshot taken before the clear (AC-8)
 *  [x] TC-GIE-011  Clears the I-bit                                      (AC-2, AC-8)
 *  [x] TC-GIE-012  Keeps all seven status flags                          (AC-3, AC-8)
 *  [x] TC-GIE-013  I already clear: snapshot has I = 0                   (AC-5, AC-8)
 *  [x] TC-GIE-014  I already clear: SREG unchanged                       (AC-6, AC-8)
 *  GIE_Restore
 *  [x] TC-GIE-015  Writes the snapshot exactly, no merge with SREG       (AC-8)
 *  [x] TC-GIE-016  Snapshot with I = 0 does not enable interrupts        (AC-8)
 *  [x] TC-GIE-007  Save/Restore pair ends with the original SREG         (AC-5)
 *  [x] TC-GIE-017  Inner restore of a nested section keeps I = 0         (AC-5, AC-8)
 *  [x] TC-GIE-018  Outer restore of a nested section re-enables I        (AC-5, AC-8)
 *
 *  AC-1 (all tests pass) and AC-7 (strict AAA) apply to every test.
 *  AC-8 (added to UT-MCAL-007): every public API has at least one test
 *  and GIE_Program.c is fully covered (all functions and lines).
 */

/* ============================== Includes ============================== */
#include "unity.h"
#include "RegisterMock.h"
#include "Atmega128Register.h"   /* SREG_Reg, redirected into the fake bank */
#include "GIE_Interface.h"

/* ========================== ISR Entry Points ========================== */
/* None: the GIE driver defines no interrupt service routine. */

/* =========================== Test Doubles ============================= */
/* None: GIE_Program.c has no lower-layer dependency to replace. */

/* ========================= Fixture: setUp/tearDown ==================== */
void setUp(void)
{
    RegMock_Reset();          /* Every register = 0x00 (SREG reset value) */
}

void tearDown(void)
{
}

/* ============================= GIE_Enable ============================= */
/* TC-GIE-001 | AC-2 */
void test_GIE_Enable_InterruptsDisabled_SetsIBit(void)
{
    /* Arrange */
    SREG_Reg = 0x00U;

    /* Act */
    GIE_Enable();

    /* Assert: SREG bit 7 set, every other register byte still 0x00 */
    TEST_ASSERT_EQUAL_HEX8(0x80U, SREG_Reg);
    TEST_ASSERT_EACH_EQUAL_HEX8(0x00U, &g_fakeRegisterBank[0x00U], 0x5FU);   /* 0x00..0x5E */
    TEST_ASSERT_EACH_EQUAL_HEX8(0x00U, &g_fakeRegisterBank[0x60U], 0xA0U);   /* 0x60..0xFF */
}

/* TC-GIE-002 | AC-3 */
void test_GIE_Enable_StatusFlagsSet_PreservesOtherSregBits(void)
{
    /* Arrange: T H S V N Z C all set, I clear */
    SREG_Reg = 0x7FU;

    /* Act */
    GIE_Enable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0xFFU, SREG_Reg);
}

/* TC-GIE-003 | AC-4 | Mirrors TC-GIE-009; the pre-state is reached through the API */
void test_GIE_Enable_RepeatedCall_KeepsIBitSet(void)
{
    /* Arrange: first call, SREG 0x00 -> 0x80 */
    SREG_Reg = 0x00U;
    GIE_Enable();

    /* Act: second call */
    GIE_Enable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x80U, SREG_Reg);
}

/* TC-GIE-009 | AC-6 */
void test_GIE_Enable_InterruptsAlreadyEnabled_LeavesSregUnchanged(void)
{
    /* Arrange */
    SREG_Reg = 0x80U;

    /* Act */
    GIE_Enable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x80U, SREG_Reg);
}

/* ============================= GIE_Disable ============================ */
/* TC-GIE-004 | AC-2 */
void test_GIE_Disable_InterruptsEnabled_ClearsIBit(void)
{
    /* Arrange */
    SREG_Reg = 0x80U;

    /* Act */
    GIE_Disable();

    /* Assert: SREG bit 7 clear, every other register byte still 0x00 */
    TEST_ASSERT_EQUAL_HEX8(0x00U, SREG_Reg);
    TEST_ASSERT_EACH_EQUAL_HEX8(0x00U, &g_fakeRegisterBank[0x00U], 0x5FU);   /* 0x00..0x5E */
    TEST_ASSERT_EACH_EQUAL_HEX8(0x00U, &g_fakeRegisterBank[0x60U], 0xA0U);   /* 0x60..0xFF */
}

/* TC-GIE-005 | AC-3 */
void test_GIE_Disable_AllBitsSet_PreservesOtherSregBits(void)
{
    /* Arrange */
    SREG_Reg = 0xFFU;

    /* Act */
    GIE_Disable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x7FU, SREG_Reg);
}

/* TC-GIE-006 | AC-4 | Mirrors TC-GIE-008; the pre-state is reached through the API */
void test_GIE_Disable_RepeatedCall_KeepsIBitClear(void)
{
    /* Arrange: first call, SREG 0x80 -> 0x00 */
    SREG_Reg = 0x80U;
    GIE_Disable();

    /* Act: second call */
    GIE_Disable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x00U, SREG_Reg);
}

/* TC-GIE-008 | AC-6 */
void test_GIE_Disable_InterruptsAlreadyDisabled_LeavesSregUnchanged(void)
{
    /* Arrange */
    SREG_Reg = 0x00U;

    /* Act */
    GIE_Disable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x00U, SREG_Reg);
}

/* ========================= GIE_SaveAndDisable ========================= */
/* TC-GIE-010 | AC-8 */
void test_GIE_SaveAndDisable_InterruptsEnabled_ReturnsFullSregSnapshot(void)
{
    /* Arrange: I H N C set (1010 0101b) */
    uint8_t snapshot = 0x00U;
    SREG_Reg = 0xA5U;

    /* Act */
    snapshot = GIE_SaveAndDisable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0xA5U, snapshot);
}

/* TC-GIE-011 | AC-2, AC-8 */
void test_GIE_SaveAndDisable_InterruptsEnabled_ClearsIBit(void)
{
    /* Arrange */
    SREG_Reg = 0x80U;

    /* Act */
    (void)GIE_SaveAndDisable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x00U, SREG_Reg);
}

/* TC-GIE-012 | AC-3, AC-8 */
void test_GIE_SaveAndDisable_AllBitsSet_PreservesStatusFlags(void)
{
    /* Arrange */
    SREG_Reg = 0xFFU;

    /* Act */
    (void)GIE_SaveAndDisable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x7FU, SREG_Reg);
}

/* TC-GIE-013 | AC-5, AC-8 */
void test_GIE_SaveAndDisable_InterruptsAlreadyDisabled_ReturnsSnapshotWithIBitClear(void)
{
    /* Arrange: inside a critical section, H N C set (0010 0101b) */
    uint8_t snapshot = 0xFFU;
    SREG_Reg = 0x25U;

    /* Act */
    snapshot = GIE_SaveAndDisable();

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x25U, snapshot);
}

/* TC-GIE-014 | AC-6, AC-8 */
void test_GIE_SaveAndDisable_InterruptsAlreadyDisabled_LeavesSregUnchanged(void)
{
    /* Arrange */
    SREG_Reg = 0x25U;

    /* Act */
    (void)GIE_SaveAndDisable();

    /* Assert: I stays clear (a toggle of bit 7 would give 0xA5) */
    TEST_ASSERT_EQUAL_HEX8(0x25U, SREG_Reg);
}

/* ============================= GIE_Restore ============================ */
/* TC-GIE-015 | AC-8 */
void test_GIE_Restore_DifferentCurrentSreg_WritesExactSnapshot(void)
{
    /* Arrange: current SREG is the bitwise complement of the snapshot */
    SREG_Reg = 0x5AU;

    /* Act */
    GIE_Restore(0xA5U);

    /* Assert: plain write (an OR merge gives 0xFF, an AND merge 0x00) */
    TEST_ASSERT_EQUAL_HEX8(0xA5U, SREG_Reg);
}

/* TC-GIE-016 | AC-8 */
void test_GIE_Restore_SnapshotWithIBitClear_KeepsInterruptsDisabled(void)
{
    /* Arrange */
    SREG_Reg = 0x00U;

    /* Act */
    GIE_Restore(0x25U);

    /* Assert: I is not forced to 1 */
    TEST_ASSERT_EQUAL_HEX8(0x25U, SREG_Reg);
}

/* TC-GIE-007 | AC-5 */
void test_GIE_Restore_SnapshotFromSaveAndDisable_RestoresOriginalSreg(void)
{
    /* Arrange: I N Z C set, critical section entered */
    uint8_t snapshot = 0x00U;
    SREG_Reg = 0x87U;
    snapshot = GIE_SaveAndDisable();

    /* Act: leave the critical section */
    GIE_Restore(snapshot);

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x87U, SREG_Reg);
}

/* TC-GIE-017 | AC-5, AC-8 */
void test_GIE_Restore_InnerSnapshotOfNestedSection_KeepsInterruptsDisabled(void)
{
    /* Arrange: outer section entered (I 1 -> 0), then a nested one */
    uint8_t innerSnapshot = 0xFFU;
    SREG_Reg = 0x80U;
    (void)GIE_SaveAndDisable();
    innerSnapshot = GIE_SaveAndDisable();

    /* Act: leave the nested section */
    GIE_Restore(innerSnapshot);

    /* Assert: still inside the outer section */
    TEST_ASSERT_EQUAL_HEX8(0x00U, SREG_Reg);
}

/* TC-GIE-018 | AC-5, AC-8 */
void test_GIE_Restore_OuterSnapshotOfNestedSection_ReEnablesInterrupts(void)
{
    /* Arrange: outer and nested sections entered, nested one already left */
    uint8_t outerSnapshot = 0x00U;
    uint8_t innerSnapshot = 0xFFU;
    SREG_Reg = 0x80U;
    outerSnapshot = GIE_SaveAndDisable();
    innerSnapshot = GIE_SaveAndDisable();
    GIE_Restore(innerSnapshot);

    /* Act: leave the outer section */
    GIE_Restore(outerSnapshot);

    /* Assert */
    TEST_ASSERT_EQUAL_HEX8(0x80U, SREG_Reg);
}

/* ============================ Test Runner ============================= */
int main(void)
{
    UNITY_BEGIN();

    /* GIE_Enable */
    RUN_TEST(test_GIE_Enable_InterruptsDisabled_SetsIBit);
    RUN_TEST(test_GIE_Enable_StatusFlagsSet_PreservesOtherSregBits);
    RUN_TEST(test_GIE_Enable_RepeatedCall_KeepsIBitSet);
    RUN_TEST(test_GIE_Enable_InterruptsAlreadyEnabled_LeavesSregUnchanged);

    /* GIE_Disable */
    RUN_TEST(test_GIE_Disable_InterruptsEnabled_ClearsIBit);
    RUN_TEST(test_GIE_Disable_AllBitsSet_PreservesOtherSregBits);
    RUN_TEST(test_GIE_Disable_RepeatedCall_KeepsIBitClear);
    RUN_TEST(test_GIE_Disable_InterruptsAlreadyDisabled_LeavesSregUnchanged);

    /* GIE_SaveAndDisable */
    RUN_TEST(test_GIE_SaveAndDisable_InterruptsEnabled_ReturnsFullSregSnapshot);
    RUN_TEST(test_GIE_SaveAndDisable_InterruptsEnabled_ClearsIBit);
    RUN_TEST(test_GIE_SaveAndDisable_AllBitsSet_PreservesStatusFlags);
    RUN_TEST(test_GIE_SaveAndDisable_InterruptsAlreadyDisabled_ReturnsSnapshotWithIBitClear);
    RUN_TEST(test_GIE_SaveAndDisable_InterruptsAlreadyDisabled_LeavesSregUnchanged);

    /* GIE_Restore */
    RUN_TEST(test_GIE_Restore_DifferentCurrentSreg_WritesExactSnapshot);
    RUN_TEST(test_GIE_Restore_SnapshotWithIBitClear_KeepsInterruptsDisabled);
    RUN_TEST(test_GIE_Restore_SnapshotFromSaveAndDisable_RestoresOriginalSreg);
    RUN_TEST(test_GIE_Restore_InnerSnapshotOfNestedSection_KeepsInterruptsDisabled);
    RUN_TEST(test_GIE_Restore_OuterSnapshotOfNestedSection_ReEnablesInterrupts);

    return UNITY_END();
}
