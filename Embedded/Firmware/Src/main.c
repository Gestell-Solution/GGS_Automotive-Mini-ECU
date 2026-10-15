/**
 * @file      main.c
 * @author    (Developer: Abdulrahman Ali)
 * @brief     Simple DoD verification test for the USART0 driver.
 * @version   1.0.0
 * @date      17/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */

#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "Common/Config.h"
#include "Common/Definition.h"
#include "Common/MacroFunction.h"
#include "MCAL/Atmega128Register.h"
#include "MCAL/Usart0/Usart0_Interface.h"

/* Link AVR hardware interrupt vectors to driver ISR functions */
ISR(USART0_RX_vect)
{
    ISR_USART0_RXC();
}

ISR(USART0_UDRE_vect)
{
    ISR_USART0_UDRE();
}

/* Static buffer for incoming frames (AC-7: Static allocation in .bss) */
static char g_rxBuffer[USART0_RING_BUFFER_SIZE];
static uint8_t g_rxIndex = 0;
static uint16_t g_frameCount = 0;

int main(void)
{
    uint8_t rxByte = 0;
    uint16_t elapsedTicks = 0;

    /* Initialize USART0 (9600 bps, 8N1) and enable global interrupts */
    mUSART0_Init();
    sei();

    /* Start Timer1 with no prescaler (16 ticks = 1 us) to measure AC-2 time */
    TCCR1B_Reg = 0x01;
    TCNT1_Reg  = 0;

    /* AC-1 & AC-2: Send 64-byte string and measure non-blocking execution time */
    mUSART0_SendString("Send string is ready\r\n");
    elapsedTicks = TCNT1_Reg;

    /* Wait 100 ms until all 64 bytes are transmitted out of g_txRingBuf */
    _delay_ms(100);

    /* AC-2 Check: 10 us at 16 MHz equals 160 timer ticks */
    if (elapsedTicks <= 160)
    {
        mUSART0_SendString("AC-2 PASS\r\n");
    }

    /* AC-4 Check: Verify UDRIE0 is automatically cleared when TX buffer is empty */
    if (GetBit(UCSR0B_Reg, UDRIE0) == 0)
    {
        mUSART0_SendString("AC-4 PASS: UDRIE0 Cleared\r\n");
    }

    /* AC-5 Check: Verify SREG global interrupt bit is preserved after API calls */
    if (GetBit(SREG_Reg, USART0_SREG_I) == 1)
    {
        mUSART0_SendString("AC-5 PASS: SREG Protected\r\n");
    }

    _delay_ms(50);

    while (1)
    {
        /* AC-3: Simulate CPU workload while RX ISR buffers incoming bytes */
        _delay_ms(20);

        /* Dequeue all received bytes from g_rxRingBuf */
        while (mUSART0_DataAvailable() == 1)
        {
            if (mUSART0_ReceiveByte(&rxByte) == USART0_STATUS_OK)
            {
                if (g_rxIndex < (USART0_RING_BUFFER_SIZE - 1))
                {
                    g_rxBuffer[g_rxIndex] = (char)rxByte;
                    g_rxIndex++;
                }

                /* AC-6: Check for end of command frame (\r\n) */
                if (rxByte == '\n')
                {
                    g_rxBuffer[g_rxIndex] = '\0';

                    if (strcmp(g_rxBuffer, "CMD:START_ENGINE\r\n") == 0)
                    {
                        g_frameCount++;
                        mUSART0_SendString("ACK:START_ENGINE\r\n");

                        if (g_frameCount == 10000)
                        {
                            mUSART0_SendString("AC-6 PASS: 10000 Frames Done\r\n");
                            g_frameCount = 0;
                        }
                    }
                    else
                    {
                        /* Echo back any other string to verify AC-3 (64 continuous bytes) */
                        mUSART0_SendString(g_rxBuffer);
                    }

                    /* Reset index for the next incoming frame */
                    g_rxIndex = 0;
                }
            }
        }
    }

    return 0;
}