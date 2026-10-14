/**
 * @file      Atmega128Register.h
 * @brief     ATmega128 Hardware Register Definitions (US-000)
 * @details   Centralized memory-mapped register definitions for all ATmega128
 *            peripherals used by the Gestell Automotive Mini-ECU MCAL layer.
 *            All registers are accessed via volatile pointer-cast helper macros
 *            using absolute data-space addresses from the ATmega128 datasheet.
 *
 * @ref       ATmega128 Datasheet — Rev. 2467X–AVR–06/11
 *
 * @note      I/O registers in the range 0x20–0x5F (Classic I/O space) are
 *            addressed using their data-space address (I/O addr + 0x20).
 *            Extended I/O registers (0x60–0xFF) are accessed directly.
 */

#ifndef _EMBEDDED_FIRMWARE_SRC_MCAL_ATMEGA128REGISTER_H
#define _EMBEDDED_FIRMWARE_SRC_MCAL_ATMEGA128REGISTER_H

#include <stdint.h>

/* =========================================================================
 * SECTION 1 — Address Helper Macros
 * ========================================================================= */

/** @brief  Dereference a memory-mapped 8-bit register at the given address. */
#define _SetAddress8Bit(Address) (*((volatile uint8_t *)(Address)))

/** @brief  Dereference a memory-mapped 16-bit register at the given address. */
#define _SetAddress16Bit(Address) (*((volatile uint16_t *)(Address)))

/* =========================================================================
 * SECTION 2 — CPU Core & Status Registers
 * =========================================================================
 * Data-space: 0x3F (SREG), 0x3E (SPH), 0x3D (SPL),
 *             0x35 (MCUCR), 0x34 (MCUCSR), 0x41 (WDTCR)
 * ========================================================================= */

/** @brief Status Register */
#define SREG_Reg _SetAddress8Bit(0x5FU)

/** @brief Stack Pointer High byte */
#define SPH_Reg _SetAddress8Bit(0x5EU)

/** @brief Stack Pointer Low byte */
#define SPL_Reg _SetAddress8Bit(0x5DU)

/** @brief MCU Control Register */
#define MCUCR_Reg _SetAddress8Bit(0x55U)

/** @brief MCU Control and Status Register */
#define MCUCSR_Reg _SetAddress8Bit(0x54U)

/** @brief Watchdog Timer Control Register */
#define WDTCR_Reg _SetAddress8Bit(0x41U)
/* =========================================================================
 * SECTION 3 — DIO Port Registers (A – G)
 * =========================================================================
 * Classic I/O (data-space = I/O + 0x20):
 *   PORTA=0x3B, DDRA=0x3A, PINA=0x39
 *   PORTB=0x38, DDRB=0x37, PINB=0x36
 *   PORTC=0x35, DDRC=0x34, PINC=0x33
 *   PORTD=0x32, DDRD=0x31, PIND=0x30
 *   PORTE=0x23, DDRE=0x22, PINE=0x21
 * Extended I/O (data-space directly):
 *   PORTF=0x62, DDRF=0x61, PINF=0x20 (PINF is Classic I/O)
 *   PORTG=0x65, DDRG=0x64, PING=0x63
 * ========================================================================= */

/* --- Port A --- */
#define PORTA_Reg _SetAddress8Bit(0x3BU) /**< Port A Data Register */
#define DDRA_Reg _SetAddress8Bit(0x3AU)  /**< Port A Data Direction Register */
#define PINA_Reg _SetAddress8Bit(0x39U)  /**< Port A Input Pins */

/* --- Port B --- */
#define PORTB_Reg _SetAddress8Bit(0x38U) /**< Port B Data Register */
#define DDRB_Reg _SetAddress8Bit(0x37U)  /**< Port B Data Direction Register */
#define PINB_Reg _SetAddress8Bit(0x36U)  /**< Port B Input Pins */

/* --- Port C --- */
#define PORTC_Reg _SetAddress8Bit(0x35U) /**< Port C Data Register */
#define DDRC_Reg _SetAddress8Bit(0x34U)  /**< Port C Data Direction Register */
#define PINC_Reg _SetAddress8Bit(0x33U)  /**< Port C Input Pins */

