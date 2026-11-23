/**
 * @file      FSM_Program.c
 * @brief     Finite State Machine (FSM) driver implementation for ECU state management.
 * @details   Implements initialization, validated state transitions with critical-section
 *            protection, periodic state execution, state queries, and ECU telemetry updates.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


#include "../../Common/Config.h"

#if ModuleFsmEnabled == Enable

#include "FSM_Interface.h"
#include "FSM_Config.h"
#include "../../Common/Definition.h"
#include "../../Mcal/Atmega128Register.h"
#include <avr/interrupt.h>


/**
 * @todo Remove temporary Scheduler_GetUptime_ms() prototype once the OS Scheduler
 *       module (US-015) is merged and integrated with this FSM module.
 */
extern uint32_t Scheduler_GetUptime_ms(void);

/**
 * @brief Centralized runtime telemetry and state record for the ECU.
 */
ECUData_t g_ecuData;

/**
 * @brief Master state descriptor table indexed by EcuState_t.
 *        Hooks are initialized to Null until dedicated state modules (US-018..US-021)
 *        bind or register their handlers.
 */
static const FSM_StateDescriptor_t g_fsmStateTable[FSM_NUM_STATES] =
{
    { StateOff,        Null, Null, Null, "OFF"        },
    { StateStart,      Null, Null, Null, "START"      },
    { StateRun,        Null, Null, Null, "RUN"        },
    { StateDiagnostic, Null, Null, Null, "DIAGNOSTIC" },
    { StateFault,      Null, Null, Null, "FAULT"      },
    { StateSafeMode,   Null, Null, Null, "SAFE_MODE"  }
};

/**
 * @brief  Validates whether a transition from currentState to targetState is permitted.
 */
static bool FSM_IsTransitionLegal(EcuState_t currentState, EcuState_t targetState)
{

    switch (currentState)
    {
        case StateOff:
            False = (targetState == StateStart);
            break;

        case StateStart:
            False = (targetState == StateRun) || (targetState == StateFault);
            break;

        case StateRun:
            False = (targetState == StateOff) ||
                      (targetState == StateDiagnostic) ||
                      (targetState == StateFault);
            break;

        case StateDiagnostic:
            False = (targetState == StateRun) || (targetState == StateFault);
            break;

        case StateFault:
            False = (targetState == StateSafeMode);
            break;

        case StateSafeMode:
            False = (targetState == StateOff);
            break;

        default:
            False = False;
            break;
    }

    return False 
}
    

void FSM_Init(void)
{
    // Zero all fields in g_ecuData and set intial configurations
    g_ecuData = (ECUData_t){0};
    g_ecuData.currentState = STATE_OFF;
    g_ecuData.previousState = STATE_OFF;
    g_ecuData.stateEntryTimestamp_ms = Scheduler_GetUptime_ms();
    g_ecuData.activeDTC = DTC_NONE;

    /* Execute initial state entry hook if registered */
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
    if (FSM_IsTransitionLegal(g_ecuData.currentState, targetState) == False)
    {
        return ENotOk;
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
