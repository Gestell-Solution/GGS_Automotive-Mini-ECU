/**
 * @file    Lcd_Config.h
 * @author (Developer: Abdulrahman Ali)
 * @author (Reviewer:Beshoy Esmat)
 * @brief  Configuration file for the Lcd driver
 * @details This header file defines the settings of the Lcd Driver
 * @version 1.0.0
 * @date 25/10/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */
#ifndef LCD_CONFIG_H
#define LCD_CONFIG_H

/**
 * @brief   Number of rows (lines) of the LCD.
 * @details For a 20x4 LCD this is 4. Valid row indices are 0 to
 *          (LCD_ROWS - 1).
 */
#define LCD_ROWS 4U

/**
 * @brief   Number of columns (characters per line) of the LCD.
 * @details For a 20x4 LCD this is 20. Valid column indices are 0 to
 *          (LCD_COLS - 1).
 */
#define LCD_COLS 20U

/**
 * @brief Register Select (0 = Command, 1 = Data)
 */
#define Lcd_RSGroup GroupC
#define Lcd_RSPin Pin0
// /**
//  * @brief Read/Write control (Driven LOW for Write-Only)
//  */
// #define Lcd_RWGroup GroupC
// #define Lcd_RWPin Pin1
/**
 * @brief Enable strobe (Falling-edge data latch)
 */
#define Lcd_EGroup GroupC
#define Lcd_EPin Pin1

/**
 * @authors Define LCD Data Group
 */
#define Lcd_DataGroup GroupC

/**
 * @brief Data Bit 4 (High nibble bit 0 / Low nibble bit 0)
 */
#define Lcd_D4Group GroupC
#define Lcd_D4Pin Pin4
/**
 * @brief Data Bit 5
 */
#define Lcd_D5Group GroupC
#define Lcd_D5Pin Pin5
/**
 * @brief Data Bit 6
 */
#define Lcd_D6Group GroupC
#define Lcd_D6Pin Pin6
/**
 * @brief Data Bit 7 (Most Significant Bit)
 */
#define Lcd_D7Group GroupC
#define Lcd_D7Pin Pin7

#endif