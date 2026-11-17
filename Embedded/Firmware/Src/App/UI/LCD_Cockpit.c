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


#if (ModuleUiEnabled == Enable)

#include "LCD_Cockpit.h"

static LCD_CockpitControlBlock_t g_cockpit;

static const uint8_t g_customIcons[5][8] = {
    {0x06U, 0x09U, 0x09U, 0x06U, 0x00U, 0x00U, 0x00U, 0x00U}, /* Degree  */
    {0x0EU, 0x11U, 0x15U, 0x15U, 0x11U, 0x11U, 0x1FU, 0x00U}, /* Battery */
    {0x15U, 0x0EU, 0x1BU, 0x04U, 0x1BU, 0x0EU, 0x15U, 0x00U}, /* Fan     */
    {0x04U, 0x0EU, 0x15U, 0x04U, 0x0EU, 0x04U, 0x04U, 0x00U}, /* BT      */
    {0x0EU, 0x11U, 0x01U, 0x06U, 0x01U, 0x00U, 0x04U, 0x00U}  /* WiFi    */
};

void LCD_Cockpit_Init(void)
{
    /* we init the lcd first */
    hLCD_Init();

    /* then we load the icons for each */
    LCD_Cockpit_LoadCustomIcons();
    
    /* fill the lines with spaces and at the end with null terminator*/
    for (uint8_t row = 0 ; row < LCD_COCKPIT_ROWS ; row++)
    {
        for (uint8_t col = 0 ; col < LCD_COCKPIT_COLS ; col++)
        {
            g_cockpit.stagedBuffers[row][col] = ' ';
        }
        g_cockpit.stagedBuffers[row][LCD_COCKPIT_COLS] = '\0';
    }

    /* then we init the g_cockpit values */
    g_cockpit.currentScreenMode = LCD_SCREEN_STANDBY_OFF ;
    g_cockpit.bootPhase = BOOT_ANIM_PHASE_BANNER;
    g_cockpit.totalFramesRendered = 0U;
    g_cockpit.lastRefreshTimestamp_ms = 0U;
    g_cockpit.screenEntryTimestamp_ms = 0U;

    /* write into shadow buffer */


    /* remove the update dirty lines */
    hLCD_UpdateDirtyLines();

}

void LCD_Cockpit_LoadCustomIcons(void)
{
    /* repeat by storing it into the coustom icons*/
    for (uint8_t i = 0U; i < 5U; i++)
    {
        hLCD_CreateCustomChar(i, g_customIcons[i]);
    }
    hLCD_SetCursor(0U, 0U);
}

void LCD_Cockpit_RenderTask_200ms(void)
{
    EcuState_t state = FSM_GetState();
    uint32_t now = Scheduler_GetUptime_ms();

    switch (state)
    {
        case STATE_START:
        {
            g_cockpit.currentScreenMode = LCD_SCREEN_BOOT_SPLASH;
            uint32_t elapsed = now - g_ecuData.stateEntryTimestamp_ms;
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


void LCD_Cockpit_FormatFloat1dp(char* outBuf, uint8_t maxLen, float value)
{
    /* bounds limits */
    if (value < 0.0f)
    {
        value = 0.0f;
    }
    else if (value > 999.9f)
    {
        value = 999.9f;
    }

    uint32_t scaled = (uint32_t)((value * 10.0f) + 0.5f);
    uint16_t whole = (uint16_t)(scaled / 10U);
    uint8_t fraction = (uint8_t)(scaled % 10U);
    
    if (whole < 100)
    {
        snprintf(outBuf, maxLen, "%2u.%1u", (uint8_t)whole, (uint8_t)fraction);
    }
    else 
    {
        snprintf(outBuf, maxLen, "%3u.%1u", (uint8_t)whole, (uint8_t)fraction);
    }
}

void LCD_Cockpit_RenderLiveScreen(void)
{
    const ECUData_t* p = FSM_GetECUData()

    char tempStr[8];
    char battStr[8];

    LCD_Cockpit_FormatFloat1dp(tempStr, (uint8_t)sizeof(tempStr), tempVal);
    LCD_Cockpit_FormatFloat1dp(battStr, (uint8_t)sizeof(battStr), battVal);


}


void LCD_Cockpit_RenderSafeModeScreen(void)
{

}

void LCD_Cockpit_PadField(char* dest, uint8_t startCol, uint8_t width, const char* content)
{

}

void LCD_Cockpit_RenderStandbyScreen(void)
{

}

void LCD_Cockpit_RenderBootScreen(uint32_t elapsed_ms)
{
    if (elapsed_ms < 600U)
    {
        g_cockpit.bootPhase = BOOT_ANIM_PHASE_BANNER;
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[0], 0U, LCD_COCKPIT_COLS, "  GESTELL MINI-ECU  ");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[1], 0U, LCD_COCKPIT_COLS, "FIRMWARE V1.0.0-OFF ");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[2], 0U, LCD_COCKPIT_COLS, "ATMEGA128 @ 16.000M ");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[3], 0U, LCD_COCKPIT_COLS, "BOOTING SYSTEM..... ");
    }
    else if (elapsed_ms < 1400U)
    {
        g_cockpit.bootPhase = BOOT_ANIM_PHASE_SENSORS;
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[0], 0U, LCD_COCKPIT_COLS, "POST: SENSORS CHECK ");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[1], 0U, LCD_COCKPIT_COLS, "LM35 TEMP ADC0: [OK]");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[2], 0U, LCD_COCKPIT_COLS, "BATT VOLT ADC1: [OK]");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[3], 0U, LCD_COCKPIT_COLS, "HARNESS SIG: PASSED ");
    }
    else
    {
        g_cockpit.bootPhase = BOOT_ANIM_PHASE_READY;
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[0], 0U, LCD_COCKPIT_COLS, "POST: ACTUATOR CHECK");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[1], 0U, LCD_COCKPIT_COLS, "PWM FAN PB4: READY  ");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[2], 0U, LCD_COCKPIT_COLS, "SAFETY GUARD: ARMED ");
        LCD_Cockpit_PadField(g_cockpit.stagedBuffers[3], 0U, LCD_COCKPIT_COLS, "SYSTEM READY TO RUN ");
    }
}


void LCD_Cockpit_RenderDiagScreen(void)
{
    LCD_Cockpit_PadField(g_cockpit.stagedBuffers[0], 0U, LCD_COCKPIT_COLS, "MODE: DIAGNOSTIC UDS");
    LCD_Cockpit_PadField(g_cockpit.stagedBuffers[1], 0U, LCD_COCKPIT_COLS, "SNAPSHOT CAPTURED   ");
    LCD_Cockpit_PadField(g_cockpit.stagedBuffers[2], 0U, LCD_COCKPIT_COLS, "STREAMING $DIAG/JSON");
    LCD_Cockpit_PadField(g_cockpit.stagedBuffers[3], 0U, LCD_COCKPIT_COLS, "BT: TX   WiFi: TX   ");
}

void LCD_Cockpit_SetScreenMode(LCD_ScreenMode_t screenMode)
{
    g_cockpit.currentScreenMode = screenMode;
    g_cockpit.screenEntryTimestamp_ms = Scheduler_GetUptime_ms();
}

LCD_ScreenMode_t LCD_Cockpit_GetScreenMode(void)
{
    return g_cockpit.currentScreenMode;
}



#endif