/* --- Port D --- */
#define PORTD_Reg _SetAddress8Bit(0x32U) /**< Port D Data Register */
#define DDRD_Reg _SetAddress8Bit(0x31U)  /**< Port D Data Direction Register */
#define PIND_Reg _SetAddress8Bit(0x30U)  /**< Port D Input Pins */

/* --- Port E --- */
#define PORTE_Reg _SetAddress8Bit(0x23U) /**< Port E Data Register */
#define DDRE_Reg _SetAddress8Bit(0x22U)  /**< Port E Data Direction Register */
#define PINE_Reg _SetAddress8Bit(0x21U)  /**< Port E Input Pins */

/* --- Port F (Extended I/O) --- */
#define PORTF_Reg _SetAddress8Bit(0x62U) /**< Port F Data Register */
#define DDRF_Reg _SetAddress8Bit(0x61U)  /**< Port F Data Direction Register */
#define PINF_Reg _SetAddress8Bit(0x20U)  /**< Port F Input Pins (Classic I/O space) */

/* --- Port G (Extended I/O) --- */
#define PORTG_Reg _SetAddress8Bit(0x65U) /**< Port G Data Register */
#define DDRG_Reg _SetAddress8Bit(0x64U)  /**< Port G Data Direction Register */
#define PING_Reg _SetAddress8Bit(0x63U)  /**< Port G Input Pins */

/* =========================================================================
 * SECTION 4 — ADC Registers
 * =========================================================================
 * ADMUX=0x27, ADCSRA=0x26, ADCH=0x25, ADCL=0x24 (Classic I/O)
 * ADCSRB=0x7B (Extended I/O)
 * ADC data register 16-bit: 0x24 (ADCL/ADCH pair)
 * ========================================================================= */

/** @brief ADC Multiplexer Selection Register */
#define ADMUX_Reg _SetAddress8Bit(0x27U)

/** @brief ADC Control and Status Register A */
#define ADCSRA_Reg _SetAddress8Bit(0x26U)

/** @brief ADC Data Register High byte */
#define ADCH_Reg _SetAddress8Bit(0x25U)

/** @brief ADC Data Register Low byte */
#define ADCL_Reg _SetAddress8Bit(0x24U)

/** @brief ADC Data Register (16-bit combined read — ADCL must be read first) */
#define ADC_Reg _SetAddress16Bit(0x24U)

/** @brief ADC Control and Status Register B (Extended I/O) */
#define ADCSRB_Reg _SetAddress8Bit(0x7BU)

/* =========================================================================
 * SECTION 5 — Timer0 Registers (8-bit — Fan PWM / OC0 on PB4)
 * =========================================================================
 * TCCR0=0x53, TCNT0=0x52, OCR0=0x51, TIMSK=0x57, TIFR=0x56 (Classic I/O)
 * ========================================================================= */

/** @brief Timer/Counter0 Control Register */
#define TCCR0_Reg _SetAddress8Bit(0x53U)

/** @brief Timer/Counter0 Register */
#define TCNT0_Reg _SetAddress8Bit(0x52U)

/** @brief Output Compare Register 0 */
#define OCR0_Reg _SetAddress8Bit(0x51U)

/** @brief Timer/Counter Interrupt Mask Register (shared T0/T1) */
#define TIMSK_Reg _SetAddress8Bit(0x57U)

/** @brief Timer/Counter Interrupt Flag Register (shared T0/T1) */
#define TIFR_Reg _SetAddress8Bit(0x56U)
/* =========================================================================
 * SECTION 6 — Timer1 Registers (16-bit)
 * =========================================================================
 * TCCR1A=0x4F, TCCR1B=0x4E, TCCR1C=0x7A (Extended I/O)
 * TCNT1H=0x4D, TCNT1L=0x4C, TCNT1=0x4C (16-bit)
 * OCR1AH=0x4B, OCR1AL=0x4A, OCR1A=0x4A (16-bit)
 * OCR1BH=0x49, OCR1BL=0x48, OCR1B=0x48 (16-bit)
 * OCR1CH=0x79, OCR1CL=0x78, OCR1C=0x78 (16-bit, Extended I/O)
 * ICR1H=0x47, ICR1L=0x46, ICR1=0x46 (16-bit)
 * ========================================================================= */

