/**
 * @file      FSM_Private.h
 * @brief     FSM driver Private APIs and enums.
 * @details   
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Abdulrahman Ali
 * @date      2026-11-17
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */


 #ifndef _FSM_PRIVATE_H_
 #define _FSM_PRIVATE_H_

 #include <stdint.h>
 #include <stdbool.h> 

 typedef enum {
     STATE_OFF         = 0U,
     STATE_START       = 1U,
     STATE_RUN         = 2U,
     STATE_DIAGNOSTIC  = 3U,
    STATE_FAULT        = 4U,
    STATE_SAFE_MODE    = 5U
} ECU_State_t;

typedef enum {
    DTC_NONE                 = 0x00U,
    DTC_F001_OVER_TEMP       = 0x01U,
    DTC_F002_BATT_VOLT       = 0x02U,
    DTC_F003_SENSOR_DISC     = 0x03U,
    DTC_F004_COMMS_TIMEOUT   = 0x04U,
    DTC_F005_WATCHDOG_RESET  = 0x05U
} DTC_Code_t;

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

typedef void (*FSM_ActionFunc_t)(void);

typedef struct {
    ECU_State_t       stateId;
    FSM_ActionFunc_t  onEntry;
    FSM_ActionFunc_t  onRun;
    FSM_ActionFunc_t  onExit;
    const char*       stateName;
} FSM_StateDescriptor_t;


#endif