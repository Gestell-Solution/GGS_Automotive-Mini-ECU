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
#include <avr/interrupt.h>

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


/**
 * @brief Enqueues a single byte into the TX ring buffer and enables UDRE interrupt.
 * @param data Byte to transmit.
 * @return USART0_STATUS_OK if enqueued, USART0_STATUS_FULL if buffer is full.
 */
USART0_Status_t mUSART0_SendByte(uint8_t data){

//Save SREG & cli and Enter Critical Section
uint8_t sreg = SREG_Reg;
cli();
// ClearBit(SREG_Reg,USART0_SREG_I);

//Check TX ring buffer full then we restore SREG and return USART0 Status Full
if(g_txRingBuf.count == USART0_RING_BUFFER_SIZE){
SREG_Reg=sreg;
return USART0_STATUS_FULL;
}

//Space is available then we restore data at buffer[head]
g_txRingBuf.buffer[g_txRingBuf.head]=data;

//Update head = (g_txRingBuf.head + 1) & (USART0_RING_BUFFER_SIZE - 1).
g_txRingBuf.head =(uint8_t)((g_txRingBuf.head + 1) & (USART0_RING_BUFFER_SIZE - 1));

//Increment g_txRingBuf.count++.
g_txRingBuf.count++;

//Enable UDRIE0 in UCSR0B (UCSR0B |= (1 << UDRIE0)).
SetBit(UCSR0B_Reg,UDRIE0);

//Restore SREG.
SREG_Reg=sreg;

//Return USART0_STATUS_OK.
return USART0_STATUS_OK;
}


/**
 * @brief Enqueues a null-terminated string into the TX ring buffer non-blockingly.
 * @param str Pointer to null-terminated C-string.
 */
void mUSART0_SendString(const char *str){

//validate iput pointer and return immediately if is Null
if(str == Null){
  return;
}

//we Iterate through string characters until *str == '\0'
while (*str != '\0')
{
//we call mUSART0_Sendbyte to see if the TX is full and break the iteration to prevent deadlocks
if (mUSART0_SendByte((uint8_t)(*str)) == USART0_STATUS_FULL)
{
  break;
}
//Advance pointer str++.
str++;
}
}

/**
 * @brief Enqueues a binary buffer of specified length into the TX ring buffer.
 * @param data_ptr Pointer to byte array.
 * @param length Number of bytes to transmit.
 */
void mUSART0_SendBuffer(const uint8_t *data_ptr, uint16_t length){
//Validate parameters we check if data_ptr == NULL or length == 0 and then we return immediately.
if((data_ptr == Null) || (length == 0)){
    return ;
}
// loop through length bytes
for(uint16_t i =0 ; i<length ;i++){
    USART0_Status_t status = mUSART0_SendByte(data_ptr[i]);
//if return value is USART0_STATUS_FULL Then Terminate transmission loop 
if(status == USART0_STATUS_FULL){
    break;
}
}
}


/**
 * @brief Dequeues a single received byte from the RX ring buffer.
 * @param data_ptr Pointer to write dequeued byte.
 * @return USART0_STATUS_OK if byte retrieved, USART0_STATUS_EMPTY if empty.
 */
USART0_Status_t mUSART0_ReceiveByte(uint8_t *data_ptr){
//we Check parameter first if data_ptr == NULL then return USART0_STATUS_ERROR.
if(data_ptr == Null){
    return USART0_STATUS_ERROR;
}

//Enter critical section to save SREG and execute cli().
uint8_t sreg = SREG_Reg;
cli();
// ClearBit(SREG_Reg,USART0_SREG_I);

//Check g_rxRingBuf.count == 0 and If true (buffer empty) restore SREG and return USART0_STATUS_EMPTY.
if(g_rxRingBuf.count == 0){
    SREG_Reg =sreg;
    return USART0_STATUS_EMPTY;
}

//if byte available we fetch the data ptr to equail the tail and thn updata the tail
*data_ptr = g_rxRingBuf.buffer[g_rxRingBuf.tail];
//updata the tail
g_rxRingBuf.tail =(g_rxRingBuf.tail + 1) & (USART0_RING_BUFFER_SIZE - 1);
//decrement the g_rxRingBuf.count
g_rxRingBuf.count--;
//restore SREG
SREG_Reg = sreg;

//return the status
return USART0_STATUS_OK;

}



