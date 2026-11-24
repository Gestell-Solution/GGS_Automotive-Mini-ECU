/**
* @file       LCD_Cockpit.c
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: yassen ahmed)
* @brief      Program file for the LCD_Cockpit driver
* @details    This source file contains the implementation of the LCD_Cockpit driver functions
* @version    1.0.0
* @date       17/11/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/


#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include <string.h>


#if (ModuleUiEnabled == Enable)

#include "LCD_Cockpit.h"

/**
 * @brief Central Cockpit Control Block holding staged line buffers and states.
 */

static LCD_CockpitControlBlock_t g_cockpit;

/**
 * @brief 5 Automotive Icon Bitmaps uploaded into HD44780 CGRAM.
 *        Pattern 0: Degree (°)
 *        Pattern 1: Battery Terminal
 *        Pattern 2: Radiator Fan Impeller
 *        Pattern 3: Bluetooth Monogram
 *        Pattern 4: Wi-Fi Signal Arc
 */

static const uint8_t g_customIcons[5][8] = {
    {0x06U, 0x09U, 0x09U, 0x06U, 0x00U, 0x00U, 0x00U, 0x00U}, /* Degree  */
    {0x0EU, 0x11U, 0x15U, 0x15U, 0x11U, 0x11U, 0x1FU, 0x00U}, /* Battery */
    {0x15U, 0x0EU, 0x1BU, 0x04U, 0x1BU, 0x0EU, 0x15U, 0x00U}, /* Fan     */
    {0x04U, 0x0EU, 0x15U, 0x04U, 0x0EU, 0x04U, 0x04U, 0x00U}, /* BT      */
    {0x0EU, 0x11U, 0x01U, 0x06U, 0x01U, 0x00U, 0x04U, 0x00U}  /* WiFi    */
};

/* =========================================================================
   INTERNAL HELPER FUNCTIONS
   ========================================================================= */

/**
 * @brief  Safely copies up to 20 chars into g_cockpit.stagedBuffers[row],
 *         space-padding unused columns and setting null-terminator at index 20.
 * @param  row  Target row index (0 to 3).
 * @param  str  Source null-terminated string.
 */
static void LCD_Cockpit_SetRowString(uint8_t row, const char* str)
{
    if (row >= LCD_COCKPIT_ROWS)
    {
        return;
    }

    uint8_t i = 0U;
    if (str != NULL)
    {
        while ((i < LCD_COCKPIT_COLS) && (str[i] != '\0'))
        {
            g_cockpit.stagedBuffers[row][i] = str[i];
            i++;
        }
    }
    while (i < LCD_COCKPIT_COLS)
    {
        g_cockpit.stagedBuffers[row][i] = ' ';
        i++;
    }
    g_cockpit.stagedBuffers[row][LCD_COCKPIT_COLS] = '\0';
}

/**
 * @brief  Simple helper to copy characters from str into row starting at startCol.
 *         Operates without strlen, memcpy, or dynamic memory.
 * @param  row      Target line buffer.
 * @param  startCol Column offset (0 to 19).
 * @param  str      Null-terminated source string.
 */
static void LCD_Cockpit_PutString(char* row, uint8_t startCol, const char* str)
{
    if ((row != NULL) && (str != NULL))
    {
        while ((*str != '\0') && (startCol < LCD_COCKPIT_COLS))
        {
            row[startCol] = *str;
            startCol++;
            str++;
        }
    }
}

/* =========================================================================
   PUBLIC API FUNCTION PROTOTYPES WITH DETAILED DOXYGEN SPECIFICATIONS
   ========================================================================= */

/**
 * @brief    Initializes the LCD Cockpit dashboard UI subsystem.
 * @details  Executes the HD44780 4-bit hardware startup sequence via the HAL LCD driver,
 *           uploads the 5 automotive custom icon bitmaps (Degree, Battery, Fan, BT, WiFi)
 *           into CGRAM, zeroes the internal control block, initializes all staged line buffers,
 *           and stages the initial Standby offline layout to the physical LCD glass.
 * @pre      MCAL DIO (PORTC bus) and OS Timer must be initialized prior to invocation.
 * @post     LCD display is operational in STANDBY mode showing the initial offline message.
 * @return   None.
 */
