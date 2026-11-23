/**
 * @file      State_START.c
 * @brief     Implementation of ECU STATE_START boot and hardware self-test logic.
 * @details   Executes the 2000 ms Power-On Self-Test (POST) sequence, validates
 *            LM35 (ADC0) and Battery (ADC1) channels across consecutive samples,
 *            verifies LCD and Timer0 PWM readiness, and transitions to StateRun
 *            on success or StateFault on failure.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-21
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "../../Common/Config.h"
#include "../../Common/Definition.h"

#if ModuleFsmEnabled == Enable
#include "State_START.h"
#include "../../Hal/Lcd/Lcd_Interface.h"

static StartControlBlock_t g_startControl;

void State_START_OnEntry(void){

    g_startControl.startEntryTimestamp_ms = Scheduler_GetUptime_ms();
    g_startControl.selfTestComplete = false;
    g_startControl.selfTestPassed = false;
    g_startControl.selfTestStep = 0U;
    
    // Render boot splash screen on LCD:
    hLCD_WriteString(0,0,"  GESTELL MINI-ECU  ");
    hLCD_WriteString(1,0,"SELF-TEST RUNNING...");
    hLCD_WriteString(2,0,"CHECKING SENSORS...");
    hLCD_WriteString(3,0,"SYSTEM V1.0.0-OFFLINE");

}

void State_START_OnRun(void){
    // elapsed = Scheduler_GetUptime_ms() - g_startControl.startEntryTimestamp_ms
    

}
void State_START_OnExit(void){
    //implementation
}

#endif