/**
 * @file      FSM_Interface.h
 * @brief     FSM driver interface APIs and data types.
 * @details   
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Abdulrahman Ali
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _FSM_INTERFACE_H_
 #define _FSM_INTERFACE_H_

 #include "FSM_Private.h"
 #include "FSM_Config.h"
 #include "../../Common/Definition.h" 
#include <stdbool.h> 
 void              FSM_Init(void);
 void              FSM_Run(void);
 Std_ReturnType    FSM_Transition(ECU_State_t targetState);
 ECU_State_t       FSM_GetState(void);
 const char*       FSM_GetStateString(ECU_State_t state);
 bool              FSM_IsState(ECU_State_t queryState);
 const ECUData_t*  FSM_GetECUData(void);
 void              FSM_UpdateSensors(float temp_C, float batt_V);
 void              FSM_UpdateActuators(uint8_t fanDutyPct);
 void              FSM_UpdateCommsStatus(bool btActive, bool wifiActive);
 
 #endif
