/**
 * @file DIO_Interface.h
 * @author DEV salma mohamed mahmoud fouad(salmafouadothman@gmail.com) 
 * @author REV Abdulrahman Ali
 * @brief DIO driver interface
 * @details Api used in dio driver like init pin or group
 *          write pin or group,read pin or group,toggle pin
 *          and lastly set pin high or low.
 * @version 0.1
 * @date 2026-10-14
 * @copyright Copyright (c) 2026,GESTELL Company
 */
#ifndef DIO_INTERFACE_H_
#define DIO_INTERFACE_H_

#include"DIO_Private.h"

 /**
 * @brief Configures data direction (DIO_Input or DIO_Output) for an individual pin.
 * @param GroupName Target port group (GroupA through GroupG).
 * @param PinNo     Target pin index (Pin0 through Pin7).
 * @param DirectionState Desired direction (DIO_Input or DIO_Output).
 */
void mDIO_InitPin(uint8_t GroupName, uint8_t PinNo, uint8_t DirectionState);

/**
 * @brief Writes a digital logic level (Dio_Low or Dio_High) to an output pin.
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 * @param OutputValue Logic level to write (Dio_Low or Dio_High).
 */
void mDIO_WritePin(uint8_t GroupName, uint8_t PinNo, uint8_t OutputValue);

/**
 * @brief Reads the current digital logic level of a pin from PINx register.
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 * @return uint8_t 1 (Dio_High) if HIGH, 0 (Dio_Low) if LOW.
 */
uint8_t mDIO_ReadPin(uint8_t GroupName, uint8_t PinNo);

/**
 * @brief Toggles the current logic output state of a pin.
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 */
void mDIO_TogglePin(uint8_t GroupName, uint8_t PinNo);

/**
 * @brief Drives an output pin directly HIGH (1).
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 */
void mDIO_SetPinHigh(uint8_t GroupName, uint8_t PinNo);

/**
 * @brief Drives an output pin directly LOW (0).
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 */
void mDIO_SetPinLow(uint8_t GroupName, uint8_t PinNo);

/**
 * @brief Configures data direction for an entire 8-bit port group.
 * @param GroupName Target port group.
 * @param DirectionState Desired 8-bit direction mask (or DIO_Input / DIO_Output).
 */
void mDIO_InitGroup(uint8_t GroupName, uint8_t DirectionState);

/**
 * @brief Writes an 8-bit value directly to an entire port group.
 * @param GroupName Target port group.
 * @param OutputValue 8-bit value to write to PORTx.
 */
void mDIO_WriteGroup(DIO_GroupName_t GroupName, uint8_t OutputValue);

/**
 * @brief Reads the full 8-bit digital input value of an entire port group.
 * @param GroupName Target port group.
 * @return uint8_t Current 8-bit logic level from PINx.
 */
uint8_t mDIO_ReadGroup(DIO_GroupName_t GroupName);

#endif