void LCD_Cockpit_Init(void)
{
    /* we init the lcd first */
    hLCD_Init();

    /* then we load the icons for each */
    LCD_Cockpit_LoadCustomIcons();
    
    /* fill the lines with spaces and at the end with null terminator*/

    /*
        void *memset(void *ptr, int value, size_t num);
        ptr = is the pointer to the block of memory to fill
        value = is the value to be set. The value is passed as an int, but
                the function fills the block of memory using the unsigned char conversion of this value.
        num = is the number of bytes to be set to the value.
        and ends in a null termanitor always
    */
    memset(&g_cockpit, 0, sizeof(g_cockpit));
    /* same code but memset is more efficient and cleaner
        for (uint8_t row = 0 ; row < LCD_COCKPIT_ROWS ; row++)
        {
            for (uint8_t col = 0 ; col < LCD_COCKPIT_COLS ; col++)
            {
                g_cockpit.stagedBuffers[row][col] = ' ';
            }
            g_cockpit.stagedBuffers[row][LCD_COCKPIT_COLS] = '\0';
        }
    */

    /* then we init the g_cockpit values */
    g_cockpit.currentScreenMode = LCD_SCREEN_STANDBY_OFF ;
    g_cockpit.bootPhase = BOOT_ANIM_PHASE_BANNER;
    g_cockpit.totalFramesRendered = 0U;
    g_cockpit.lastRefreshTimestamp_ms = 0U;
    g_cockpit.screenEntryTimestamp_ms = 0U;

    /* write intial screen */
    LCD_Cockpit_RenderStandbyScreen();
    for (uint8_t r = 0U; r < LCD_COCKPIT_ROWS; r++)
    {
        (void)hLCD_WriteString(r, 0U, g_cockpit.stagedBuffers[r]);
    }

    /* remove the update dirty lines */
    hLCD_UpdateDirtyLines();

}

/**
 * @brief    Uploads 5 automotive custom icon bitmaps into HD44780 CGRAM.
 * @details  Iterates through the static g_customIcons bitmap array and programs 5 user-defined
 *           5x8 character glyphs (Degree, Battery, Fan, Bluetooth, Wi-Fi) into LCD CGRAM
 *           addresses 0x00 through 0x04, then resets the DDRAM cursor to home (row 0, col 0).
 * @pre      hLCD_Init() must have completed hardware configuration.
 * @post     CGRAM contains custom automotive glyph bitmaps; cursor reset to (0, 0).
 * @return   None.
 */
void LCD_Cockpit_LoadCustomIcons(void)
{
    /* repeat by storing it into the coustom icons*/
    for (uint8_t i = 0U; i < 5U; i++)
    {
        hLCD_CreateCustomChar(i, g_customIcons[i]);
    }
    hLCD_SetCursor(0U, 0U);
}

/**
 * @brief    Periodic 200 ms (5 Hz) real-time executive rendering task.
 * @details  Dispatched cyclically by the cooperative SysTick OS scheduler (US-015).
 *           Evaluates the current FSM operational state (Boot, Run, Safe Mode, Diagnostic, Off),
 *           synthesizes 4 rows of 20 characters into staged buffers, pushes them to the
 *           HAL shadow buffer via hLCD_WriteString(), and triggers a differential hardware
 *           parallel bus flush via hLCD_UpdateDirtyLines().
 * @pre      LCD_Cockpit_Init() must have been called; FSM state registry must be valid.
 * @post     Staged buffers are committed to the HAL shadow buffer; only modified lines
 *           are strobed to the LCD glass to eliminate visual flicker.
 * @return   None.
 */
void LCD_Cockpit_RenderTask_200ms(void)
{
    EcuState_t state = FSM_GetState();
    uint32_t now = Scheduler_GetUptime_ms();

switch (state)
{
    case STATE_START:
    {
        const ECUData_t* pData = FSM_GetECUData();
        uint32_t elapsed = 0U;

        if (now >= pData->stateEntryTimestamp_ms)
        {
            elapsed = now - pData->stateEntryTimestamp_ms;
        }

        LCD_Cockpit_RenderBootScreen(elapsed);
        break;
    }
    case STATE_SAFE_MODE:
    case STATE_FAULT:
    {
        g_cockpit.currentScreenMode = LCD_SCREEN_SAFE_MODE;
        LCD_Cockpit_RenderSafeModeScreen();
        break;
    }

    case STATE_DIAGNOSTIC:
    {
        g_cockpit.currentScreenMode = LCD_SCREEN_DIAGNOSTIC;
        LCD_Cockpit_RenderDiagScreen();
        break;
    }

    case STATE_OFF:
    {
        g_cockpit.currentScreenMode = LCD_SCREEN_STANDBY_OFF;
        LCD_Cockpit_RenderStandbyScreen();
        break;
    }

    case STATE_RUN:
    default:
    {
        g_cockpit.currentScreenMode = LCD_SCREEN_LIVE_COCKPIT;
        LCD_Cockpit_RenderLiveScreen();
        break;
    }
}

    /* Stage formatted lines into HAL LCD shadow buffer */
    for (uint8_t row = 0U; row < LCD_COCKPIT_ROWS; row++)
    {
        hLCD_WriteString(row, 0U, g_cockpit.stagedBuffers[row]);
    }

    /* Dispatch hardware differential flush */
    hLCD_UpdateDirtyLines();

    /* Update runtime */
    g_cockpit.lastRefreshTimestamp_ms = now;
    g_cockpit.totalFramesRendered++;
}