/** @brief Timer/Counter1 Control Register A */
#define TCCR1A_Reg _SetAddress8Bit(0x4FU)

/** @brief Timer/Counter1 Control Register B */
#define TCCR1B_Reg _SetAddress8Bit(0x4EU)

/** @brief Timer/Counter1 Control Register C (Extended I/O) */
#define TCCR1C_Reg _SetAddress8Bit(0x7AU)

/** @brief Timer/Counter1 Register (16-bit) */
#define TCNT1_Reg _SetAddress16Bit(0x4CU)

/** @brief Timer/Counter1 Register High byte */
#define TCNT1H_Reg _SetAddress8Bit(0x4DU)

/** @brief Timer/Counter1 Register Low byte */
#define TCNT1L_Reg _SetAddress8Bit(0x4CU)

/** @brief Output Compare Register 1A (16-bit) */
#define OCR1A_Reg _SetAddress16Bit(0x4AU)

/** @brief Output Compare Register 1A High byte */
#define OCR1AH_Reg _SetAddress8Bit(0x4BU)

/** @brief Output Compare Register 1A Low byte */
#define OCR1AL_Reg _SetAddress8Bit(0x4AU)

/** @brief Output Compare Register 1B (16-bit) */
#define OCR1B_Reg _SetAddress16Bit(0x48U)

/** @brief Output Compare Register 1B High byte */
#define OCR1BH_Reg _SetAddress8Bit(0x49U)

/** @brief Output Compare Register 1B Low byte */
#define OCR1BL_Reg _SetAddress8Bit(0x48U)

/** @brief Output Compare Register 1C (16-bit, Extended I/O) */
#define OCR1C_Reg _SetAddress16Bit(0x78U)

/** @brief Output Compare Register 1C High byte */
#define OCR1CH_Reg _SetAddress8Bit(0x79U)

/** @brief Output Compare Register 1C Low byte */
#define OCR1CL_Reg _SetAddress8Bit(0x78U)

/** @brief Input Capture Register 1 (16-bit) */
#define ICR1_Reg _SetAddress16Bit(0x46U)

/** @brief Input Capture Register 1 High byte */
#define ICR1H_Reg _SetAddress8Bit(0x47U)

/** @brief Input Capture Register 1 Low byte */
#define ICR1L_Reg _SetAddress8Bit(0x46U)
/* =========================================================================
 * SECTION 7 — Timer2 Registers (8-bit — SysTick / CTC mode)
 * =========================================================================
 * TCCR2=0x45, TCNT2=0x44, OCR2=0x43 (Classic I/O)
 * ETIMSK=0x7D, ETIFR=0x7C (Extended I/O) — Timer2 OC interrupt enable
 * ========================================================================= */

/** @brief Timer/Counter2 Control Register */
#define TCCR2_Reg _SetAddress8Bit(0x45U)

/** @brief Timer/Counter2 Register */
#define TCNT2_Reg _SetAddress8Bit(0x44U)

/** @brief Output Compare Register 2 */
#define OCR2_Reg _SetAddress8Bit(0x43U)

/** @brief Extended Timer Interrupt Mask Register (Extended I/O) */
#define ETIMSK_Reg _SetAddress8Bit(0x7DU)

/** @brief Extended Timer Interrupt Flag Register (Extended I/O) */
#define ETIFR_Reg _SetAddress8Bit(0x7CU)
/* =========================================================================
 * SECTION 8 — Timer3 Registers (16-bit, Extended I/O)
 * =========================================================================
 * TCCR3A=0x8B, TCCR3B=0x8A, TCCR3C=0x8C
 * TCNT3H=0x8D, TCNT3L=0x8E, TCNT3=0x8E (16-bit)
 * OCR3AH=0x87, OCR3AL=0x86, OCR3A=0x86 (16-bit)
 * OCR3BH=0x85, OCR3BL=0x84, OCR3B=0x84 (16-bit)
 * OCR3CH=0x83, OCR3CL=0x82, OCR3C=0x82 (16-bit)
 * ICR3H=0x81, ICR3L=0x80, ICR3=0x80 (16-bit)
 * ========================================================================= */

