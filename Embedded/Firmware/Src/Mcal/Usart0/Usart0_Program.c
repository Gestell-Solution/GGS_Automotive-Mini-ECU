/**
 * @file      Usart0_Program.c
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Naira-mohammed)
 * @brief     Implementation of the USART0 driver.
 * @details   This source file implements the USART0 driver API and interrupt service routines using TX/RX ring buffers.
 * @version   1.0.0
 * @date      17/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */
#include "../../Common/Config.h"
#include "Usart0_Interface.h"
#include "../Atmega128Register.h"

#if ModuleUsart0Enabled == Enable


/**
 * @enum USART0_RingBuffer_RX & TX
 * @brief Internal ring buffer instances transmission and reciver
 */
static USART0_RingBuffer_t g_rxRingBuf;
static USART0_RingBuffer_t g_txRingBuf;


/**
 * @brief Initializes USART0 peripheral for 9600 bps, 8N1, Interrupt RX/TX.
 *        Configures PE0 (RXD0) and PE1 (TXD0) hardware pins.
 */
void mUSART0_Init(void){

//Reset the Ring Buffer
g_rxRingBuf.head  =0;
g_rxRingBuf.tail  =0;
g_rxRingBuf.count =0;
g_txRingBuf.head  =0;
g_txRingBuf.tail  =0;
g_txRingBuf.count =0;



//Set the Baud Rate 9600
uint16_t UBRR_Value =(uint16_t)USART0_Baud_9600;
UBRR0H_Reg =(uint8_t)(UBRR_Value>>8);
UBRR0L_Reg =(uint8_t)UBRR_Value;

//Asynchronous Mode
ClearBit(UCSR0C_Reg,UMSEL0);

//Normal Asynchronous Speed
ClearBit(UCSR0A_Reg,U2X0);

//Parity Disable
ClearBit(UCSR0C_Reg,UPM00);
ClearBit(UCSR0C_Reg,UPM01);

//Set 1_Stop_Bit
ClearBit(UCSR0C_Reg,USBS0);

//Set Character_size_8 Data Bits
SetBit(UCSR0C_Reg,UCSZ00);
SetBit(UCSR0C_Reg,UCSZ01);
ClearBit(UCSR0B_Reg,UCSZ02);

//Enable RX Complete Interrupt
SetBit(UCSR0B_Reg,RXCIE0);
//Enable Receiver (RX)
SetBit(UCSR0B_Reg,RXEN0);
//Enable Transmitter (TX)
SetBit(UCSR0B_Reg,TXEN0);



}







#endif