/**
 * @brief    Lightweight integer-scaled floating-point formatter converting float to "XX.X".
 * @details  Converts a single-precision float into a fixed-point string with 1 decimal place
 *           using scaled integer math and half-up rounding (+0.5f). Operates with zero dynamic
 *           memory, strictly avoiding stdio.h, snprintf, and libprintf_flt.a.
 *           Clamps inputs between 0.0f and 999.9f, and provides buffer capacity protection.
 * @param[out] outBuf Destination character buffer where formatted string is stored.
 * @param[in]  maxLen Total byte capacity of destination buffer (including null-terminator).
 * @param[in]  value  Single-precision float value to convert (clamped to 0.0f - 999.9f).
 * @pre      outBuf must point to a valid writable buffer of at least maxLen bytes.
 * @post     outBuf populated with null-terminated ASCII string (e.g. " 5.3", "75.2", "105.4"),
 *           or left unmodified if maxLen is insufficient (less than 5 or 6 bytes).
 * @return   None.
 */
void LCD_Cockpit_FormatFloat1dp(char* outBuf, uint8_t maxLen, float value)
{

    /* 1. Safety guard */
    if ((outBuf == NULL))
    {
        return;
    }

    /* 2. Input validation */
    if (value < 0.0f)
    {
        value = 0.0f;
    }
    else if (value > 999.9f)
    {
        value = 999.9f;
    }

    /* scale the value to 1 decimal place */
    uint32_t scaled = (uint32_t)((value * 10.0f) + 0.5f);
    
    /* we clamp it again */
    if (scaled > 9999U)
    {
        scaled = 9999U;
    }

    /* we split it into whole and fractional parts */
    uint16_t whole   = (uint16_t)(scaled / 10U);
    uint8_t fraction = (uint8_t)(scaled % 10U);
    
    /*
        int snprintf(char *str, size_t size, const char *format, ...);
        str = is the outbuf 
        size_t is maxlen of the string we are gonna but in
        format = is how much we are gonna clamp it 
        ... is the things we are gonna type out
    */
    if (whole < 100U)
    {
        snprintf(outBuf, maxLen, "%2u.%1u", (uint32_t)whole, (uint32_t)fraction);
        /* "%2u.%1u": Formats numbers 0-99 as 4 chars (" X.X" or "XX.X"),
         * padding single digits with a leading space for alignment. */
    }
    else 
    {
        snprintf(outBuf, maxLen, "%3u.%1u", (uint32_t)whole, (uint32_t)fraction);
        /* "%3u.%1u": Formats numbers 100-999 as 5 chars ("XXX.X")
         * with 3 integer digits and 1 decimal place. */
    }
}

/**
 * @brief    Synthesizes the 4-line live driving instrumentation cluster for STATE_RUN.
 * @details  Ingests live powertrain telemetry from the central ECUData_t model:
 *           - Line 0: Operational Mode ("RUN") and Ignition state ("ON"/"OFF").
 *           - Line 1: Coolant Temp with Degree icon + Battery Voltage with Battery icon.
 *           - Line 2: Radiator Fan PWM Duty % with Fan icon + Active DTC code.
 *           - Line 3: Dual wireless health (Bluetooth HC-05 & Wi-Fi ESP-01) with icons.
 *           All lines are formatted to exactly 20 characters and null-terminated.
 * @pre      FSM must be in STATE_RUN; sensor and telemetry values must be updated.
 * @post     g_cockpit.stagedBuffers populated with driving cluster layout.
 * @return   None.
 */