/** @brief Timer/Counter3 Control Register A (Extended I/O) */
#define TCCR3A_Reg _SetAddress8Bit(0x8BU)

/** @brief Timer/Counter3 Control Register B (Extended I/O) */
#define TCCR3B_Reg _SetAddress8Bit(0x8AU)

/** @brief Timer/Counter3 Control Register C (Extended I/O) */
#define TCCR3C_Reg _SetAddress8Bit(0x8CU)

/** @brief Timer/Counter3 Register (16-bit, Extended I/O) */
#define TCNT3_Reg _SetAddress16Bit(0x8EU)

/** @brief Timer/Counter3 Register High byte */
#define TCNT3H_Reg _SetAddress8Bit(0x8DU)

/** @brief Timer/Counter3 Register Low byte */
#define TCNT3L_Reg _SetAddress8Bit(0x8EU)

/** @brief Output Compare Register 3A (16-bit, Extended I/O) */
#define OCR3A_Reg _SetAddress16Bit(0x86U)

/** @brief Output Compare Register 3A High byte */
#define OCR3AH_Reg _SetAddress8Bit(0x87U)

/** @brief Output Compare Register 3A Low byte */
#define OCR3AL_Reg _SetAddress8Bit(0x86U)

/** @brief Output Compare Register 3B (16-bit, Extended I/O) */
#define OCR3B_Reg _SetAddress16Bit(0x84U)

/** @brief Output Compare Register 3B High byte */
#define OCR3BH_Reg _SetAddress8Bit(0x85U)

/** @brief Output Compare Register 3B Low byte */
#define OCR3BL_Reg _SetAddress8Bit(0x84U)

/** @brief Output Compare Register 3C (16-bit, Extended I/O) */
#define OCR3C_Reg _SetAddress16Bit(0x82U)

/** @brief Output Compare Register 3C High byte */
#define OCR3CH_Reg _SetAddress8Bit(0x83U)

/** @brief Output Compare Register 3C Low byte */
#define OCR3CL_Reg _SetAddress8Bit(0x82U)

/** @brief Input Capture Register 3 (16-bit, Extended I/O) */
#define ICR3_Reg _SetAddress16Bit(0x80U)

/** @brief Input Capture Register 3 High byte */
#define ICR3H_Reg _SetAddress8Bit(0x81U)

/** @brief Input Capture Register 3 Low byte */
#define ICR3L_Reg _SetAddress8Bit(0x80U)
/* =========================================================================
 * SECTION 9 — External Interrupt Registers (EXTI)
 * =========================================================================
 * EICRA=0x6A, EICRB=0x6B (Extended I/O)
 * EIMSK=0x39, EIFR=0x38 (Classic I/O — data-space addresses)
 * ========================================================================= */

/** @brief External Interrupt Control Register A (Extended I/O) */
#define EICRA_Reg _SetAddress8Bit(0x6AU)

/** @brief External Interrupt Control Register B (Extended I/O) */
#define EICRB_Reg _SetAddress8Bit(0x5AU)

/** @brief External Interrupt Mask Register */
#define EIMSK_Reg _SetAddress8Bit(0x59U)

/** @brief External Interrupt Flag Register */
#define EIFR_Reg _SetAddress8Bit(0x58U)
/* =========================================================================
 * SECTION 10 — USART0 Registers (Bluetooth HC-05, PE0/PE1)
 * =========================================================================
 * UDR0=0x2C, UCSR0A=0x2B, UCSR0B=0x2A, UCSR0C=0x95 (Extended I/O)
 * UBRR0H=0x90 (Extended I/O), UBRR0L=0x29 (Classic I/O)
 * ========================================================================= */

/** @brief USART0 Data Register */
#define UDR0_Reg _SetAddress8Bit(0x2CU)

/** @brief USART0 Control and Status Register A */
#define UCSR0A_Reg _SetAddress8Bit(0x2BU)

/** @brief USART0 Control and Status Register B */
#define UCSR0B_Reg _SetAddress8Bit(0x2AU)

/** @brief USART0 Control and Status Register C (Extended I/O) */
#define UCSR0C_Reg _SetAddress8Bit(0x95U)

