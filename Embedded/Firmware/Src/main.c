/**
 * @file       main.c
 * @author     Developer: Beshoy Esmat
 * @author     Reviewer: Salma Mohamed
 * @brief      Timer2 SysTick 20x4 LCD Real-Time Diagnostic Dashboard
 * @details    Displays live real-time variables across 4 rows on a 20x4 HD44780 LCD:
 *             - Row 0: g_sysTick_ms      (Increments by +10 ms on every tick!)
 *             - Row 1: g_uptime_seconds  (Total whole seconds elapsed)
 *             - Row 2: g_subSecondTicks  (Current sub-second tick: 00..99)
 *             - Row 3: Reset Counter     (Visibly shows mTimer2_ResetTick() execution)
 * 
 * =============================================================================
 * HD44780 20x4 LCD WIRING TO ATmega128 (PORTC - 4-BIT MODE):
 * =============================================================================
 * - PC0 (Pin 35) -> LCD Pin 4  (RS - Register Select)
 * - PC1 (Pin 36) -> LCD Pin 6  (E  - Enable Strobe)
 * - PC4 (Pin 39) -> LCD Pin 11 (D4)
 * - PC5 (Pin 40) -> LCD Pin 12 (D5)
 * - PC6 (Pin 41) -> LCD Pin 13 (D6)
 * - PC7 (Pin 42) -> LCD Pin 14 (D7)
 * - GND          -> LCD Pin 1 (VSS), Pin 3 (V0), Pin 5 (RW)
 * - +5V          -> LCD Pin 2 (VDD)
 * 
 * =============================================================================
 * FREQUENCY & LED PINS (PORTA):
 * =============================================================================
 * - PA0 (Pin 51) -> Frequency Meter Channel 1 = EXACTLY 50.08 Hz (AC-1)
 * - PA1 (Pin 50) -> Frequency Meter Channel 2 = EXACTLY 1.000 Hz (Heartbeat)
 * 
 * @version    7.0.0
 * @date       17/10/2026
 * @copyright  Copyright (c) 2026, Gestell Company
 */

#include <stdint.h>
#include "TIMER2_interface.h"

#ifndef sei
#define sei() __asm__ __volatile__ ("sei" ::: "memory")
#endif

/* =========================================================================
 * HARDWARE PIN DEFINITIONS
 * ========================================================================= */
#define PIN_AC1_SCOPE       0U  /**< PA0 (Pin 51): 50.08 Hz Callback Square Wave */
#define PIN_LED_1SEC        1U  /**< PA1 (Pin 50): 1.000 Hz Heartbeat LED */

/* LCD Pin Mapping on PORTC */
#define LCD_PORT            PORTC_Reg
#define LCD_DDR             DDRC_Reg
#define LCD_RS_PIN          0U  /**< PC0: Register Select */
#define LCD_E_PIN           1U  /**< PC1: Enable Strobe */
#define LCD_DATA_MASK       0xF0U /**< PC4..PC7: 4-Bit Data Bus (D4..D7) */

/* Auto-reset interval (in seconds) to demonstrate mTimer2_ResetTick() */
#define RESET_INTERVAL_SEC  15UL

/* =========================================================================
 * GLOBAL DIAGNOSTIC VARIABLES
 * ========================================================================= */
static volatile uint8_t g_subSecondTicks = 0U; /**< 0..99 ticks per second */

/* =========================================================================
 * HIGH-SPEED MICROSECOND DELAY (Non-blocking to Timer2)
 * ========================================================================= */

static void LCD_DelayUs(uint16_t us)
{
    while (us--)
    {
        __asm__ __volatile__(
            "nop\n\tnop\n\tnop\n\tnop\n\t"
            "nop\n\tnop\n\tnop\n\tnop\n\t"
            "nop\n\tnop\n\tnop\n\tnop\n\t"
            "nop\n\tnop\n\tnop\n\tnop\n\t"
        );
    }
}

/* =========================================================================
 * LOW-LEVEL HD44780 20x4 LCD DRIVER (4-BIT FAST MODE)
 * ========================================================================= */

static void LCD_PulseEnable(void)
{
    LCD_PORT |= (uint8_t)(1U << LCD_E_PIN);
    LCD_DelayUs(2U);
    LCD_PORT &= (uint8_t)(~(1U << LCD_E_PIN));
    LCD_DelayUs(2U);
}

static void LCD_SendNibble(uint8_t nibble)
{
    LCD_PORT &= (uint8_t)(~LCD_DATA_MASK);
    LCD_PORT |= (uint8_t)(nibble & LCD_DATA_MASK);
    LCD_PulseEnable();
}

