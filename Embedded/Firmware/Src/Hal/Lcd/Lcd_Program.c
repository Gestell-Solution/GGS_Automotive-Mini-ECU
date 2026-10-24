/**
 * @file      Lcd_Program.c
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Beshoy Esmat)
 * @brief     Implementation of the Lcd driver.
 * @details   This source file implements the Lcd driver API .
 * @version   1.0.0
 * @date      25/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */
#include "../../Common/Config.h"
#include "../../Common/Definition.h"

#if ModuleLcdEnabled == Enable

#include "Lcd_Interface.h"

/**
 * @brief Internal Data
 */
static char g_lcdShadow[LCD_ROWS][LCD_COLS + 1U];
static uint8_t g_lcdDirtyFlags = 0U;

static void LCD_PulseEnable(void)
{
    /**
     * @brief  Generates a short pulse on the LCD Enable (E) pin.
     * @note   E is driven high, held for ~1 µs, then driven low. The LCD
     *         latches the data on the falling edge of E.
     * @return None
     */
    mDIO_SetPinHigh(Lcd_EGroup, Lcd_EPin);
    _delay_us(1);
    mDIO_SetPinLow(Lcd_EGroup, Lcd_EPin);
}

static void LCD_WriteNibble(uint8_t nibble)
{
    /**
     * @brief  Writes a 4-bit nibble to the LCD data pins (D4–D7).
     * @param  nibble The 4-bit value to output (only the lower 4 bits are used).
     * @note   Bit 0 maps to D4 and bit 3 maps to D7. The Enable pulse is not
     *         generated here; the caller must toggle E to latch the data.
     * @return None
     */
    mDIO_WritePin(Lcd_D4Group, Lcd_D4Pin, GetBit(nibble, 0));
    mDIO_WritePin(Lcd_D5Group, Lcd_D5Pin, GetBit(nibble, 1));
    mDIO_WritePin(Lcd_D6Group, Lcd_D6Pin, GetBit(nibble, 2));
    mDIO_WritePin(Lcd_D7Group, Lcd_D7Pin, GetBit(nibble, 3));
}

static void LCD_SendByte(uint8_t byte, uint8_t isData)
{
    /**
     * @brief define RS with id data depend on which function we use
     */
    mDIO_WritePin(Lcd_RSGroup, Lcd_RSPin, isData);
    /**
     * @brief define High_Nibble
     */
    uint8_t high_Nibble = ((byte >> 4) & 0x0F);
    LCD_WriteNibble(high_Nibble);
    // Write the High byte Bit(7 6 5 4) on the Data Pin (D7 D6 D5 D4)
    // mDIO_WritePin(Lcd_D4Group, Lcd_D4Pin, GetBit(byte, 4));
    // mDIO_WritePin(Lcd_D5Group, Lcd_D5Pin, GetBit(byte, 5));
    // mDIO_WritePin(Lcd_D6Group, Lcd_D6Pin, GetBit(byte, 6));
    // mDIO_WritePin(Lcd_D7Group, Lcd_D7Pin, GetBit(byte, 7));
    /**
     * @brief Call Pulse Enable
     */
    LCD_PulseEnable();
    /**
     * @brief define Low Nibble
     */
    uint8_t Low_Nibble = (byte & 0x0F);
    LCD_WriteNibble(Low_Nibble);
    // Update the Low byte Bit(3 2 1 0) on the Data Pin (D7 D6 D5 D4)
    // mDIO_WritePin(Lcd_D4Group, Lcd_D4Pin, GetBit(byte, 0));
    // mDIO_WritePin(Lcd_D5Group, Lcd_D5Pin, GetBit(byte, 1));
    // mDIO_WritePin(Lcd_D6Group, Lcd_D6Pin, GetBit(byte, 2));
    // mDIO_WritePin(Lcd_D7Group, Lcd_D7Pin, GetBit(byte, 3));
    /**
     * @brief Call Pulse Enable
     */
    LCD_PulseEnable();
    /**
     * Execution delay wait 50μs or 2ms if the command is clear display or return home
     */
    if ((isData == 0U) && ((byte == LCD_CMD_CLEAR_DISPLAY) || (byte == LCD_CMD_RETURN_HOME)))
    {
        _delay_ms(2);
    }
    else
    {
        _delay_us(50);
    }
}

