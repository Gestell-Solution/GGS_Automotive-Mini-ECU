/**
 * @file System_Init.h
 * @author salma mohamed (salmafouadothman@gmail.com)
 * @author roua ayman
 * @brief The API for the system init
 * @details API and enums for the system init  
 * @version 0.1
 * @date 2026-11-19  
 * @copyright Copyright (c) 2026 GESTELL
 */
#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include <stdint.h>
#include <stdbool.h>

#include"System_Config.h"

typedef enum {
    RESET_REASON_UNKNOWN        = 0x00U,
    RESET_REASON_POWER_ON       = 0x01U,
    RESET_REASON_EXTERNAL       = 0x02U,
    RESET_REASON_BROWNOUT       = 0x04U,
    RESET_REASON_WATCHDOG       = 0x08U,
    RESET_REASON_JTAG           = 0x10U
} System_ResetReason_t;

typedef enum {
    SYS_INIT_OK                 = 0x00U,
    SYS_INIT_ERR_MCAL           = 0x01U,
    SYS_INIT_ERR_HAL            = 0x02U,
    SYS_INIT_ERR_OS             = 0x03U,
    SYS_INIT_ERR_APP            = 0x04U,
    SYS_INIT_ERR_TIMEOUT        = 0x05U
} System_InitStatus_t;

typedef struct {
    System_ResetReason_t resetReason;
    uint32_t             bootDuration_us;
    System_InitStatus_t  initStatus;
    bool                 watchdogArmed;
} System_BootInfo_t;

void System_Init(void);
System_ResetReason_t System_GetResetReason(void);
const System_BootInfo_t* System_GetBootInfo(void);

void System_Init_MCAL(void);
void System_Init_HAL(void);
void System_Init_OS(void);
void System_Init_APP(void);

void System_PetWatchdog(void);
void System_EmergencyHalt(System_InitStatus_t errorReason);

#endif /* SYSTEM_INIT_H */