static void LCD_SendCommand(uint8_t cmd)
{
    LCD_PORT &= (uint8_t)(~(1U << LCD_RS_PIN)); /* RS = 0 for command */
    LCD_SendNibble(cmd & 0xF0U);
    LCD_SendNibble((uint8_t)(cmd << 4));
    
    if ((cmd == 0x01U) || (cmd == 0x02U))
    {
        mTimer2_DelayMs(2UL); /* Clear Screen and Return Home need 2ms */
    }
    else
    {
        LCD_DelayUs(50U);
    }
}

static void LCD_SendData(uint8_t data)
{
    LCD_PORT |= (uint8_t)(1U << LCD_RS_PIN);  /* RS = 1 for data */
    LCD_SendNibble(data & 0xF0U);
    LCD_SendNibble((uint8_t)(data << 4));
    LCD_DelayUs(50U);
}

/**
 * @brief  Sets cursor for HD44780 20x4 Display.
 *         Row 0: 0x80 | Row 1: 0xC0 | Row 2: 0x94 | Row 3: 0xD4
 */
static void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr;
    switch (row)
    {
        case 0U:  addr = (uint8_t)(0x80U + col); break;
        case 1U:  addr = (uint8_t)(0xC0U + col); break;
        case 2U:  addr = (uint8_t)(0x94U + col); break;
        case 3U:  addr = (uint8_t)(0xD4U + col); break;
        default:  addr = (uint8_t)(0x80U + col); break;
    }
    LCD_SendCommand(addr);
}

static void LCD_PrintString(const char *str)
{
    while (*str != '\0')
    {
        LCD_SendData((uint8_t)(*str++));
    }
}

static void LCD_Print2Digits(uint8_t val)
{
    LCD_SendData((uint8_t)('0' + ((val / 10U) % 10U)));
    LCD_SendData((uint8_t)('0' + (val % 10U)));
}

static void LCD_PrintField(uint32_t num, uint8_t width)
{
    char buf[12];
    int8_t i = 0;
    uint8_t count = 0;

    if (num == 0UL)
    {
        buf[i++] = '0';
    }
    else
    {
        while (num > 0UL)
        {
            buf[i++] = (char)('0' + (num % 10UL));
            num /= 10UL;
        }
    }
    count = (uint8_t)i;

    while (--i >= 0)
    {
        LCD_SendData((uint8_t)buf[i]);
    }

    while (count < width)
    {
        LCD_SendData((uint8_t)' ');
        count++;
    }
}

static void LCD_PrintStaticLabels(void)
{
    LCD_SetCursor(0U, 0U);
    LCD_PrintString("SysTick : ");

    LCD_SetCursor(1U, 0U);
    LCD_PrintString("Uptime  : ");

    LCD_SetCursor(2U, 0U);
    LCD_PrintString("SubTicks: ");

    LCD_SetCursor(3U, 0U);
    LCD_PrintString("ResetCnt: ");
}

static void LCD_Init(void)
{
    LCD_DDR |= (uint8_t)((1U << LCD_RS_PIN) | (1U << LCD_E_PIN) | LCD_DATA_MASK);
    LCD_PORT &= (uint8_t)(~((1U << LCD_RS_PIN) | (1U << LCD_E_PIN) | LCD_DATA_MASK));

    mTimer2_DelayMs(50UL); /* Power-on wait */

    LCD_PORT &= (uint8_t)(~(1U << LCD_RS_PIN));
    LCD_SendNibble(0x30U);
    mTimer2_DelayMs(5UL);
    LCD_SendNibble(0x30U);
    mTimer2_DelayMs(1UL);
    LCD_SendNibble(0x30U);
    mTimer2_DelayMs(1UL);
    LCD_SendNibble(0x20U); /* Switch to 4-bit */
    mTimer2_DelayMs(1UL);

    LCD_SendCommand(0x28U); /* 4-bit mode, 2/4 lines, 5x8 font */
    LCD_SendCommand(0x0CU); /* Display ON, Cursor OFF, Blink OFF */
    LCD_SendCommand(0x01U); /* Clear Screen */
    mTimer2_DelayMs(2UL);
    LCD_SendCommand(0x06U); /* Entry Mode: Increment Cursor */
}

/* =========================================================================
 * 10MS ISR CALLBACK (AC-1 & SUB-SECOND TICK TRACKER)
 * ========================================================================= */

/**
 * @brief  Invoked directly inside the Compare Match ISR on every 10ms tick.
 */
