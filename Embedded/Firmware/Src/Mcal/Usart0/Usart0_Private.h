/**
 * @file      Usart0_Private.h
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Naira-mohammed)
 * @brief     Private definitions for the USART0 driver.
 * @details   This header file defines the baud rate and status enums, the ring buffer structure and the critical section helpers used internally by the USART0 driver.
 * @version   1.0.0
 * @date      17/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
*/
#ifndef USART0_PRIVATE_H
#define USART0_PRIVATE_H
#include <stdint.h>
#include "Usart0_Config.h"

/**
 * @addtogroup UCSR0A Bits
 * @brief This section defines the bit positions for the UCSR0A register.
*/

 /**
 * @def RXC0
 * @brief Bit position for the RXC0 (Receive Complete) flag in the UCSR0A register.
 */
#define RXC0 7

/**
 * @def TXC0
 * @brief Bit position for the TXC0 (Transmit Complete) flag in the UCSR0A register.
 */
#define TXC0 6
/**
 * @def UDRE0
 * @brief Bit position for the UDRE0 (USART Data Register Empty) flag in the UCSR0A register.
 */
#define UDRE0 5
/**
 * @def FE0
 * @brief Bit position for the FE0 (Frame Error) flag in the UCSR0A register.
 */
#define FE0 4
/**
 * @def DOR0
 * @brief Bit position for the DOR0 (Data OverRun) flag in the UCSR0A register.
 */
#define DOR0 3
/**
 * @def UPE0
 * @brief Bit position for the UPE0 (USART Parity Error) flag in the UCSR0A register.
 */
#define UPE0 2
/**
 * @def U2X0
 * @brief Bit position for the U2X0 (Double the USART Transmission Speed) flag in the UCSR0A register.
 */
#define U2X0 1
/**
 * @def MPCM0
 * @brief Bit position for the MPCM0 (Multi-processor Communication Mode) flag in the UCSR0A register.
 */
#define MPCM0 0

/** @} */ // End of UCSR0A Bits



/**
 * @addtogroup UCSR0B Bits
 * @brief This section defines the bit positions for the UCSR0B register.
 * @{
 */
/**
 * @def RXCIE0
 * @brief Bit position for the RXCIE0 (RX Complete Interrupt Enable) flag in the UCSR0B register.
 */
#define RXCIE0 7

/**
 * @def TXCIE0
 * @brief Bit position for the TXCIE0 (TX Complete Interrupt Enable) flag in the UCSR0B register.
 */
#define TXCIE0 6

/**
 * @def UDRIE0
 * @brief Bit position for the UDRIE0 (USART Data Register Empty Interrupt Enable) flag in the UCSR0B register.
 */
#define UDRIE0 5

/**
 * @def RXEN0
 * @brief Bit position for the RXEN0 (Receiver Enable) flag in the UCSR0B register.
 */
#define RXEN0 4

/**
 * @def TXEN0
 * @brief Bit position for the TXEN0 (Transmitter Enable) flag in the UCSR0B register.
 */
#define TXEN0 3

/**
 * @def UCSZ02
 * @brief Bit position for the UCSZ02 (USART Character Size bit 2) flag in the UCSR0B register.
 */
#define UCSZ02 2

/**
 * @def RXB80
 * @brief Bit position for the ninth data bit of the received character when operating with 9-Data bits
 */
#define RXB80 1

/**
 * @def TXB80
 * @brief Bit position for  the 9th data bit in the character to be transmitted when operating with serial frames 9-Data bits
 */
#define TXB80 0

/** @} */ // End of UCSR0B Bits




/** 
* @addtogroup UCSR0C Bits
* @brief This section defines the bit positions for the UCSR0C register.
*/

/**
 * @def UMSEL0
 * @brief This bit selects between Asynchronous and Synchronous mode of operation.
 */
#define UMSEL0 6

/** @def UPM01
 * @brief Bit position for the UPM01 (USART Parity Mode bit 1) flag
 */
#define UPM01 5

/**
 * @def UPM00
 * @brief Bit position for the UPM00 (USART Parity Mode bit 0) flag
 */
#define UPM00 4

/** @def USBS0
 * @brief Bit position for the USBS0 (USART Stop Bit Select) flag
 */
#define USBS0 3

/**
 * @def UCSZ01
 * @brief Bit position for the UCSZ01 (USART Character Size bit 1) flag
 */
#define UCSZ01 2

/**
 * @def UCSZ00
 * @brief Bit position for the UCSZ00 (USART Character Size bit 0) flag
 */
#define UCSZ00 1

/** @def UCPOL0
 * @brief Bit position for the UCPOL0 (USART Clock Polarity) flag
 */
#define UCPOL0 0

/** @} */ // End of UCSR0C Bits

// /**
//  * @addtogroup USART0 Configurations Options
//  * @brief Parity Mode options 
// */
// #define No_Parity_Mode   0

// #define Even_Parity_Mode 2

// #define Odd_Parity_Mode  3


/**
 * @addtogroup Interrupt options
 */
#define RX_Interrupt_Enabled      1

#define TX_Interrupt_Enabled      1

#define RX_Interrupt_Disabled     0

#define TX_Interrupt_Disabled     0

#define UDRIE0_interrupt_Enabled  1

#define UDRIE0_interrupt_Disabled 0
/**@} */



/**
 * @addtogroup UART Running Mode Options
 * {@
*/

#define Transmitter_Mode 1

#define Reciever_Mode    2

#define Transciever_Mode 3
/** @} */





/**
 * @enum USART0_BaudRate_t
 * @brief Pre-calculated UBRR register values for 16 MHz clock.
 */
typedef enum {
    USART0_Baud_9600   = 103,  /* 9600 bps  (Error: +0.16%) */
    USART0_Baud_19200  = 51,   /* 19200 bps (Error: +0.16%) */
    USART0_Baud_38400  = 25,   /* 38400 bps (Error: +0.16%) */
    USART0_Baud_57600  = 16,   /* 57600 bps (Error: -0.79%) */
    USART0_Baud_115200 = 8     /* 115200 bps(Error: -3.55%) */
} USART0_BaudRate_t;

/**
 * @enum USART0_Status_t
 * @brief API return and ring buffer transaction status.
 */
typedef enum {
    USART0_STATUS_OK         = 0,  /* Operation successful */
    USART0_STATUS_EMPTY      = 1,  /* Buffer is empty, no data returned */
    USART0_STATUS_FULL       = 2,  /* Buffer is full, byte dropped */
    USART0_STATUS_ERROR      = 3,  /* Null pointer or hardware frame error */
    USART0_STATUS_BUSY       = 4   /* Hardware transmitter active */
} USART0_Status_t;

/**
 * @struct USART0_RingBuffer_t
 * @brief Circular FIFO buffer for asynchronous serial communication.
 */
typedef struct {
    uint8_t buffer[USART0_RING_BUFFER_SIZE];
    volatile uint8_t head;   /* Producer write index */
    volatile uint8_t tail;   /* Consumer read index */
    volatile uint8_t count;  /* Active byte count */
} USART0_RingBuffer_t;



/**
 * @addtogroup Data_Bits_Options
 * {@
 */
#define _5_Bit_Mode 0

#define _6_Bit_Mode 1

#define _7_Bit_Mode 2

#define _8_Bit_Mode 3 

#define _9_Bit_Mode 4 

/**@} */




#endif