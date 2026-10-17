/**
 * @file  main.c
 * @brief EXTI driver bring-up test.
 * @details
 *   Tests INT4 (Ignition) and INT5 (Fault Reset) with both callback
 *  and flag polling paths. The test is run on the ATmega128 target.
 * Hardware (temporary test wiring):
 *   PE4 (INT4) : Ignition push-button  -> GND (internal pull-up)
 *   PE5 (INT5) : Fault Reset button    -> GND (internal pull-up)
 *   PA0        : LED toggled from the INT4 ISR callback   (tests callback path)
 *   PA1        : LED toggled in main loop on INT5 flag    (tests flag polling path)
 *
 * Expected: each INT4 press toggles PA0, each INT5 press toggles PA1.
 */
#include <avr/interrupt.h>
#include "Common/Config.h"
#include "Common/MacroFunction.h"
#include "Mcal/Atmega128Register.h"
#include "Mcal/Exti/EXTI_Interface.h"

#define TEST_LED_CALLBACK_BIT   0U   /* PA0 */
#define TEST_LED_FLAG_BIT       1U   /* PA1 */
#define TEST_BTN_INT4_BIT       4U   /* PE4 */
#define TEST_BTN_INT5_BIT       5U   /* PE5 */

/** Runs in ISR context: keep it short. */
static void Test_Int4Callback(void)
{
    ToggleBit(PORTA_Reg, TEST_LED_CALLBACK_BIT);
}

int main(void)
{
    /* LEDs: outputs, initially off */
    SetBit(DDRA_Reg, TEST_LED_CALLBACK_BIT);
    SetBit(DDRA_Reg, TEST_LED_FLAG_BIT);
    ClearBit(PORTA_Reg, TEST_LED_CALLBACK_BIT);
    ClearBit(PORTA_Reg, TEST_LED_FLAG_BIT);

    /* Buttons: inputs with pull-up (DIO driver not available yet) */
    ClearBit(DDRE_Reg, TEST_BTN_INT4_BIT);
    ClearBit(DDRE_Reg, TEST_BTN_INT5_BIT);
    SetBit(PORTE_Reg, TEST_BTN_INT4_BIT);
    SetBit(PORTE_Reg, TEST_BTN_INT5_BIT);

    mEXTI_Init();
    mEXTI_SetCallback((uint8_t)EXTI_Line_INT4, Test_Int4Callback);

    sei();

    while (1)
    {
        if (mEXTI_GetResetFlag())
        {
            mEXTI_ClearResetFlag();
            ToggleBit(PORTA_Reg, TEST_LED_FLAG_BIT);
        }
    }
    return 0;
}