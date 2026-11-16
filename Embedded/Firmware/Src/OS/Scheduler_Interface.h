/**
* @file       Scheduler_Interface.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Yassen Ahmed )
* @brief      Interface file for the Scheduler
* @details    This header file defines the interface for the Scheduler
* @version    1.0.0
* @date       16/11/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/


/* Public library */
#include <stdint.h>
#include <stdbool.h>

/* common library */
#include "../Common/Config.h"
#include "../Common/Definition.h"
#include "../Common/MacroFunction.h"

/* led library */
#include "Scheduler_Private.h"
#include "Scheduler_Config.h" 

/* other dependencies */
#include "../Mcal/Timer2/Timer2_Interface.h"
#include "Embedded\Firmware\Src\Mcal\Gie\GIE_Interface.h"


void Scheduler_Init(void);

Std_ReturnType Scheduler_AddTask(SchedulerTaskFunc_t taskFunc, 
                                 uint16_t periodMs, 
                                 uint16_t offsetMs, 
                                 const char* name);


void Scheduler_Run(void);

void Scheduler_Tick_ISR(void);

Std_ReturnType Scheduler_EnableTask(const char* name);

Std_ReturnType Scheduler_DisableTask(const char* name);

uint32_t Scheduler_GetUptime_ms(void);

uint32_t Scheduler_GetUptime_s(void);

uint32_t Scheduler_GetTaskRunCount(const char* name);

void Scheduler_GetTelemetry(SchedulerTelemetry_t* statsPtr);