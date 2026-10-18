/**
 * @file
 *      EXTI_Program.c
 * @brief
 *      Implementation of the External Interrupt (EXTI) MCAL driver.
 * @details
 *    Contains the EXTI APIs and the INT4..INT7 interrupt service routines.
 * @version
 *      1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *     Reviewer:  Beshoy Esmat
 * @date
 *      2026-10-17
 * @copyright
 *      Copyright (c) 2026 Gestell. All rights reserved.
 */
#include "../../Common/Config.h"
#include "../../Common/MacroFunction.h"
#include "EXTI_Interface.h"
#include "EXTI_Private.h"
#include "../Atmega128Register.h"
#include "../Gie/GIE_Interface.h"

#if ModuleExtiEnabled == Enable

/**
 * @brief Event flags latched by the ISRs, cleared by the OS scheduler.
 */
static volatile bool g_ignitionEventPending = false;
static volatile bool g_resetEventPending    = false;

#if EXTI_CALLBACK_ENABLE == Enable
/**
 * @brief Registered ISR callbacks, indexed by EXTI_Index(Line).
 */
static volatile EXTI_Callback_t g_extiCallbacks[EXTI_LineCount];
#endif


/**
 * @brief Initializes INT4..INT7 with the configured trigger modes.
 */
void mEXTI_Init(void)
{
    /* Reset software state before any line is armed */
    g_ignitionEventPending = false;
    g_resetEventPending    = false;

#if EXTI_CALLBACK_ENABLE == Enable
    g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT4)] = Null;
    g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT5)] = Null;
    g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT6)] = Null;
    g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT7)] = Null;
#endif

    /* Configure trigger mode in EICRB */
    mEXTI_SetSenseControl((uint8_t)EXTI_Line_INT4, (uint8_t)EXTI_INT4_SENSE);
    mEXTI_SetSenseControl((uint8_t)EXTI_Line_INT5, (uint8_t)EXTI_INT5_SENSE);
    mEXTI_SetSenseControl((uint8_t)EXTI_Line_INT6, (uint8_t)EXTI_INT6_SENSE);
    mEXTI_SetSenseControl((uint8_t)EXTI_Line_INT7, (uint8_t)EXTI_INT7_SENSE);

    /* Flush stale flags (write 1 to clear) before unmasking */
    EIFR_Reg = (uint8_t)((1U << INTF4_Bit) | (1U << INTF5_Bit) |
                         (1U << INTF6_Bit) | (1U << INTF7_Bit));

    /* Unmask INT4..INT7 */
    SetBit(EIMSK_Reg, INT4_Bit);
    SetBit(EIMSK_Reg, INT5_Bit);
    SetBit(EIMSK_Reg, INT6_Bit);
    SetBit(EIMSK_Reg, INT7_Bit);
}

/**
 * @brief Enables the interrupt request of a line.
 * @param Line Interrupt line (EXTI_Line_INT4 .. EXTI_Line_INT7).
 */
void mEXTI_EnableLine(uint8_t Line)
{
    if ((Line >= (uint8_t)EXTI_Line_INT4) &&
        (Line <= (uint8_t)EXTI_Line_INT7))
    {
        EIFR_Reg = (uint8_t)(1U << Line);   /* Write 1 to clear this flag only */
        SetBit(EIMSK_Reg, Line);
    }
}

/**
 * @brief Disables the interrupt request of a line.
 * @param Line Interrupt line (EXTI_Line_INT4 .. EXTI_Line_INT7).
 */
void mEXTI_DisableLine(uint8_t Line)
{
    if ((Line >= (uint8_t)EXTI_Line_INT4) &&
        (Line <= (uint8_t)EXTI_Line_INT7))
    {
        ClearBit(EIMSK_Reg, Line);
    }
}

/**
 * @brief Configures the trigger mode of a line in EICRB.
 * @param Line      Interrupt line (EXTI_Line_INT4 .. EXTI_Line_INT7).
 * @param SenseMode Trigger mode (EXTI_SenseControl_t).
 */
