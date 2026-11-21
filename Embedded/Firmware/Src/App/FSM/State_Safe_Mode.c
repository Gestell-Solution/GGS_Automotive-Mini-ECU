/**
 * @file      State_Safe_Mode_Program.c
 * @author    (Developer: Abdulrahman Ali)
 * @author    (Reviewer:Beshoy Esmat)
 * @brief     Implementation for the Safe Mode state.
 * @details   This source file contains the implementation for the Safe Mode state
 *  including actuator lockdown handling, dual-barrier recovery logic, and EEPROM logging.
 * @version   1.0.0
 * @date      22/11/2026
 * @copyright Copyright (c) 2026, Gestell Company
 */
#include "../../Common/Config.h"
#include "../../Common/Definition.h"
#include "State_Safe_Mode.h"

#if ModuleFsmEnabled == Enable

static SafeModeControlBlock_t g_safeModeControl;


/**
 * @brief   Entry handler for STATE_FAULT.
 * @details Latches the active DTC, captures entry timestamp, enforces emergency
 *          actuator fail-safes, and alerts the user on LCD.
 */
void State_FAULT_OnEntry(void)
{
    /**
     * Upon entering STATE_FAULT, the system immediately records the current
     */
    g_safeModeControl.faultEntryTimestamp_ms = Scheduler_GetUptime_ms();
    /**
     * The active Diagnostic Trouble Code (DTC) is latched into the control block
     */
    g_safeModeControl.lockedDTC = g_ecuData.activeDTC;
    /**
     * Sending the latched DTC to EEPROM for persistent logging, along with the current uptime in seconds.
     */
    DTC_Log(g_safeModeControl.lockedDTC, Scheduler_GetUptime_s());
    /**
     * Turning on the buzzer and red warning LED to alert the user of the fault condition.
     */
    hBUZZER_SetState(BUZZER_STATE_ON);
    hLED_SetState(LED_WARNING_YELLOW, LED_STATE_ON);
    
}

/**
 * @brief   Cyclic execution handler for STATE_FAULT.
 * @details Monitors critical fault persistence and transitions the FSM into
 *          STATE_SAFE_MODE once immediate stabilization completes.
 */
void State_FAULT_OnRun(void)
{
    /**
     * The system Auto transitions to STATE_SAFE_MODE after a 5-second stabilization period in STATE_FAULT.
     */
    FSM_Transition(StateSafeMode);
    
}

/**
 * @fn      void State_FAULT_OnExit(void)
 * @brief   Exit handler for STATE_FAULT.
 * @details Performs cleanup actions prior to transitioning out of the immediate fault state.
 */
void State_FAULT_OnExit(void){
    return;
}

/**
 * @brief   Entry handler for STATE_SAFE_MODE.
 * @details Configures degraded limp-home actuator limits and updates LCD status
 *          to indicate Safe Mode operation.
 */
void State_SAFE_MODE_OnEntry(void)
{
    /**
     * Enforce emergency cooling
     */
    hFAN_ForceMax();
    /**
     * Enforce warning indication
     */
    hLED_Blink(LED_WARNING_YELLOW, 250U);
    hBUZZER_SetBeep(BUZZER_MODE_BEEP);
    hLED_SetState(LED_STATUS_BLUE, LED_STATE_OFF);
    /**
     * TODO: Update LCD cockpit display
     */
    hLCD_WriteString(FirstLine, 0, "MODE: SAFE | ALARM!");
    hLCD_WriteString(SecondLine, 0, "COOLING AT 100%");
    hLCD_WriteString(ThirdLine, 0, "DTC: F00%d ACTIVE");
    hLCD_WriteString(FourthLine, 0, "PRESS RESET TO CLEAR");
    /**
     * Clear recovery flags
     */
    g_safeModeControl.recoveryConditionsMet = false;
    g_safeModeControl.resetSignalReceived = false;
    
}

/**
 * @brief   Cyclic execution handler for STATE_SAFE_MODE.
 * @details Evaluates sensor recovery thresholds and listens for hardware reset
 *          (PE5 / INT5) or wireless CLEAR_DTC commands to restore normal drive mode.
 */
void State_SAFE_MODE_OnRun(void){
    /**
     * Check Barrier 1 (Physical Parameter Recovery with Hysteresis)
     */
    if(g_safeModeControl.lockedDTC == DtcF001OverTemp){
        if(g_ecuData.engineTemp_C < SAFE_MODE_TEMP_RECOVERY_C){
            g_safeModeControl.recoveryConditionsMet = true;
        }
        else 
        {
            g_safeModeControl.recoveryConditionsMet = false;
        }
    }
    /**
     * check on BATT Voltage Recovery with Hysteresis
     */
    if(g_safeModeControl.lockedDTC == DtcF002BatteryFault){
        if(g_ecuData.batteryVoltage_V >= 12.0f && g_ecuData.batteryVoltage_V <= 14.0f){
            g_safeModeControl.recoveryConditionsMet = true;
        }
        else 
        {
            g_safeModeControl.recoveryConditionsMet = false;
        }
    }
    /**
     * TODO: Check Barrier 2 (Reset Signal Received)
     */
    EventMask_t evt = Event_Consume(EVENT_FAULT_RESET);
    if(evt & EVENT_FAULT_RESET){
        g_safeModeControl.resetSignalReceived = true;
    }
    /**
     * If both recovery conditions are met, transition back to STATE_RUN
     */
    if(g_safeModeControl.recoveryConditionsMet == true && g_safeModeControl.resetSignalReceived == true){
        DTC_Clear(g_safeModeControl.lockedDTC);
        FSM_Transition(StateOff);
    }
}

/**
 * @brief   Exit handler for STATE_SAFE_MODE.
 * @details Clears latched fault flags in the control block and restores normal
 *          operational LCD indicators upon returning to STATE_RUN.
 */
void State_SAFE_MODE_OnExit(void){
    /**
     * Silence emergency alarms
     */
    hBUZZER_SetState(BUZZER_STATE_OFF);
    hLED_SetState(LED_WARNING_YELLOW, LED_STATE_OFF);
    /**
     * Return cooling fan control from failsafe to normal mode
     */
    hFAN_Stop();
    /**
     * Clear internal control block
     */
    g_safeModeControl.lockedDTC = DtcNone;
}

/**
 * @brief   Checks whether all hardware and environmental conditions allow exiting Safe Mode.
 * @details Verifies that engine temperature is <= SAFE_MODE_TEMP_RECOVERY_C,
 *          battery voltage is within [SAFE_MODE_BATT_MIN_RECOVERY_V, SAFE_MODE_BATT_MAX_RECOVERY_V],
 *          and a valid reset signal (EVENT_FAULT_RESET / CLEAR_DTC) has been received.
 */
uint8_t State_SAFE_MODE_IsRecoveryAllowed(void){
    /**
     * Check if both recovery conditions are met: physical parameter recovery and reset signal received
     * Returns 1 (true) if both conditions are satisfied, otherwise returns 0 (false).
     */
    if ((g_safeModeControl.recoveryConditionsMet == True) && (g_safeModeControl.resetSignalReceived == True)) 
    {
        return True;
    }
    else
    {
    return False;
    }
}

#endif