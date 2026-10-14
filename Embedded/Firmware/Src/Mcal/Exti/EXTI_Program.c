/**
 * @file
 *      EXTI_Program.c
 * @brief
 *      Implementation of the External Interrupt (EXTI) MCAL driver.
 * @details
 *      INT4 (Ignition) and INT5 (Fault Reset) are edge-triggered. Each ISR
 *      sets a volatile flag, optionally calls a callback and returns. No
 *      debounce, delay or state machine logic exists in this driver.
 * @version
 *      1.0.0
 * @author
 *      Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *      2026-10-14
 * @copyright
 *      Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "Definition.h"
#include "MacroFunction.h"
#include "Atmega128Register.h"
#include "EXTI_Config.h"
#include "EXTI_Private.h"
#include "EXTI_Interface.h"

/** @brief Ignition button event flag, set by the INT4 ISR. */
static volatile bool g_ignitionEventPending = false;

/** @brief Fault Reset button event flag, set by the INT5 ISR. */
static volatile bool g_resetEventPending = false;

#if (ExtiCallbackEnabled == Enable)
/** @brief Registered ISR callbacks, indexed by ExtiLineIndex(Line). */
static volatile EXTI_Callback_t g_extiCallbacks[ExtiLineCount];
#endif

/*API's*/
void mEXTI_Init(void)
{
#if (ExtiCallbackEnabled == Enable)
    uint8_t Local_u8Index;
#endif

    /* 1- Reset software state before any line is armed */
    g_ignitionEventPending = false;
    g_resetEventPending = false;

#if (ExtiCallbackEnabled == Enable)
    for (Local_u8Index = 0U; Local_u8Index < (uint8_t)ExtiLineCount; Local_u8Index++)
    {
        g_extiCallbacks[Local_u8Index] = Null;
    }
#endif

    /* 2- Configure the trigger mode of each line in EICRB */
    mEXTI_SetSenseControl((uint8_t)EXTI_Line_INT4, (uint8_t)ExtiIgnitionSenseMode);
    mEXTI_SetSenseControl((uint8_t)EXTI_Line_INT5, (uint8_t)ExtiResetSenseMode);

    /* 3- Flush stale flags (write 1 to clear) BEFORE unmasking */
    EIFR_Reg = (uint8_t)((1U << (uint8_t)EIFR_INTF4) | (1U << (uint8_t)EIFR_INTF5));

    /* 4- Unmask the lines selected in EXTI_Config.h */
#if (ExtiIgnitionEnableOnInit == Enable)
    SetBit(EIMSK_Reg, (uint8_t)EIMSK_INT4);
#endif
#if (ExtiResetEnableOnInit == Enable)
    SetBit(EIMSK_Reg, (uint8_t)EIMSK_INT5);
#endif
}

void mEXTI_EnableLine(uint8_t Line)
{
    if (ExtiIsValidLine(Line))
    {
        /* 1- Flush the stale flag first (write 1 to clear) */
        EIFR_Reg = (uint8_t)(1U << Line);
        /* 2- Unmask the line */
        SetBit(EIMSK_Reg, Line);
    }
    else
    {
        /* Invalid line: no action */
    }
}

void mEXTI_DisableLine(uint8_t Line)
{
    if (ExtiIsValidLine(Line))
    {
        ClearBit(EIMSK_Reg, Line);
    }
    else
    {
        /* Invalid line: no action */
    }
}

void mEXTI_SetSenseControl(uint8_t Line, uint8_t SenseMode)
{
    uint8_t Local_u8Shift;
    uint8_t Local_u8WasEnabled;

    /* 1- Validate the line and the sense mode */
    if (ExtiIsValidLine(Line) && ExtiIsValidSense(SenseMode))
    {
        Local_u8Shift = (uint8_t)ExtiSenseShift(Line);
        Local_u8WasEnabled = (uint8_t)GetBit(EIMSK_Reg, Line);

        /* 2- Datasheet rule: mask the line while its sense bits change */
        ClearBit(EIMSK_Reg, Line);

        /* 3- Reg = (Reg & ~Mask) | Value (the other line is preserved) */
        WriteReg(EICRB_Reg,
                 (uint8_t)ExtiSenseMask(Line),
                 (uint8_t)(SenseMode << Local_u8Shift));

        /* 4- Clear a flag that the change itself may have set */
        EIFR_Reg = (uint8_t)(1U << Line);

        /* 5- Restore the previous mask state */
        if (Local_u8WasEnabled != 0U)
        {
            SetBit(EIMSK_Reg, Line);
        }
        else
        {
            /* Line was masked before: leave it masked */
        }
    }
    else
    {
        /* Invalid line or sense mode: no action */
    }
}

void mEXTI_ClearPendingFlag(uint8_t Line)
{
    if (ExtiIsValidLine(Line))
    {
        /* '=' on purpose: writing 1 clears the flag, '|=' would clear the other flag too */
        EIFR_Reg = (uint8_t)(1U << Line);
    }
    else
    {
        /* Invalid line: no action */
    }
}

bool mEXTI_GetIgnitionFlag(void)
{
    return g_ignitionEventPending;
}

void mEXTI_ClearIgnitionFlag(void)
{
    g_ignitionEventPending = false;
}

bool mEXTI_GetResetFlag(void)
{
    return g_resetEventPending;
}

void mEXTI_ClearResetFlag(void)
{
    g_resetEventPending = false;
}

/*Call-Back-Function*/
void mEXTI_SetCallback(uint8_t Line, EXTI_Callback_t CallbackPtr)
{
#if (ExtiCallbackEnabled == Enable)
    uint8_t Local_u8WasEnabled;

    if (ExtiIsValidLine(Line))
    {
        /* 1- Mask the line: the 16-bit pointer write must not be interrupted */
        Local_u8WasEnabled = (uint8_t)GetBit(EIMSK_Reg, Line);
        ClearBit(EIMSK_Reg, Line);

        /* 2- Store the callback (Null removes it) */
        g_extiCallbacks[ExtiLineIndex(Line)] = CallbackPtr;

        /* 3- Restore the previous mask state */
        if (Local_u8WasEnabled != 0U)
        {
            SetBit(EIMSK_Reg, Line);
        }
        else
        {
            /* Line was masked before: leave it masked */
        }
    }
    else
    {
        /* Invalid line: no action */
    }
#else
    (void)Line;
    (void)CallbackPtr;
#endif
}

/*ISR*/

/*INT4: Ignition button*/
void __vector_5(void)
{
    /* 1- Latch the event */
    g_ignitionEventPending = true;

#if (ExtiCallbackEnabled == Enable)
    /* 2- Call the registered callback, if any */
    {
        EXTI_Callback_t Local_pvCallback = g_extiCallbacks[ExtiIndexInt4];

        if (Local_pvCallback != Null)
        {
            Local_pvCallback();
        }
        else
        {
            /* No callback registered: nothing to do */
        }
    }
#endif
}

/*INT5: Fault Reset button*/
void __vector_6(void)
{
    /* 1- Latch the event */
    g_resetEventPending = true;

#if (ExtiCallbackEnabled == Enable)
    /* 2- Call the registered callback, if any */
    {
        EXTI_Callback_t Local_pvCallback = g_extiCallbacks[ExtiIndexInt5];

        if (Local_pvCallback != Null)
        {
            Local_pvCallback();
        }
        else
        {
            /* No callback registered: nothing to do */
        }
    }
#endif
}