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


#ifndef LCD_COCKPIT_H_
#define LCD_COCKPIT_H_


/* Include necessary headers */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* common libararys */
#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include "../../Common/MacroFunction.h"


/* calling lower level layers APIs*/
#include "../../Mcal/Adc/ADC_Interface.h"
#include "../../Hal/Sensors/Batt/BATT_Interface.h"
#include "../../Hal/Lcd/Lcd_Interface.h"
#include "../../OS/Schedular/Scheduler_Interface.h"
#include "../FSM/CORE/FSM_Interface.h"
#include "../DTC/DTC_Manager_Interface.h"

/* Configuration parameters */

#define LCD_COCKPIT_COLS                20U
#define LCD_COCKPIT_ROWS                4U

#define LCD_ICON_DEGREE                 "\x08"
#define LCD_ICON_BATTERY                "\x01"
#define LCD_ICON_FAN                    "\x02"
#define LCD_ICON_BLUETOOTH              "\x03"
#define LCD_ICON_WIFI                   "\x04"

/**
 * @enum  LCD_ScreenMode_t
 * @brief Discrete layout screens arbitrated by active FSM operational states.
 */

typedef enum {
    LCD_SCREEN_BOOT_SPLASH  = 0U,
    LCD_SCREEN_LIVE_COCKPIT = 1U,
    LCD_SCREEN_STANDBY_OFF  = 2U,
    LCD_SCREEN_SAFE_MODE    = 3U,
    LCD_SCREEN_DIAGNOSTIC   = 4U
} LCD_ScreenMode_t;

/**
 * @enum  LCD_BootAnimPhase_t
 * @brief Three distinct timed animation stages across the 2000 ms boot window.
 */

typedef enum {
    BOOT_ANIM_PHASE_BANNER  = 0U,
    BOOT_ANIM_PHASE_SENSORS = 1U,
    BOOT_ANIM_PHASE_READY   = 2U
} LCD_BootAnimPhase_t;


/**
 * @struct LCD_CockpitControlBlock_t
 * @brief  Staged line buffers and internal state descriptors.
 * @note   Total size guaranteed <= 120 bytes in .bss (AC-7 compliance).
 */


typedef struct {
    char                stagedBuffers[LCD_COCKPIT_ROWS][LCD_COCKPIT_COLS + 1U]; /**< 4 lines of 20 chars + null */
    LCD_ScreenMode_t    currentScreenMode;                                      /**< Active screen layout */
    LCD_BootAnimPhase_t bootPhase;                                              /**< Boot animation phase */
    uint32_t            lastRefreshTimestamp_ms;                                /**< Last 200ms task execution */
    uint32_t            screenEntryTimestamp_ms;                                /**< Screen entry timestamp */
    uint32_t            totalFramesRendered;                                    /**< Cyclic render count */
} LCD_CockpitControlBlock_t;


/* =========================================================================
   PUBLIC API FUNCTION PROTOTYPES
   ========================================================================= */

/**
 * @brief  Initializes the LCD hardware via HAL, loads custom CGRAM icons,
 *         sets default mode to STANDBY, and renders initial stand-alone message.
 */
void LCD_Cockpit_Init(void);

/**
 * @brief  Periodic 200ms real-time task dispatched by cooperative scheduler.
 *         Arbitrates screen mode based on FSM state, formats 4 lines, and flushes dirty rows.
 */
void LCD_Cockpit_RenderTask_200ms(void);

/**
 * @brief  Manually sets the active screen mode.
 * @param  screenMode Target screen mode enum.
 */
void LCD_Cockpit_SetScreenMode(LCD_ScreenMode_t screenMode);

/**
 * @brief  Queries the active screen mode.
 * @return LCD_ScreenMode_t Current screen mode.
 */
LCD_ScreenMode_t LCD_Cockpit_GetScreenMode(void);


/**
 * @brief  Uploads 5 automotive custom icon bitmaps (Degree, Battery, Fan, BT, WiFi) into CGRAM.
 */
void LCD_Cockpit_LoadCustomIcons(void);

/**
 * @brief  Lightweight integer division/modulo formatter converting float into "XX.X"
 *         without linking floating-point printf or stdio.h.
 * @param  outBuf Destination character buffer.
 * @param  maxLen Maximum buffer capacity in bytes.
 * @param  value  Floating-point value to format.
 */
void LCD_Cockpit_FormatFloat1dp(char* outBuf, uint8_t maxLen, float value);

/**
 * @brief  Copies content string into dest buffer starting at startCol up to width chars,
 *         padding unused space with ASCII spaces (' ') and null-terminating at startCol + width.
 * @param  dest     Destination character buffer.
 * @param  startCol Starting column offset (0 to 19).
 * @param  width    Desired field width in columns.
 * @param  content  Null-terminated source string (or NULL to fill with spaces).
 */
void LCD_Cockpit_PadField(char* dest, uint8_t startCol, uint8_t width, const char* content);

/**
 * @brief  Synthesizes the 4-line live instrumentation cluster for STATE_RUN.
 */
void LCD_Cockpit_RenderLiveScreen(void);

/**
 * @brief  Synthesizes the multi-phase animated boot sequence during STATE_START.
 * @param  elapsed_ms Milliseconds elapsed since entering STATE_START.
 */
void LCD_Cockpit_RenderBootScreen(uint32_t elapsed_ms);

/**
 * @brief  Synthesizes the emergency warning and DTC identification screen for STATE_SAFE_MODE.
 */
void LCD_Cockpit_RenderSafeModeScreen(void);

/**
 * @brief  Synthesizes the service technician telemetry screen for STATE_DIAGNOSTIC.
 */
void LCD_Cockpit_RenderDiagScreen(void);

/**
 * @brief  Synthesizes the standby offline prompt screen for STATE_OFF.
 */
void LCD_Cockpit_RenderStandbyScreen(void);


#endif /* LCD_COCKPIT_H_ */
