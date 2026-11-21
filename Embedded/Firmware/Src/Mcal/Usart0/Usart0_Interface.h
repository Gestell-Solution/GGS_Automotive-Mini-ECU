/**
 * @file      Usart0_Interface.h
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Naira-mohammed)
 * @brief     Public interface for the USART0 driver.
 * @details   This header file declares the public functions and ISRs that can be used by other modules to interact with the USART0 peripheral.
 * @version   1.0.0
 * @date      17/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */
#ifndef USART0_INTERFACE_H
#define USART0_INTERFACE_H
#include <stdint.h>
#include "Usart0_Private.h"
#include "Usart0_Config.h"
#include <avr/interrupt.h>
#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include "../../Common/MacroFunction.h"

/**
 * @fn    void mUSART0_Init(void)
 * @brief Initializes USART0 peripheral for 9600 bps, 8N1, Interrupt RX/TX.
 *        Configures PE0 (RXD0) and PE1 (TXD0) hardware pins.
 */
void mUSART0_Init(void);

/**
 * @fn    USART0_Status_t mUSART0_SendByte(uint8_t data)
 * @brief Enqueues a single byte into the TX ring buffer and enables UDRE interrupt.
 * @param data Byte to transmit.
 * @return USART0_STATUS_OK if enqueued, USART0_STATUS_FULL if buffer is full.
 */
USART0_Status_t mUSART0_SendByte(uint8_t data);

/**
 * @fn    void mUSART0_SendString(const char *str)
 * @brief Enqueues a null-terminated string into the TX ring buffer non-blockingly.
 * @param str Pointer to null-terminated C-string.
 */
void mUSART0_SendString(const char *str);

/**
 * @fn    void mUSART0_SendBuffer(const uint8_t *data_ptr, uint16_t length)
 * @brief Enqueues a binary buffer of specified length into the TX ring buffer.
 * @param data_ptr Pointer to byte array.
 * @param length Number of bytes to transmit.
 */
void mUSART0_SendBuffer(const uint8_t *data_ptr, uint16_t length);

/**
 * @fn    USART0_Status_t mUSART0_ReceiveByte(uint8_t *data_ptr)
 * @brief Dequeues a single received byte from the RX ring buffer.
 * @param data_ptr Pointer to write dequeued byte.
 * @return USART0_STATUS_OK if byte retrieved, USART0_STATUS_EMPTY if empty.
 */
USART0_Status_t mUSART0_ReceiveByte(uint8_t *data_ptr);

/**
 * @fn    uint8_t mUSART0_DataAvailable(void)
 * @brief Checks if unread data is present in the RX ring buffer.
 * @return 1 if one or more bytes available, 0 if empty.
 */
uint8_t mUSART0_DataAvailable(void);

/**
 * @fn    uint8_t mUSART0_TxReady(void)
 * @brief Checks if the TX ring buffer has space available for new transmissions.
 * @return 1 if TX buffer count < USART0_RING_BUFFER_SIZE, 0 otherwise.
 */
uint8_t mUSART0_TxReady(void);

/**
 * @fn    void mUSART0_FlushRX(void)
 * @brief Flushes and clears all unread data from the RX ring buffer.
 */
void mUSART0_FlushRX(void);

/**
 * @fn    ISR(USART0_RX_vect)
 * @brief Interrupt Service Routine for USART0 Receive Complete.
 *        Captures byte from UDR0 into g_rxRingBuf.
 */
ISR(USART0_RX_vect);

/**
 * @fn    ISR(USART0_UDRE_vect)
 * @brief Interrupt Service Routine for USART0 Data Register Empty.
 *        Pops byte from g_txRingBuf into UDR0; disables UDRIE0 when empty.
 */
ISR(USART0_UDRE_vect);

#endif