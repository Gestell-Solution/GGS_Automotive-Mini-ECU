/**
* @file       LCD_Cockpit.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: yassen ahmed)
* @brief      Program file for the LCD_Cockpit driver
* @details    This source file contains the implementation of the LCD_Cockpit driver functions
* @version    1.0.0
* @date       17/11/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

/* Include necessary headers */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* common libararys */
#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include "../../Common/MacroFunction.h"

/* Driver-specific headers */
#include "BATT_Private.h"
#include "BATT_Config.h" 

/* calling lower level layers APIs*/
#include "../../Mcal/Adc/ADC_Interface.h"
#include "../../Hal/Sensors/Batt/BATT_Interface.h"
#include "../../Hal/Lcd/Lcd_Interface.h"



#ifndef LCD_COCKPIT_H_
#define LCD_COCKPIT_H_

#define LCD_COCKPIT_COLS                20U
#define LCD_COCKPIT_ROWS                4U

#define LCD_ICON_DEGREE                 "\x00"
#define LCD_ICON_BATTERY                "\x01"
#define LCD_ICON_FAN                    "\x02"
#define LCD_ICON_BLUETOOTH              "\x03"
#define LCD_ICON_WIFI                   "\x04"

typedef enum {
    LCD_SCREEN_BOOT_SPLASH  = 0U,
    LCD_SCREEN_LIVE_COCKPIT = 1U,
    LCD_SCREEN_STANDBY_OFF  = 2U,
    LCD_SCREEN_SAFE_MODE    = 3U,
    LCD_SCREEN_DIAGNOSTIC   = 4U
} LCD_ScreenMode_t;

typedef enum {
    BOOT_ANIM_PHASE_BANNER  = 0U,
    BOOT_ANIM_PHASE_SENSORS = 1U,
    BOOT_ANIM_PHASE_READY   = 2U
} LCD_BootAnimPhase_t;

typedef struct {
    char                stagedBuffers[LCD_COCKPIT_ROWS][LCD_COCKPIT_COLS + 1U];
    LCD_ScreenMode_t    currentScreenMode;
    LCD_BootAnimPhase_t bootPhase;
    uint32_t            lastRefreshTimestamp_ms;
    uint32_t            screenEntryTimestamp_ms;
    uint32_t            totalFramesRendered;
} LCD_CockpitControlBlock_t;

void           LCD_Cockpit_Init(void);
void           LCD_Cockpit_RenderTask_200ms(void);
void           LCD_Cockpit_SetScreenMode(LCD_ScreenMode_t screenMode);
LCD_ScreenMode_t LCD_Cockpit_GetScreenMode(void);
void           LCD_Cockpit_LoadCustomIcons(void);
void           LCD_Cockpit_FormatFloat1dp(char* outBuf, uint8_t maxLen, float value);
void           LCD_Cockpit_PadField(char* dest, uint8_t startCol, uint8_t width, const char* content);
void           LCD_Cockpit_RenderLiveScreen(void);
void           LCD_Cockpit_RenderBootScreen(uint32_t elapsed_ms);
void           LCD_Cockpit_RenderSafeModeScreen(void);
void           LCD_Cockpit_RenderDiagScreen(void);

#endif /* LCD_COCKPIT_H_ */
