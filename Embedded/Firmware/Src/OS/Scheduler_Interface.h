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
#include "../Mcal/Gie/GIE_Interface.h"

/**
 * @enum  SchedulerTaskId_t
 * @brief Predefined Task IDs mapped to the core cyclic tasks (Table 1.3).
 */
typedef enum {
    TASK_ID_CMD_PARSER     = 0,
    TASK_ID_FAULT_MONITOR  = 1,
    TASK_ID_ADC_SAMPLING   = 2,
    TASK_ID_LCD_REFRESH    = 3,
    TASK_ID_TELEMETRY      = 4,
    TASK_ID_HEARTBEAT      = 5,
    TASK_ID_MAX
} SchedulerTaskId_t;


/**
 * @typedef SchedulerTaskFunc_t
 * @brief   Function pointer type for non-preemptive cyclic tasks.
 * @note    Tasks must satisfy run-to-completion contract without blocking delays.
 */
typedef void (*SchedulerTaskFunc_t)(void);

/**
 * @struct  SchedulerTask_t
 * @brief   Task descriptor holding execution metadata and diagnostic counters.
 */
typedef struct {
    char                name[SCHEDULER_TASK_NAME_LEN]; 
    SchedulerTaskFunc_t taskFunc;                   
    uint16_t            periodMs;               
    uint16_t            offsetMs;                
    uint32_t            lastRunMs;              
    uint32_t            runCount;                
    bool                enabled;                 
} SchedulerTask_t;


/**
 * @struct  SchedulerTelemetry_t
 * @brief   Diagnostic snapshot of OS kernel state and timing statistics.
 */
typedef struct {
    uint32_t sysTick_ms;    
    uint32_t uptimeSeconds;  
    uint8_t  activeTaskCount; 
    bool     overrunDetected; 
} SchedulerTelemetry_t;

/**
 * @brief      Initializes scheduler internal variables, clears the static task table,
 *             registers the 10ms SysTick hook with MCAL Timer2, and starts Timer2 CTC mode.
 * @return     void
 */
void Scheduler_Init(void);

/**
 * @brief      Registers a periodic task in the static task table.
 * @param[in]  taskFunc  Pointer to the task function (void(*)(void)).
 * @param[in]  periodMs  Task execution period in milliseconds (must be >= 10ms and a multiple of 10ms).
 * @param[in]  offsetMs  Initial execution offset / phase shift in milliseconds.
 * @param[in]  name      Null-terminated string name identifying the task.
 * @return     EOk if registered successfully; ENotOk if parameters are invalid or table is full.
 */
Std_ReturnType Scheduler_AddTask(SchedulerTaskFunc_t taskFunc, 
                                 uint16_t periodMs, 
                                 uint16_t offsetMs, 
                                 const char* name);

/**
 * @brief      Continuous cyclic dispatcher. Evaluates elapsed time for each registered
 *             task and invokes ready tasks synchronously (Run-to-Completion).
 * @note       Must be called repeatedly from the super-loop in main().
 * @return     void
 */
void Scheduler_Run(void);

/**
 * @brief      10ms SysTick interrupt callback invoked from MCAL Timer2 ISR context.
 *             Increments sysTick_ms and updates uptime.
 * @note       Executes in < 1.5 us and contains no application business logic.
 * @return     void
 */
void Scheduler_Tick_ISR(void);

/**
 * @brief      Enables a registered task by name, allowing it to be dispatched.
 * @param[in]  name  Name of the task to enable.
 * @return     EOk if task was found and enabled; ENotOk otherwise.
 */
Std_ReturnType Scheduler_EnableTask(const char* name);

/**
 * @brief      Disables a registered task by name, halting its execution immediately.
 * @param[in]  name  Name of the task to disable.
 * @return     EOk if task was found and disabled; ENotOk otherwise.
 */
Std_ReturnType Scheduler_DisableTask(const char* name);

/**
 * @brief      Returns the current system uptime in milliseconds since boot.
 *             Guaranteed atomic 32-bit read protected against register tearing.
 * @return     uint32_t Milliseconds elapsed since ECU power-on.
 */
uint32_t Scheduler_GetUptime_ms(void);

/**
 * @brief      Returns the current system uptime in whole seconds since boot.
 *             Thread-safe atomic read wrapper.
 * @return     uint32_t Seconds elapsed since ECU power-on.
 */
uint32_t Scheduler_GetUptime_s(void);

/**
 * @brief      Retrieves the total execution run count of a registered task.
 * @param[in]  name  Name of the task to query.
 * @return     uint32_t Execution count, or 0 if task not found or name is NULL.
 */
uint32_t Scheduler_GetTaskRunCount(const char* name);

/**
 * @brief      Atomically copies current scheduler telemetry and health statistics.
 * @param[out] statsPtr Pointer to destination SchedulerTelemetry_t structure.
 * @return     void
 */
void Scheduler_GetTelemetry(SchedulerTelemetry_t* statsPtr);

/**
 * @brief      Enables a registered task directly by Task ID (O(1) immediate access).
 * @param[in]  taskId  Task table index / ID.
 * @return     EOk if taskId < activeTaskCount; ENotOk otherwise.
 */
Std_ReturnType Scheduler_EnableTaskById(uint8_t taskId);

/**
 * @brief      Disables a registered task directly by Task ID (O(1) immediate access).
 * @param[in]  taskId  Task table index / ID.
 * @return     EOk if taskId < activeTaskCount; ENotOk otherwise.
 */
Std_ReturnType Scheduler_DisableTaskById(uint8_t taskId);

/**
 * @brief      Retrieves the total execution run count of a registered task by its ID.
 * @param[in]  taskId  Task table index / ID.
 * @return     uint32_t Execution count, or 0 if taskId is invalid.
 */
uint32_t Scheduler_GetTaskRunCountById(uint8_t taskId);