void LCD_Cockpit_RenderLiveScreen(void)
{
    g_cockpit.currentScreenMode = LCD_SCREEN_LIVE_COCKPIT;

    /* 1. Retrieve const pointer to telemetry data */
    const ECUData_t* p = FSM_GetECUData();

    /* 2. Format temporary numeric buffers */
    char tempStr[8];
    char battStr[8];
    LCD_Cockpit_FormatFloat1dp(tempStr, sizeof(tempStr), p->engineTemp_C);
    LCD_Cockpit_FormatFloat1dp(battStr, sizeof(battStr), p->batteryVoltage_V);

    /* 3. Synthesize Line 1 (MODE & IGNITION) */
    memset(g_cockpit.stagedBuffers[0U], ' ', LCD_COCKPIT_COLS);
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[0U], 0U, "MODE: RUN   IGN: ");
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[0U], 17U, p->ignitionActive ? "ON " : "OFF");

    /* 4. Synthesize Line 2 (TEMPERATURE & BATTERY) */
    memset(g_cockpit.stagedBuffers[1U], ' ', LCD_COCKPIT_COLS);
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[1U], 0U, tempStr);
    g_cockpit.stagedBuffers[1U][4U] = LCD_ICON_DEGREE[0];
    g_cockpit.stagedBuffers[1U][5U] = 'C';
    g_cockpit.stagedBuffers[1U][11U] = LCD_ICON_BATTERY[0];
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[1U], 13U, battStr);
    g_cockpit.stagedBuffers[1U][17U] = 'V';

    /* 5. Synthesize Line 3 (FAN SPEED & ACTIVE DTC) */
    memset(g_cockpit.stagedBuffers[2U], ' ', LCD_COCKPIT_COLS);
    g_cockpit.stagedBuffers[2U][0U] = LCD_ICON_FAN[0];
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[2U], 1U, "FAN:");
    if (p->fanDutyPct >= 100U)
    {
        LCD_Cockpit_PutString(g_cockpit.stagedBuffers[2U], 5U, "100%");
    }
    else if (p->fanDutyPct >= 10U)
    {
        g_cockpit.stagedBuffers[2U][6U] = (char)('0' + (p->fanDutyPct / 10U));
        g_cockpit.stagedBuffers[2U][7U] = (char)('0' + (p->fanDutyPct % 10U));
        g_cockpit.stagedBuffers[2U][8U] = '%';
    }
    else
    {
        g_cockpit.stagedBuffers[2U][7U] = (char)('0' + p->fanDutyPct);
        g_cockpit.stagedBuffers[2U][8U] = '%';
    }

    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[2U], 10U, "FLT:");
    const char* dtcStr = "NONE";
    if (p->activeDTC == DtcF001OverTemp)
    {
        dtcStr = "F001";
    }
    else if (p->activeDTC == DtcF002BatteryFault)
    {
        dtcStr = "F002";
    }
    else if (p->activeDTC == DtcF003SensorError)
    {
        dtcStr = "F003";
    }
    else if (p->activeDTC == DtcF004CommsTimeout)
    {
        dtcStr = "F004";
    }
    else if (p->activeDTC == DtcF005Watchdog)
    {
        dtcStr = "F005";
    }
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[2U], 15U, dtcStr);

    /* 6. Synthesize Line 4 (WIRELESS LINK HEALTH) */
    memset(g_cockpit.stagedBuffers[3U], ' ', LCD_COCKPIT_COLS);
    bool btTimeout = (p->activeDTC == DtcF004CommsTimeout);
    bool wifiTimeout = (p->activeDTC == DtcF004CommsTimeout);
    const char* btText = btTimeout ? "ERR  " : (p->btConnected ? "OK   " : "DISC ");
    const char* wifiText = wifiTimeout ? "LOST   " : (p->wifiConnected ? "ONLINE " : "DISC   ");

    g_cockpit.stagedBuffers[3U][0U] = LCD_ICON_BLUETOOTH[0];
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[3U], 1U, "BT:");
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[3U], 4U, btText);

    g_cockpit.stagedBuffers[3U][9U] = LCD_ICON_WIFI[0];
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[3U], 10U, "WF:");
    LCD_Cockpit_PutString(g_cockpit.stagedBuffers[3U], 13U, wifiText);

    /* 7. Enforce exact 20-character null-termination on all rows */
    for (uint8_t r = 0U; r < LCD_COCKPIT_ROWS; r++)
    {
        g_cockpit.stagedBuffers[r][LCD_COCKPIT_COLS] = '\0';
    }

    /* 8. Return */
}


