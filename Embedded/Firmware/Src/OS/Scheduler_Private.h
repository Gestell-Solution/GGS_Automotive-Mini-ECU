/**
* @file       Scheduler_Private.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Yassen Ahmed )
* @brief      Private header file for the Scheduler
* @details    This header file contains the private definitions and declarations for the Scheduler
* @version    1.0.0
* @date       16/11/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#include <stdint.h>

#define SCHEDULER_TICK_MS          10U   
#define SCHEDULER_MAX_TASKS        8U  
#define SCHEDULER_TASK_NAME_LEN    16U  

#define TASK_PERIOD_CMD_PARSER     10U   
#define TASK_PERIOD_FAULT_MONITOR  20U  
#define TASK_PERIOD_ADC_SAMPLING   100U 
#define TASK_PERIOD_LCD_REFRESH    200U  
#define TASK_PERIOD_TELEMETRY      500U 
#define TASK_PERIOD_HEARTBEAT      1000U 

/* =========================================================================
   PRIVATE STATIC MODULE VARIABLES
   ========================================================================= */


typedef void (*SchedulerTaskFunc_t)(void);

typedef struct {
    char                name[SCHEDULER_TASK_NAME_LEN]; 
    SchedulerTaskFunc_t taskFunc;                   
    uint16_t            periodMs;               
    uint16_t            offsetMs;                
    uint32_t            lastRunMs;              
    uint32_t            runCount;                
    bool                enabled;                 
} SchedulerTask_t;

typedef struct {
    uint32_t sysTick_ms;    
    uint32_t uptimeSeconds;  
    uint8_t  activeTaskCount; 
    bool     overrunDetected; 
} SchedulerTelemetry_t;