/** @brief USART0 Baud Rate Register Low byte */
#define UBRR0L_Reg _SetAddress8Bit(0x29U)

/** @brief USART0 Baud Rate Register High byte (Extended I/O) */
#define UBRR0H_Reg _SetAddress8Bit(0x90U)
/* =========================================================================
 * SECTION 11 — USART1 Registers (Wi-Fi ESP-01, PD2/PD3)
 * =========================================================================
 * UDR1=0x9C, UCSR1A=0x9B, UCSR1B=0x9A, UCSR1C=0x9D (Extended I/O)
 * UBRR1H=0x98, UBRR1L=0x99 (Extended I/O)
 * ========================================================================= */

/** @brief USART1 Data Register (Extended I/O) */
#define UDR1_Reg _SetAddress8Bit(0x9CU)

/** @brief USART1 Control and Status Register A (Extended I/O) */
#define UCSR1A_Reg _SetAddress8Bit(0x9BU)

/** @brief USART1 Control and Status Register B (Extended I/O) */
#define UCSR1B_Reg _SetAddress8Bit(0x9AU)

/** @brief USART1 Control and Status Register C (Extended I/O) */
#define UCSR1C_Reg _SetAddress8Bit(0x9DU)

/** @brief USART1 Baud Rate Register High byte (Extended I/O) */
#define UBRR1H_Reg _SetAddress8Bit(0x98U)

/** @brief USART1 Baud Rate Register Low byte (Extended I/O) */
#define UBRR1L_Reg _SetAddress8Bit(0x99U)

/* =========================================================================
 * SECTION 12 — SPI Registers
 * =========================================================================
 * SPCR=0x2D, SPSR=0x2E, SPDR=0x2F (Classic I/O)
 * ========================================================================= */

/** @brief SPI Control Register */
#define SPCR_Reg _SetAddress8Bit(0x2DU)

/** @brief SPI Status Register */
#define SPSR_Reg _SetAddress8Bit(0x2EU)

/** @brief SPI Data Register */
#define SPDR_Reg _SetAddress8Bit(0x2FU)
/* =========================================================================
 * SECTION 13 — TWI / I2C Registers
 * =========================================================================
 * TWBR=0x20, TWSR=0x21, TWAR=0x22, TWDR=0x23 ... wait
 * Actual data-space: TWBR=0x70, TWSR=0x71, TWAR=0x72, TWDR=0x73 (Extended I/O)
 * TWCR=0x74, TWAMR=0x75 (Extended I/O)
 * ========================================================================= */

/** @brief TWI Bit Rate Register (Extended I/O) */
#define TWBR_Reg _SetAddress8Bit(0x70U)

/** @brief TWI Status Register (Extended I/O) */
#define TWSR_Reg _SetAddress8Bit(0x71U)

/** @brief TWI (Slave) Address Register (Extended I/O) */
#define TWAR_Reg _SetAddress8Bit(0x72U)

/** @brief TWI Data Register (Extended I/O) */
#define TWDR_Reg _SetAddress8Bit(0x73U)

/** @brief TWI Control Register (Extended I/O) */
#define TWCR_Reg _SetAddress8Bit(0x74U)

/** @brief TWI (Slave) Address Mask Register (Extended I/O) */
#define TWAMR_Reg _SetAddress8Bit(0x75U)

/* =========================================================================
 * SECTION 14 — EEPROM Registers
 * =========================================================================
 * EEARH=0x3F ... wait: EEARH=0x22 (I/O=0x02), EEARL=0x21 (I/O=0x01)
 * Actual data-space: EEARH=0x42, EEARL=0x41, EEDR=0x40, EECR=0x3F
 * ========================================================================= */

/** @brief EEPROM Address Register High byte */
#define EEARH_Reg _SetAddress8Bit(0x42U)

/** @brief EEPROM Address Register Low byte */
#define EEARL_Reg _SetAddress8Bit(0x41U)

/** @brief EEPROM Data Register */
#define EEDR_Reg _SetAddress8Bit(0x40U)

/** @brief EEPROM Control Register */
#define EECR_Reg _SetAddress8Bit(0x3FU)

#endif /* _EMBEDDED_FIRMWARE_SRC_MCAL_ATMEGA128REGISTER_H */