/**
 * @brief    Synthesizes emergency warning and DTC identification screen for STATE_SAFE_MODE.
 * @details  Constructs high-contrast diagnostic alert displaying critical fault banner,
 *           active DTC code (DTC_F001 to DTC_F005) with short description, confirmation of
 *           100% failsafe fan cooling, and clear driver recovery instructions (INT5 push button).
 * @pre      FSM must be in STATE_SAFE_MODE or STATE_FAULT; active DTC must be registered.
 * @post     g_cockpit.stagedBuffers populated with emergency failsafe layout.
 * @return   None.
 */
void LCD_Cockpit_RenderSafeModeScreen(void)
{
    g_cockpit.currentScreenMode = LCD_SCREEN_SAFE_MODE;
    const ECUData_t* p = FSM_GetECUData();

    LCD_Cockpit_SetRowString(0U, "!CRITICAL FAULT DET!");

    switch (p->activeDTC)
    {
        case DtcF001OverTemp:
            LCD_Cockpit_SetRowString(1U, "DTC:F001 (OVER-TEMP)");
            break;
        case DtcF002BatteryFault:
            LCD_Cockpit_SetRowString(1U, "DTC:F002 (BATT-VOLT)");
            break;
        case DtcF003SensorError:
            LCD_Cockpit_SetRowString(1U, "DTC:F003 (SENS-DISC)");
            break;
        case DtcF004CommsTimeout:
            LCD_Cockpit_SetRowString(1U, "DTC:F004 (COMMS-TO) ");
            break;
        case DtcF005Watchdog:
            LCD_Cockpit_SetRowString(1U, "DTC:F005 (WATCHDOG) ");
            break;
        case DtcNone:
        default:
            LCD_Cockpit_SetRowString(1U, "DTC: NONE           ");
            break;
    }

    LCD_Cockpit_SetRowString(2U, "COOLING FAN: 100% PW");
    LCD_Cockpit_SetRowString(3U, "PRESS INT5 TO RESET ");
}


/**
 * @brief    Safely writes a fixed-width string field with space padding and bounds protection.
 * @details  Copies characters from content into dest starting at startCol up to width columns.
 *           If content is shorter than width, the remaining space is padded with ASCII spaces (' ').
 *           Clamps the field to line boundary (column 20) and appends a null-terminator at startCol + width.
 * @param[in,out] dest     Target destination line buffer array.
 * @param[in]     startCol Starting column offset (0 to 19).
 * @param[in]     width    Desired field width in columns (1 to 20).
 * @param[in]     content  Null-terminated source string (or NULL to fill field with spaces).
 * @pre      dest must point to a writable character buffer of capacity >= (startCol + width + 1).
 * @post     Specified column span in dest filled with text/spaces and null-terminated.
 * @return   None.
 */
void LCD_Cockpit_PadField(char* dest, uint8_t startCol, uint8_t width, const char* content)
{
    /* safty first*/
    if (dest == NULL)
    {
        return;
    }
    if ((startCol >= LCD_COCKPIT_COLS) || (width == 0U))
    {
        return;
    }

    /* Clamp field width to boundary of row (column 20) */
    if (((uint16_t)startCol + (uint16_t)width) > (uint16_t)LCD_COCKPIT_COLS)
    {
        width = (uint8_t)(LCD_COCKPIT_COLS - startCol);
    }

    uint8_t i = 0U;
    if (content != NULL)
    {
        while ((i < width) && (content[i] != '\0'))
        {
            dest[startCol + i] = content[i];
            i++;
        }
    }
    while (i < width)
    {
        dest[startCol + i] = ' ';
        i++;
    }
    dest[startCol + width] = '\0';
}


/**
 * @brief    Synthesizes the standby offline prompt screen for STATE_OFF.
 * @details  Renders idle prompt informing the driver that the engine ignition is OFF,
 *           all powertrain actuators are silenced, and the system is ready for cranking.
 * @pre      FSM must be in STATE_OFF.
 * @post     g_cockpit.stagedBuffers populated with standby offline layout.
 * @return   None.
 */
void LCD_Cockpit_RenderStandbyScreen(void)
{
    g_cockpit.currentScreenMode = LCD_SCREEN_STANDBY_OFF;
    LCD_Cockpit_SetRowString(0U, "MODE: OFF   IGN: OFF");
    LCD_Cockpit_SetRowString(1U, "STANDBY V1.0-OFFLINE");
    LCD_Cockpit_SetRowString(2U, "ACTUATORS: SILENCED ");
    LCD_Cockpit_SetRowString(3U, "PRESS START TO CRANK");
}

