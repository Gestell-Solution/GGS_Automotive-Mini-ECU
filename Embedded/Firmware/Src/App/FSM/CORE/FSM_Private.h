
/**
 * @file      FSM_Private.h
 * @brief     Private definitions for the FSM state table, internal structures, and transition validation.
 * @details   Declares internal action callback types, state descriptor structures, the master
 *            state table registry, and the static helper function used to validate transition
 *            legality against the ECU state transition matrix.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 * @see       FSM_Interface.h, FSM_Config.h, FSM_Program.c
 */


 #ifndef _FSM_PRIVATE_H_
 #define _FSM_PRIVATE_H_

 #include <stdint.h>
 #include <stdbool.h>
 #include "../../../Common/Definition.h"
 #include "FSM_Config.h"

 /**
 * @brief Function pointer type for state lifecycle callbacks (onEntry, onRun, onExit).
 */
typedef void (*FSM_ActionFunc_t)(void);

typedef struct {
    ECU_State_t   currentState;
    ECU_State_t   previousState;
    uint32_t      stateEntryTimestamp_ms;
    bool          ignitionActive;
    float         engineTemp_C;
    float         batteryVoltage_V;
    uint8_t       fanDutyPct;
    DTC_Code_t    activeDTC;
    bool          selfTestPassed;
    bool          btConnected;
    bool          wifiConnected;
    uint32_t      uptimeSeconds;
} ECUData_t;


/**
 * @brief Descriptor structure binding an ECU state to its lifecycle hooks and display name.
 */
typedef struct {
    ECU_State_t       stateId;
    FSM_ActionFunc_t  onEntry;
    FSM_ActionFunc_t  onRun;
    FSM_ActionFunc_t  onExit;
    const char*       stateName;
} FSM_StateDescriptor_t;

#endif