static void Timer2_Tick_Callback(void)
{
    /* Toggle PA0 -> Exact 50.08 Hz on frequency meter (AC-1) */
    PORTA_Reg ^= (uint8_t)(1U << PIN_AC1_SCOPE);

    /* Track sub-second ticks: increments 0..99 every 10ms */
    g_subSecondTicks++;
    if (g_subSecondTicks >= 100U)
    {
        g_subSecondTicks = 0U;
    }
}

/* =========================================================================
 * MAIN APPLICATION
 * ========================================================================= */

int main(void)
{
    uint32_t last_heartbeat_ms = 0UL;
    uint32_t last_lcd_update_ms = 0xFFFFFFFFUL;
    uint32_t last_displayed_sec = 0xFFFFFFFFUL;
    uint8_t  last_displayed_reset = 0xFFU;
    uint32_t current_ms;
    uint32_t total_sec;
    uint8_t  reset_count = 0U;

    /* Configure PA0 and PA1 as outputs */
    DDRA_Reg |= (uint8_t)((1U << PIN_AC1_SCOPE) | (1U << PIN_LED_1SEC));
    PORTA_Reg &= (uint8_t)(~((1U << PIN_AC1_SCOPE) | (1U << PIN_LED_1SEC)));

    /* 1. Initialize Timer2 in CTC mode (10ms SysTick engine) */
    mTimer2_Init_CTC_10ms();

    /* 2. Register callback for 10ms tick (AC-1) */
    mTimer2_SetCallback(Timer2_Tick_Callback);

    /* 3. Enable Global Interrupts */
    sei();

    /* 4. Initialize LCD Display on PORTC */
    LCD_Init();

    /* 5. Zero out boot initialization time */
    mTimer2_ResetTick();
    g_subSecondTicks = 0U;

    /* Write static labels to all 4 rows (20 columns each) */
    LCD_PrintStaticLabels();

    while (1)
    {
        current_ms = mTimer2_GetTick_ms();
        total_sec  = mTimer2_GetUptime_Seconds();

        /* =================================================================
         * 1.000 Hz HEARTBEAT ON PA1 (Strict Non-Drifting)
         * ================================================================= */
        if ((current_ms - last_heartbeat_ms) >= 500UL)
        {
            last_heartbeat_ms += 500UL;
            PORTA_Reg ^= (uint8_t)(1U << PIN_LED_1SEC);
        }

        /* =================================================================
         * VISUAL TEST FOR mTimer2_ResetTick() EVERY 15 SECONDS
         * ================================================================= */
        if (total_sec >= RESET_INTERVAL_SEC)
        {
            reset_count++;

            /* Visibly indicate reset is happening */
            LCD_SetCursor(3U, 10U);
            LCD_PrintString("RESETTING!");

            /* Call driver reset function */
            mTimer2_ResetTick();
            g_subSecondTicks = 0U;

            /* Re-sync heartbeat and LCD tracking */
            last_heartbeat_ms    = 0UL;
            last_lcd_update_ms   = 0xFFFFFFFFUL;
            last_displayed_sec   = 0xFFFFFFFFUL;
            last_displayed_reset = 0xFFU;

            /* Re-draw labels cleanly */
            LCD_PrintStaticLabels();
            continue;
        }

        /* =================================================================
         * 10MS REAL-TIME LCD UPDATE
         * Every tick increments by 10ms (5500 -> 5510 -> 5520...)
         * ================================================================= */
        if (current_ms != last_lcd_update_ms)
        {
            last_lcd_update_ms = current_ms;

            /* Row 0: g_sysTick_ms (Increments by +10 ms on EVERY single tick!) */
            LCD_SetCursor(0U, 10U);
            LCD_PrintField(current_ms, 6U);
            LCD_PrintString("ms ");

            /* Row 2: g_subSecondTicks (Counts 00..99 every second) */
            LCD_SetCursor(2U, 10U);
            LCD_Print2Digits(g_subSecondTicks);
            LCD_PrintString(" / 100  ");

            /* Row 1: g_uptime_seconds (Updates every whole second) */
            if (total_sec != last_displayed_sec)
            {
                last_displayed_sec = total_sec;
                LCD_SetCursor(1U, 10U);
                LCD_PrintField(total_sec, 5U);
                LCD_PrintString("sec ");
            }

            /* Row 3: Reset Counter (Updates when reset occurs) */
            if (reset_count != last_displayed_reset)
            {
                last_displayed_reset = reset_count;
                LCD_SetCursor(3U, 10U);
                LCD_PrintField((uint32_t)reset_count, 3U);
                LCD_PrintString("times ");
            }
        }
    }

    return 0;
}