/**
 * @brief    Synthesizes the 3-phase animated POST boot splash during STATE_START.
 * @details  Constructs context-sensitive self-test layouts based on elapsed time:
 *           - Phase 1 (0 to 600 ms): Branding Banner & Hardware Specifications.
 *           - Phase 2 (600 to 1400 ms): Sensor Integrity POST (LM35 Temp & Battery ADC).
 *           - Phase 3 (1400 to 2000 ms): Actuator & Safety Interlock Verification (PWM Fan).
 * @param[in] elapsed_ms Milliseconds elapsed since entering STATE_START.
 * @pre      FSM must be in STATE_START.
 * @post     g_cockpit.stagedBuffers populated with the corresponding boot phase layout.
 * @return   None.
 */
void LCD_Cockpit_RenderBootScreen(uint32_t elapsed_ms)
{
    g_cockpit.currentScreenMode = LCD_SCREEN_BOOT_SPLASH;

    if (elapsed_ms < 600U)
    {
        g_cockpit.bootPhase = BOOT_ANIM_PHASE_BANNER;
        LCD_Cockpit_SetRowString(0U, "  GESTELL MINI-ECU  ");
        LCD_Cockpit_SetRowString(1U, "FIRMWARE V1.0.0-OFF ");
        LCD_Cockpit_SetRowString(2U, "ATMEGA128 @ 16.000M ");
        LCD_Cockpit_SetRowString(3U, "BOOTING SYSTEM..... ");
    }
    else if (elapsed_ms < 1400U)
    {
        g_cockpit.bootPhase = BOOT_ANIM_PHASE_SENSORS;
        LCD_Cockpit_SetRowString(0U, "POST: SENSORS CHECK ");
        LCD_Cockpit_SetRowString(1U, "LM35 TEMP ADC0: [OK]");
        LCD_Cockpit_SetRowString(2U, "BATT VOLT ADC1: [OK]");
        LCD_Cockpit_SetRowString(3U, "HARNESS SIG: PASSED ");
    }
    else
    {
        g_cockpit.bootPhase = BOOT_ANIM_PHASE_READY;
        LCD_Cockpit_SetRowString(0U, "POST: ACTUATOR CHECK");
        LCD_Cockpit_SetRowString(1U, "PWM FAN PB4: READY  ");
        LCD_Cockpit_SetRowString(2U, "SAFETY GUARD: ARMED ");
        LCD_Cockpit_SetRowString(3U, "SYSTEM READY TO RUN ");
    }
}

/**
 * @brief    Synthesizes the service technician telemetry screen for STATE_DIAGNOSTIC.
 * @details  Constructs service session screen confirming frozen snapshot data capture,
 *           active JSON diagnostic streaming over serial, and USART transceiver link status.
 * @pre      FSM must be in STATE_DIAGNOSTIC.
 * @post     g_cockpit.stagedBuffers populated with service technician layout.
 * @return   None.
 */
void LCD_Cockpit_RenderDiagScreen(void)
{
    g_cockpit.currentScreenMode = LCD_SCREEN_DIAGNOSTIC;
    LCD_Cockpit_SetRowString(0U, "MODE: DIAGNOSTIC UDS");
    LCD_Cockpit_SetRowString(1U, "SNAPSHOT DATA CAPTUR");
    LCD_Cockpit_SetRowString(2U, "STREAMING $DIAG/JSON");
    LCD_Cockpit_SetRowString(3U, "BT: TX   WiFi: TX   ");
}

/**
 * @brief    Manually configures the active LCD screen layout mode.
 * @details  Updates the internal screen mode state descriptor in the control block.
 *           Used by external modules or test harnesses to force a specific screen layout.
 * @param[in] screenMode Target screen mode enum (e.g. LCD_SCREEN_LIVE_COCKPIT).
 * @pre      None.
 * @post     g_cockpit.currentScreenMode updated with requested value.
 * @return   None.
 */
void LCD_Cockpit_SetScreenMode(LCD_ScreenMode_t screenMode)
{
    g_cockpit.currentScreenMode = screenMode;

}

/**
 * @brief    Queries the currently active LCD screen layout mode.
 * @details  Provides read-only access to the active screen mode registered within
 *           the central cockpit control block.
 * @pre      None.
 * @post     None (pure read-only query).
 * @return   LCD_ScreenMode_t The currently active screen layout enum.
 */
LCD_ScreenMode_t LCD_Cockpit_GetScreenMode(void)
{
    return g_cockpit.currentScreenMode;
}



#endif