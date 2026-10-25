/**
 * @file      Lcd_Private.h
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Beshoy Esmat)
 * @brief     Private definitions for the Lcd driver.
 * @details   This header file defines the Lcd commands sets ,DDRAM row address and prototyoes of Lcd Driver
 * @version   1.0.0
 * @date      25/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */
#ifndef LCD_PRIVATE_H
#define LCD_PRIVATE_H

/**
 * @enum LcdCommand_t
 * @brief Standard HD44780 instruction set definitions.
 */
typedef enum
{
    LCD_CMD_CLEAR_DISPLAY = 0x01U,  /**< Clears display and homes cursor (2ms delay) */
    LCD_CMD_RETURN_HOME = 0x02U,    /**< Sets DDRAM address 0 and returns cursor */
    LCD_CMD_ENTRY_MODE = 0x06U,     /**< Increment cursor, no display shift */
    LCD_CMD_DISPLAY_ON = 0x0CU,     /**< Display ON, Cursor OFF, Blink OFF */
    LCD_CMD_DISPLAY_OFF = 0x08U,    /**< Display OFF */
    LCD_CMD_4BIT_2LINE = 0x28U,     /**< 4-bit bus, 2-line/4-line mode, 5x8 font */
    LCD_CMD_SET_DDRAM_ADDR = 0x80U, /**< Set DDRAM address base mask */
    LCD_CMD_SET_CGRAM_ADDR = 0x40U  /**< Set CGRAM address base mask */
} LcdCommand_t;

/**
 * @brief Define Lines
 */
#define FirstLine  0
#define SecondLine 1
#define ThirdLine  2
#define FourthLine 3

/**
 * @brief define Lines address
 */
#define Lcd_FirstLineAddress  0X00U
#define Lcd_SecondLineAddress 0X40U
#define Lcd_ThirdLineAddress  0X14U
#define Lcd_FourthLineAddress 0X54U

/**
 * @brief Clears the all dirty lines
 */
#define Lcd_CLR_Dirtylines  0X0FU

/**
 * @brief Setting Command and data for LCD 
 */
#define Lcd_cmd  0
#define Lcd_data 1


/**
 * @brief Issue 8-bit wake-up sequence using raw nibbles
 */
#define Lcd_Wake_up   0X03U
#define Lcd_switch_4bitmode 0X02U

#endif