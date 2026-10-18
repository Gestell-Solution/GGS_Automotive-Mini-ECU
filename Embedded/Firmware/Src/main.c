/**
 * @file  main.c
 * @brief Complete EXTI driver verification on Proteus (AutoMotive_MiniECU.pdsprj).
 * @details
 *   Tests all 4 EICRB lines (INT4..INT7), all 4 Proteus LEDs (D1..D4),
 *   and every public API in EXTI_Interface.h:
 *
 *   Proteus LED Mapping:
 *   - D1 (Power LED,         PD4) : Toggled inside Ignition ISR Callback (INT4 or INT6)
 *   - D2 (Engine Status LED, PD5) : Toggled in main loop when mEXTI_GetIgnitionFlag() is true
 *   - D3 (Warning LED,       PD6) : Toggled inside Reset ISR Callback (INT5 or INT7)
 *   - D4 (Fault LED,         PB3) : Toggled in main loop when mEXTI_GetResetFlag() is true
 *
 *   Proteus Button / Input Mapping:
 *   - PE6 (INT6, Pin 8) : Wired to "IGNITION BUTTON" -> Press toggles D1 + D2
 *   - PE4 (INT4, Pin 6) : US-004 Ignition pin        -> Falling edge toggles D1 + D2
 *   - PE5 (INT5, Pin 7) : US-004 Fault Reset pin     -> Falling edge toggles D3 + D4
 *   - PE7 (INT7, Pin 9) : Spare EICRB line           -> Falling edge toggles D3 + D4
 */
#include "Common/Config.h"
#include "Common/MacroFunction.h"
#include "Mcal/Atmega128Register.h"
#include "Mcal/Exti/EXTI_Interface.h"
#include "Mcal/Gie/GIE_Interface.h"

/* Proteus LEDs: D1..D3 on PORTD, D4 on PORTB */
#define TEST_LED_D1_POWER_BIT    4U   /* PD4 (Pin 29): D1 Power LED         */
#define TEST_LED_D2_STATUS_BIT   5U   /* PD5 (Pin 30): D2 Engine Status LED */
#define TEST_LED_D3_WARNING_BIT  6U   /* PD6 (Pin 31): D3 Warning LED       */
#define TEST_LED_D4_FAULT_BIT    3U   /* PB3 (Pin 13): D4 Fault LED         */

/* EXTI pins on PORTE (PE4..PE7) */
#define TEST_EXTI_INT4_BIT       4U   /* PE4 (Pin 6: INT4) */
#define TEST_EXTI_INT5_BIT       5U   /* PE5 (Pin 7: INT5) */
#define TEST_EXTI_INT6_BIT       6U   /* PE6 (Pin 8: INT6 - IGNITION BUTTON) */
#define TEST_EXTI_INT7_BIT       7U   /* PE7 (Pin 9: INT7) */

/**
 * @brief ISR Callback for Ignition lines (INT4 / INT6).
 *        Toggles D1 (Power LED on PD4) immediately in ISR context.
 */
static void Test_IgnitionIsrCallback(void)
{
    ToggleBit(PORTD_Reg, TEST_LED_D1_POWER_BIT);
}

/**
 * @brief ISR Callback for Fault Reset lines (INT5 / INT7).
 *        Toggles D3 (Warning LED on PD6) immediately in ISR context.
 */
static void Test_ResetIsrCallback(void)
{
    ToggleBit(PORTD_Reg, TEST_LED_D3_WARNING_BIT);
}

int main(void)
{
    /* 1. Configure all 4 Proteus LEDs (D1..D4) as outputs, initially OFF */
    SetBit(DDRD_Reg, TEST_LED_D1_POWER_BIT);
    SetBit(DDRD_Reg, TEST_LED_D2_STATUS_BIT);
    SetBit(DDRD_Reg, TEST_LED_D3_WARNING_BIT);
    SetBit(DDRB_Reg, TEST_LED_D4_FAULT_BIT);

    ClearBit(PORTD_Reg, TEST_LED_D1_POWER_BIT);
    ClearBit(PORTD_Reg, TEST_LED_D2_STATUS_BIT);
    ClearBit(PORTD_Reg, TEST_LED_D3_WARNING_BIT);
    ClearBit(PORTB_Reg, TEST_LED_D4_FAULT_BIT);

    /* 2. Configure PE4..PE7 as inputs with internal pull-up resistors (idle HIGH = 5V) */
    ClearBit(DDRE_Reg, TEST_EXTI_INT4_BIT);
    ClearBit(DDRE_Reg, TEST_EXTI_INT5_BIT);
    ClearBit(DDRE_Reg, TEST_EXTI_INT6_BIT);
    ClearBit(DDRE_Reg, TEST_EXTI_INT7_BIT);

    SetBit(PORTE_Reg, TEST_EXTI_INT4_BIT);
    SetBit(PORTE_Reg, TEST_EXTI_INT5_BIT);
    SetBit(PORTE_Reg, TEST_EXTI_INT6_BIT);
    SetBit(PORTE_Reg, TEST_EXTI_INT7_BIT);

    /* 3. Initialize EXTI driver (sets EICRB=0xAA Falling Edge, clears EIFR, sets EIMSK=0xF0) */
    mEXTI_Init();

    /* 4. Self-test of runtime APIs (DisableLine -> SetSenseControl -> ClearPendingFlag -> EnableLine) */
    mEXTI_DisableLine((uint8_t)EXTI_Line_INT6);
    mEXTI_SetSenseControl((uint8_t)EXTI_Line_INT6, (uint8_t)EXTI_Sense_FallingEdge);
    mEXTI_ClearPendingFlag((uint8_t)EXTI_Line_INT6);
    mEXTI_EnableLine((uint8_t)EXTI_Line_INT6);

    /* 5. Register ISR callbacks for both Ignition (INT4, INT6) and Reset (INT5, INT7) */
    mEXTI_SetCallback((uint8_t)EXTI_Line_INT4, Test_IgnitionIsrCallback);
    mEXTI_SetCallback((uint8_t)EXTI_Line_INT6, Test_IgnitionIsrCallback);
    mEXTI_SetCallback((uint8_t)EXTI_Line_INT5, Test_ResetIsrCallback);
    mEXTI_SetCallback((uint8_t)EXTI_Line_INT7, Test_ResetIsrCallback);

    /* 6. Enable Global Interrupts (SREG I-bit = 1) */
    GIE_Enable();

    while (1)
    {
        /* Poll & clear Ignition Flag (latched by INT4 or INT6 ISR) -> Toggles D2 (Engine Status LED) */
        if (mEXTI_GetIgnitionFlag())
        {
            mEXTI_ClearIgnitionFlag();
            ToggleBit(PORTD_Reg, TEST_LED_D2_STATUS_BIT);
        }

        /* Poll & clear Fault Reset Flag (latched by INT5 or INT7 ISR) -> Toggles D4 (Fault LED) */
        if (mEXTI_GetResetFlag())
        {
            mEXTI_ClearResetFlag();
            ToggleBit(PORTB_Reg, TEST_LED_D4_FAULT_BIT);
        }
    }
    return 0;
}