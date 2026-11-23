/**
 * @file      FSM_Interface.h
 * @brief     Public interface for the FSM State Machine Core engine.
 * @details   Declares public APIs for initialization, periodic execution, 
 *            guarded state transitions, state queries, and shared data updates.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _FSM_INTERFACE_H_
 #define _FSM_INTERFACE_H_

 #include "FSM_Private.h"
 #include "FSM_Config.h"
 #include "../../Common/Definition.h" 
#include <stdbool.h> 

/**
 * @brief  Initializes the FSM state machine, clears shared ECU data, and enters STATE_OFF.
 * @return void
 */
void FSM_Init(void);

/**
 * @brief  Executes periodic FSM state behavior and updates system uptime telemetry.
 * @return void
 */
void FSM_Run(void);

/**
 * @brief  Performs a guarded transition from the current state to targetState.
 * @param  targetState Requested destination state of type ECU_State_t.
 * @return Std_ReturnType EOk if transition succeeds or state is already active, ENotOk if invalid.
 */
Std_ReturnType FSM_Transition(ECU_State_t targetState);

/**
 * @brief  Returns the currently active ECU state.
 * @return ECU_State_t Current state value.
 */
ECU_State_t FSM_GetState(void);

/**
 * @brief  Returns a human-readable string name for the given ECU state.
 * @param  state Target state to convert to string.
 * @return const char* Pointer to constant state name string (e.g., "SAFE_MODE").
 */
const char *FSM_GetStateString(ECU_State_t state);

/**
 * @brief  Checks whether the FSM is currently in queryState.
 * @param  queryState State to compare against current state.
 * @return bool True if current state equals queryState, False otherwise.
 */
bool FSM_IsState(ECU_State_t queryState);

/**
 * @brief  Provides read-only access to the global shared ECUData_t structure.
 * @return const ECUData_t* Constant pointer to g_ecuData.
 */
const ECUData_t *FSM_GetECUData(void);

/**
 * @brief  Updates sensor telemetry fields inside the shared ECU data structure.
 * @param  temp_C Latest temperature reading in Celsius.
 * @param  batt_V Latest battery voltage reading in Volts.
 * @return void
 */
void FSM_UpdateSensors(float temp_C, float batt_V);

/**
 * @brief  Updates actuator status fields (such as fan duty cycle) in shared ECU data.
 * @param  fanDutyPct Cooling fan duty cycle percentage.
 * @return void
 */
void FSM_UpdateActuators(uint8_t fanDutyPct);

/**
 * @brief  Updates wireless communication status flags in shared ECU data.
 * @param  btActive   Bluetooth active state.
 * @param  wifiActive Wi-Fi active state.
 * @return void
 */
void FSM_UpdateCommsStatus(bool btActive, bool wifiActive);


 #endif
