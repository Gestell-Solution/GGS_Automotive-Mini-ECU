/**
 * @file      FSM_Program.c
 * @brief     FSM driver Implementations.
 * @details
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Abdulrahman Ali
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#if ModuleFsmEnabled == Enable

#include "FSM_Interface.h"
#include "FSM_Config.h"
#include "../../Common/Definition.h"
#include "../../Mcal/Atmega128Register.h"
#include <avr/interrupt.h>

ECUData_t g_ecuData;
static const FSM_StateDescriptor_t g_fsmStateTable[FSM_NUM_STATES];

void FSM_Init(void)
{
    // Zero all fields in g_ecuData and set intial configurations
    g_ecuData = (ECUData_t){0};
    g_ecuData.currentState = STATE_OFF;
    g_ecuData.previousState = STATE_OFF;
    g_ecuData.stateEntryTimestamp_ms = Scheduler_GetUptime_ms();
    g_ecuData.activeDTC = DTC_NONE;

    if (g_fsmStateTable[STATE_OFF].onEntry != Null)
    {
        g_fsmStateTable[STATE_OFF].onEntry();
    }
}

Std_ReturnType FSM_Transition(ECU_State_t targetState)
{
    if (targetState >= FSM_NUM_STATES)
    {
        return ENotOk;
    }

    if (targetState == g_ecuData.currentState)
    {
        return EOk;
    }

    // Validate legality against transition matrix, achieving AC-3

    if (g_ecuData.currentState == STATE_OFF && targetState != STATE_START)
    {
        return ENotOk;
    }
    else if (g_ecuData.currentState == STATE_START && (targetState != STATE_RUN || targetState != STATE_FAULT))
    {
        return ENotOk;
    }
    else if (g_ecuData.currentState == STATE_RUN && targetState != STATE_OFF && targetState != STATE_DIAGNOSTIC && targetState != STATE_FAULT)
    {
        return ENotOk;
    }
    else if (g_ecuData.currentState == STATE_DIAGNOSTIC && targetState != STATE_RUN && targetState != STATE_FAULT)
    {
        return ENotOk;
    }
    else if (g_ecuData.currentState == STATE_FAULT && targetState != STATE_SAFE_MODE)
    {
        return ENotOk;
    }
    else if (g_ecuData.currentState == STATE_SAFE_MODE && targetState != STATE_OFF)
    {
        return ENotOk;
    }
    else
    {
    }

    // Enter Critical Section (save SREG and cli())
    uint8_t temp = SREG_Reg;

    // clear global interrupt enable bit inside SREG register
    cli();
    // Execute Exit Action of active state, achieving AC-4
    if (g_fsmStateTable[g_ecuData.currentState].onExit != Null)
    {
        g_fsmStateTable[g_ecuData.currentState].onExit();
    }

    // Update state variables
    g_ecuData.previousState = g_ecuData.currentState;
    g_ecuData.currentState = targetState;
    g_ecuData.stateEntryTimestamp_ms = Scheduler_GetUptime_ms();

    // Execute Entry Action of target state
    if (g_fsmStateTable[targetState].onEntry != Null)
    {
        g_fsmStateTable[targetState].onEntry();
    }

    // Restore SREG and return
    SREG_Reg = temp;
    return EOk;
}

void FSM_Run(void)
{
    // fetch sctive state descriptor
    const FSM_StateDescriptor_t *desc = &g_fsmStateTable[g_ecuData.currentState];

    // update time metric
    g_ecuData.uptimeSeconds = Scheduler_GetUptime_s();

    // Execute Action Function onRun
    if (desc->onRun != Null)
    {
        desc->onRun();
    }
    return;
}

ECU_State_t FSM_GetState(void)
{
    return g_ecuData.currentState;
}

const char *FSM_GetStateString(ECU_State_t state)
{
    if (state >= FSM_NUM_STATES)
    {
        return "Invalid_State";
    }

    return g_fsmStateTable[state].stateName;
}

bool FSM_IsState(ECU_State_t queryState)
{
    if (queryState == g_ecuData.currentState)
    {
        return True;
    }
    else
    {
        return False;
    }
}
 // achieving AC-6
const ECUData_t *FSM_GetECUData(void)
{
    return &g_ecuData;
}

void FSM_UpdateSensors(float temp_C, float batt_V)
{
    g_ecuData.engineTemp_C = temp_C;
    g_ecuData.batteryVoltage_V = batt_V;
}

void FSM_UpdateActuators(uint8_t fanDutyPct)
{
    g_ecuData.fanDutyPct = fanDutyPct;
}

void FSM_UpdateCommsStatus(bool btActive, bool wifiActive)
{
    g_ecuData.btConnected = btActive;
    g_ecuData.wifiConnected = wifiActive;
}

#endif