/**
 * @brief Checks if unread data is present in the RX ring buffer.
 * @return 1 if one or more bytes available, 0 if empty.
 */
uint8_t mUSART0_DataAvailable(void){
//Save SREG and execute cli()
uint8_t sreg = SREG_Reg;
cli();
// ClearBit(SREG_Reg,USART0_SREG_I);
//Check if rx available or not
uint8_t available;
if(g_rxRingBuf.count > 0){
    available = 1;

}else{
    available = 0;
}

//Restore SREG
SREG_Reg = sreg;
//return available
return available;
}


/**
 * @brief Checks if the TX ring buffer has space available for new transmissions.
 * @return 1 if TX buffer count < USART0_RING_BUFFER_SIZE, 0 otherwise.
 */
uint8_t mUSART0_TxReady(void){
//Save SREG and execute cli().
uint8_t sreg = SREG_Reg;
cli();
// ClearBit(SREG_Reg,USART0_SREG_I);

//Check ready = (g_txRingBuf.count < USART0_RING_BUFFER_SIZE) 
uint8_t ready;
if(g_txRingBuf.count < USART0_RING_BUFFER_SIZE){
    ready = 1;
}else{
    ready = 0;
}

//Restore SREG
SREG_Reg = sreg;

//return ready
return ready;
}



/**
 * @brief Flushes and clears all unread data from the RX ring buffer.
 */
void mUSART0_FlushRX(void){
//Save SREG and execute cli()
uint8_t sreg = SREG_Reg;
cli();
// ClearBit(SREG_Reg,USART0_SREG_I);

//Reset g_rxRingBuf.head , g_rxRingBuf.tail and  g_rxRingBuf.count 
g_rxRingBuf.head  = 0;
g_rxRingBuf.tail  = 0;
g_rxRingBuf.count = 0;

//Restore SREG
SREG_Reg = sreg;

}




/**
 * @brief Interrupt Service Routine for USART0 Receive Complete.
 *        Captures byte from UDR0 into g_rxRingBuf.
 */
void ISR_USART0_RXC(void){
//Read status register UCSR0A to check for error flags (FE0, DOR0, UPE0)
uint8_t status = UCSR0A_Reg;
//Read received byte from hardware register UDR0 (this read clears the hardware interrupt condition)
uint8_t received_data = UDR0_Reg ;

//Check on error flag detected or g_rxRingBuf.count == USART0_RING_BUFFER_SIZE
if ((GetBit(status, FE0) != 0) || (GetBit(status, DOR0) != 0) || (GetBit(status, UPE0) != 0))
{
//Drop the Byte
}
else if (g_rxRingBuf.count == USART0_RING_BUFFER_SIZE)
{       
//Buffer overrun detected 
}
else{
//Store byte at g_rxRingBuf.buffer[g_rxRingBuf.head]
g_rxRingBuf.buffer[g_rxRingBuf.head] = received_data;
// Update head = (g_rxRingBuf.head + 1) & (USART0_RING_BUFFER_SIZE - 1)
g_rxRingBuf.head = (g_rxRingBuf.head + 1) & (USART0_RING_BUFFER_SIZE - 1);
//Increment g_rxRingBuf.count
g_rxRingBuf.count++;
}
}


/**
 * @brief Interrupt Service Routine for USART0 Data Register Empty.
 *        Pops byte from g_txRingBuf into UDR0; disables UDRIE0 when empty.
 */
void ISR_USART0_UDRE(void){
//we checkvfirst if g_txRingBuf.count empty or equal zero 
if(g_txRingBuf.count == 0)
{
//and if the txRing Buffer is empty we clear UDRIE0
ClearBit(UCSR0B_Reg,UDRIE0);
}
else
{
//Read byte from g_txRingBuf.buffer[g_txRingBuf.tail].
uint8_t byte = g_txRingBuf.buffer[g_txRingBuf.tail];
//Update the tail
g_txRingBuf.tail = (g_txRingBuf.tail + 1) & (USART0_RING_BUFFER_SIZE - 1);
//Decrement the active byte count
g_txRingBuf.count--;
//Write byte to hardware register UDR0
UDR0_Reg = byte;
}
}















#endif