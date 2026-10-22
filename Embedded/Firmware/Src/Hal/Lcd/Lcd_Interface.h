/**
 * @file      Lcd_Interface.h
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Beshoy Esmat)
 * @brief     Public interface for the Lcd driver.
 * @details   This header file declares the public functions that can be used by other modules to interact with the Lcd driver.
 * @version   1.0.0
 * @date      25/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */
#ifndef LCD_INTERFACE_H
#define LCD_INTERFACE_H
#include <stdint.h>
#include <Definition.h>
#include "DIO_Interface.h"
#include "Lcd_Private.h"
#include "../../MCAL/Atmega128Register.h"
#include <util/delay.h>
#include "Lcd_Config.h"
#include "Lcd_Config.h"

/**
 * @fn    void hLCD_Init(void)
 * @brief Performs the complete HD44780 4-bit hardware initialization sequence.
 *        Configures PC0, PC1, PC2, PC4-PC7 as outputs via MCAL DIO.
 */
void hLCD_Init(void);

/**
 * @fn    void hLCD_WriteCommand(uint8_t cmd)
 * @brief Sends an 8-bit command to the LCD instruction register (RS = LOW).
 * @param cmd HD44780 instruction byte.
 */
void hLCD_WriteCommand(uint8_t cmd);

/**
 * @fn    void hLCD_WriteData(uint8_t data)
 * @brief Writes a single ASCII character directly to the LCD at current cursor (RS = HIGH).
 * @param data ASCII character code.
 */
void hLCD_WriteData(uint8_t data);

/**
 * @fn    Std_ReturnType hLCD_SetCursor(uint8_t row, uint8_t col)
 * @brief Sets the hardware cursor position.
 * @param row Display row (0 to 3).
 * @param col Display column (0 to 19).
 * @return Std_ReturnType EOk if coordinates valid, ENotOk if out of bounds.
 */
Std_ReturnType hLCD_SetCursor(uint8_t row, uint8_t col);

/**
 * @fn    Std_ReturnType hLCD_WriteString(uint8_t row, uint8_t col, const char *str)
 * @brief Writes a string into the shadow buffer at specified row and col.
 *        Sets the dirty flag for the line if text changed.
 * @param row Display row (0 to 3).
 * @param col Display column (0 to 19).
 * @param str Null-terminated string to write.
 * @return Std_ReturnType EOk if successful, ENotOk if parameters invalid.
 */
Std_ReturnType hLCD_WriteString(uint8_t row, uint8_t col, const char *str);

/**
 * @fn    void hLCD_Clear(void)
 * @brief Clears the shadow buffer (fills with spaces) and marks all lines dirty.
 *        Does NOT execute hardware command 0x01 to prevent screen flicker.
 */
void hLCD_Clear(void);

/**
 * @fn    void hLCD_UpdateDirtyLines(void)
 * @brief Flushes modified shadow buffer lines to the physical LCD hardware.
 *        Called periodically by the OS scheduler (e.g. every 200 ms).
 */
void hLCD_UpdateDirtyLines(void);

/**
 * @fn    void hLCD_SetDirty(uint8_t row)
 * @brief Manually marks a specific line as dirty to force a hardware redraw.
 * @param row Row index (0 to 3).
 */
void hLCD_SetDirty(uint8_t row);

/**
 * @fn    bool hLCD_IsDirty(uint8_t row)
 * @brief Checks if a specific line currently has pending modifications.
 * @param row Row index (0 to 3).
 * @return bool true if dirty, false if unchanged.
 */
uint8_t hLCD_IsDirty(uint8_t row);

/**
 * @fn    void hLCD_CreateCustomChar(uint8_t patternIndex, const uint8_t charMap[8])
 * @brief Uploads a custom 5x8 character pattern into LCD CGRAM.
 * @param patternIndex Location in CGRAM (0 to 7).
 * @param charMap Array of 8 byte row bitmasks.
 */
void hLCD_CreateCustomChar(uint8_t patternIndex, const uint8_t charMap[8]);

/**
 * @defgroup LCD_Aliases LCD Compatibility Aliases
 * @brief    Short alias names mapped to the hLCD_ driver functions.
 * @details  These macros let legacy code that uses the LCD_ prefix compile without any change
 * @{
 */
#define LCD_Init() hLCD_Init()
#define LCD_Clear() hLCD_Clear()
#define LCD_SetCursor(r, c) hLCD_SetCursor((r), (c))
#define LCD_WriteString(r, c, s) hLCD_WriteString((r), (c), (s))
#define LCD_WriteCommand(cmd) hLCD_WriteCommand((cmd))
#define LCD_WriteData(d) hLCD_WriteData((d))
#define LCD_UpdateDirtyLines() hLCD_UpdateDirtyLines()
#define LCD_CreateCustomChar(idx, map) hLCD_CreateCustomChar((idx), (map))

#endif