/**
 * @brief Sends an 8-bit command to the LCD instruction register (RS = LOW).
 * @param cmd HD44780 instruction byte.
 */
void hLCD_WriteCommand(uint8_t cmd)
{
    /**
     * @brief Write RS=0 So we send command to only LCD
     */
    LCD_SendByte(cmd, Lcd_cmd);
}

/**
 * @brief Writes a single ASCII character directly to the LCD at current cursor (RS = HIGH).
 * @param data ASCII character code.
 */
void hLCD_WriteData(uint8_t data)
{
    /**
     * @brief Write RS=1 So we send data to be showen at LCD itself
     */
    LCD_SendByte(data, Lcd_data);
}

/**
 * @brief Performs the complete HD44780 4-bit hardware initialization sequence.
 *        Configures PC0, PC1, PC2, PC4-PC7 as outputs via MCAL DIO.
 */
void hLCD_Init(void)
{
    /**
     * @brief  Initializes the LCD in 4-bit mode.
     * @note   Configures the RS, E, and D4–D7 pins as outputs, then runs the
     *         HD44780 4-bit initialization sequence (function set, display
     *         control, clear display, entry mode).
     */
    mDIO_InitPin(Lcd_RSGroup, Lcd_RSPin, DIO_Output);
    mDIO_InitPin(Lcd_EGroup, Lcd_EPin, DIO_Output);
    mDIO_InitPin(Lcd_D4Group, Lcd_D4Pin, DIO_Output);
    mDIO_InitPin(Lcd_D5Group, Lcd_D5Pin, DIO_Output);
    mDIO_InitPin(Lcd_D6Group, Lcd_D6Pin, DIO_Output);
    mDIO_InitPin(Lcd_D7Group, Lcd_D7Pin, DIO_Output);

    /**
     * @brief Set the RS & EN =0 and delay to Initialization and power up the LCD
     */
    mDIO_WritePin(Lcd_RSGroup, Lcd_RSPin, Dio_Low);
    mDIO_WritePin(Lcd_EGroup, Lcd_EPin, Dio_Low);
    _delay_ms(50);

    /**
     * @brief Issue 8-bit wake-up sequence using raw nibbles
     */
    LCD_WriteNibble(Lcd_Wake_up);
    LCD_PulseEnable();
    _delay_ms(5);
    LCD_WriteNibble(Lcd_Wake_up);
    LCD_PulseEnable();
    _delay_us(200);
    LCD_WriteNibble(Lcd_Wake_up);
    LCD_PulseEnable();
    _delay_us(200);
    LCD_WriteNibble(Lcd_switch_4bitmode);
    LCD_PulseEnable();
    _delay_us(200);

    /**
     * @brief Issue standard 4-bit configuration commands
     */
    hLCD_WriteCommand(LCD_CMD_4BIT_2LINE);
    hLCD_WriteCommand(LCD_CMD_DISPLAY_ON);
    hLCD_WriteCommand(LCD_CMD_CLEAR_DISPLAY);
    hLCD_WriteCommand(LCD_CMD_ENTRY_MODE);

    /**
     * @brief Initialize g_lcdShadow array fill all 4 lines with spaces
     */
    for (uint8_t row = 0U; row < LCD_ROWS; row++)
    {
        for (uint8_t column = 0U; column < LCD_COLS; column++)
        {
            g_lcdShadow[row][column] = ' ';
        }
        g_lcdShadow[row][LCD_COLS] = '\0';
    }
    g_lcdDirtyFlags = 0;
}

/**
 * @brief Sets the hardware cursor position.
 * @param row Display row (0 to 3).
 * @param col Display column (0 to 19).
 * @return Std_ReturnType EOk if coordinates valid, ENotOk if out of bounds.
 */