void mEXTI_SetSenseControl(uint8_t Line, uint8_t SenseMode)
{
    bool isLineValid = ((Line >= (uint8_t)EXTI_Line_INT4) &&
                        (Line <= (uint8_t)EXTI_Line_INT7));
    bool isSenseValid = ((SenseMode == (uint8_t)EXTI_Sense_LowLevel) ||
                         (SenseMode == (uint8_t)EXTI_Sense_FallingEdge) ||
                         (SenseMode == (uint8_t)EXTI_Sense_RisingEdge));

    if (isLineValid && isSenseValid)
    {
        uint8_t shift = ISC40_Bit;
        if (Line == (uint8_t)EXTI_Line_INT5)
        {
            shift = ISC50_Bit;
        }
        else if (Line == (uint8_t)EXTI_Line_INT6)
        {
            shift = ISC60_Bit;
        }
        else if (Line == (uint8_t)EXTI_Line_INT7)
        {
            shift = ISC70_Bit;
        }
        else
        {
            /* INT4: shift remains ISC40_Bit */
        }

        /* Mask the line while its sense bits change (datasheet rule) */
        uint8_t wasEnabled = (uint8_t)GetBit(EIMSK_Reg, Line);
        ClearBit(EIMSK_Reg, Line);

        /* Read-modify-write: the other lines are preserved */
        uint8_t temp = EICRB_Reg;
        temp &= (uint8_t)~(EICRB_SenseMask << shift);
        temp |= (uint8_t)(SenseMode << shift);
        EICRB_Reg = temp;

        /* Clear a flag the change itself may have set */
        EIFR_Reg = (uint8_t)(1U << Line);

        /* Restore the previous mask state */
        if (wasEnabled != 0U)
        {
            SetBit(EIMSK_Reg, Line);
        }
    }
}

/**
 * @brief Clears the hardware interrupt flag of a line in EIFR.
 * @param Line Interrupt line (EXTI_Line_INT4 .. EXTI_Line_INT7).
 */
void mEXTI_ClearPendingFlag(uint8_t Line)
{
    if ((Line >= (uint8_t)EXTI_Line_INT4) &&
        (Line <= (uint8_t)EXTI_Line_INT7))
    {
        EIFR_Reg = (uint8_t)(1U << Line);   /* '=' on purpose: '|=' clears all flags */
    }
}

/**
 * @brief Reads the Ignition event flag latched by the Ignition ISR.
 * @return true if an event is pending, false otherwise.
 */
bool mEXTI_GetIgnitionFlag(void)
{
    return g_ignitionEventPending;
}

/**
 * @brief Clears the Ignition event flag.
 */
void mEXTI_ClearIgnitionFlag(void)
{
    g_ignitionEventPending = false;
}

/**
 * @brief Reads the Fault Reset event flag latched by the Reset ISR.
 * @return true if an event is pending, false otherwise.
 */
bool mEXTI_GetResetFlag(void)
{
    return g_resetEventPending;
}

/**
 * @brief Clears the Fault Reset event flag.
 */
void mEXTI_ClearResetFlag(void)
{
    g_resetEventPending = false;
}

/**
 * @brief Registers a callback executed inside the ISR of a line.
 * @param Line        Interrupt line (EXTI_Line_INT4 .. EXTI_Line_INT7).
 * @param CallbackPtr Pointer to a void(void) function, or Null to remove it.
 */
void mEXTI_SetCallback(uint8_t Line, EXTI_Callback_t CallbackPtr)
{
#if EXTI_CALLBACK_ENABLE == Enable
    if ((Line >= (uint8_t)EXTI_Line_INT4) &&
        (Line <= (uint8_t)EXTI_Line_INT7))
    {
        /* 16-bit pointer write must not be interrupted by the ISR */
        uint8_t sreg = SREG_Reg;
        GIE_Disable();
        g_extiCallbacks[EXTI_Index(Line)] = CallbackPtr;
        SREG_Reg = sreg;
    }
#else
    (void)Line;
    (void)CallbackPtr;
#endif
}


/**
 * @brief Interrupt Service Routine for INT4 (PE4).
 *        Latches the Ignition event flag and calls the registered callback.
 */
EXTI_ISR(INT4_vector)
{
    g_ignitionEventPending = true;

#if EXTI_CALLBACK_ENABLE == Enable
    EXTI_Callback_t callback = g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT4)];
    if (callback != Null)
    {
        callback();
    }
#endif
}

/**
 * @brief Interrupt Service Routine for INT5 (PE5).
 *        Latches the Fault Reset event flag and calls the registered callback.
 */
EXTI_ISR(INT5_vector)
{
    g_resetEventPending = true;

#if EXTI_CALLBACK_ENABLE == Enable
    EXTI_Callback_t callback = g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT5)];
    if (callback != Null)
    {
        callback();
    }
#endif
}

/**
 * @brief Interrupt Service Routine for INT6 (PE6: Ignition Push-Button on Proteus).
 *        Latches the Ignition event flag and calls the registered callback.
 */
EXTI_ISR(INT6_vector)
{
    g_ignitionEventPending = true;

#if EXTI_CALLBACK_ENABLE == Enable
    EXTI_Callback_t callback = g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT6)];
    if (callback != Null)
    {
        callback();
    }
#endif
}

/**
 * @brief Interrupt Service Routine for INT7 (PE7).
 *        Latches the Fault Reset event flag and calls the registered callback.
 */
EXTI_ISR(INT7_vector)
{
    g_resetEventPending = true;

#if EXTI_CALLBACK_ENABLE == Enable
    EXTI_Callback_t callback = g_extiCallbacks[EXTI_Index((uint8_t)EXTI_Line_INT7)];
    if (callback != Null)
    {
        callback();
    }
#endif
}


#endif