Std_ReturnType hLCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t rowAddress = 0;
    if (row >= LCD_ROWS || col >= LCD_COLS)
    {
        return ENotOk;
    }
    switch (row)
    {
    case FirstLine:
        rowAddress = Lcd_FirstLineAddress;
        break;
    case SecondLine:
        rowAddress = Lcd_SecondLineAddress;
        break;
    case ThirdLine:
        rowAddress = Lcd_ThirdLineAddress;
        break;
    case FourthLine:
        rowAddress = Lcd_FourthLineAddress;
        break;
    }
    uint8_t target_command = LCD_CMD_SET_DDRAM_ADDR | (rowAddress + col);
    hLCD_WriteCommand(target_command);
    return EOk;
}

/**
 * @brief Writes a string into the shadow buffer at specified row and col.
 *        Sets the dirty flag for the line if text changed.
 * @param row Display row (0 to 3).
 * @param col Display column (0 to 19).
 * @param str Null-terminated string to write.
 * @return Std_ReturnType EOk if successful, ENotOk if parameters invalid.
 */
Std_ReturnType hLCD_WriteString(uint8_t row, uint8_t col, const char *str)
{
    if (row >= LCD_ROWS || col >= LCD_COLS || str == Null)
    {
        return ENotOk;
    }
    while ((*str != '\0') && (col < LCD_COLS))
    {
        if (g_lcdShadow[row][col] != *str)
        {
            g_lcdShadow[row][col] = *str;
            g_lcdDirtyFlags |= (1U << row);
        }
        str++;
        col++;
    }
    return EOk;
}

/**

 * @brief Clears the shadow buffer (fills with spaces) and marks all lines dirty.
 *        Does NOT execute hardware command 0x01 to prevent screen flicker.
 */
void hLCD_Clear(void)
{
    for (uint8_t row = 0U; row < LCD_ROWS; row++)
    {
        for (uint8_t column = 0U; column < LCD_COLS; column++)
        {
            g_lcdShadow[row][column] = ' ';
        }
        g_lcdShadow[row][LCD_COLS] = '\0';
    }
    g_lcdDirtyFlags = Lcd_CLR_Dirtylines;
}

/**
 * @brief Flushes modified shadow buffer lines to the physical LCD hardware.
 *        Called periodically by the OS scheduler (e.g. every 200 ms).
 */
void hLCD_UpdateDirtyLines(void)
{
    if (g_lcdDirtyFlags == 0U)
    {
        return;
    }
    for (uint8_t r = 0U; r < LCD_ROWS; r++)
    {
        if ((g_lcdDirtyFlags & (1U << r)) != 0U)
        {
            hLCD_SetCursor(r, 0);
            for (uint8_t col = 0U; col < LCD_COLS; col++)
            {
                hLCD_WriteData((uint8_t)g_lcdShadow[r][col]);
            }
            g_lcdDirtyFlags &= ~(1U << r);
        }
    }
}

/**
 * @brief Manually marks a specific line as dirty to force a hardware redraw.
 * @param row Row index (0 to 3).
 */
void hLCD_SetDirty(uint8_t row)
{
    if (row < LCD_ROWS)
    {
        g_lcdDirtyFlags |= (1U << row);
    }
}

/**
 * @brief Checks if a specific line currently has pending modifications.
 * @param row Row index (0 to 3).
 * @return bool true if dirty, false if unchanged.
 */
uint8_t hLCD_IsDirty(uint8_t row)
{
    if (row >= LCD_ROWS)
    {
        return False;
    }
    return ((g_lcdDirtyFlags & (uint8_t)(1U << row)) != 0U);
}

/**
 * @brief Uploads a custom 5x8 character pattern into LCD CGRAM.
 * @param patternIndex Location in CGRAM (0 to 7).
 * @param charMap Array of 8 byte row bitmasks.
 */
void hLCD_CreateCustomChar(uint8_t patternIndex, const uint8_t charMap[8])
{
    if (patternIndex > 7 || charMap == Null)
    {
        return;
    }
    hLCD_WriteCommand(LCD_CMD_SET_CGRAM_ADDR | (patternIndex << 3));
    for (uint8_t i = 0U; i < 8; i++)
    {
        hLCD_WriteData(charMap[i]);
    }
    hLCD_SetCursor(0, 0);
}

#endif