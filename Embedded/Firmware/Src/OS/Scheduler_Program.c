/**
* @file       Scheduler_Program.c
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Yassen Ahmed )
* @brief      Implementation file for the Scheduler
* @details    This source file contains the implementation of the Scheduler
* @version    1.0.0
* @date       16/11/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#include "../Common/Definition.h"
#include "../Common/Config.h"

#if (ModuleOsEnabled == Enable) 

#include "Scheduler_Interface.h"


static SchedulerTask_t      g_taskTable[SCHEDULER_MAX_TASKS];
static uint8_t              g_activeTaskCount = 0U;
static volatile uint32_t    g_sysTick_ms      = 0U;
static volatile uint32_t    g_uptimeSeconds   = 0U;
static volatile bool        g_schedulerRunning = false;
static volatile bool        g_overrunDetected  = false;

/* =========================================================================
   PRIVATE HELPER FUNCTIONS
   ========================================================================= */

/**
 * @brief  Safe string equality comparison without <string.h> or <stdio.h>.
 * @param  s1 First null-terminated string.
 * @param  s2 Second null-terminated string.
 * @return true if both strings match exactly; false otherwise.
 */
static bool Scheduler_StrEquals(const char* s1, const char* s2)
{
    if ((s1 == Null) || (s2 == Null))
    {
        return false;
    }

    while ((*s1 != '\0') && (*s2 != '\0'))
    {
        if (*s1 != *s2)
        {
            return false;
        }
        s1++;
        s2++;
    }

    return (*s1 == *s2);
}

/* =========================================================================
   PUBLIC API IMPLEMENTATION
   ========================================================================= */

/**
 * @brief      Initializes the cooperative scheduler and hardware timing subsystems.
 * @details    Performs atomic initialization by temporarily disabling global interrupts (GIE),
 *             clearing internal telemetry counters, wiping the static task table, configuring
 *             MCAL Timer2 in 10ms CTC mode, registering the SysTick ISR hook callback, and
 *             re-enabling global interrupts upon completion.
 * @return     void
 */
void Scheduler_Init(void)
{
    uint8_t count = 0 ;

    /* diable global interrupts*/
    GIE_Disable();

    /* set all the values to nothing */
    g_activeTaskCount = 0U ;
    g_sysTick_ms = 0U ;
    g_uptimeSeconds = 0U;
    g_overrunDetected  = false;
    g_schedulerRunning = false;

    /* clear all the table from any random data */
    for (uint8_t i = 0U; i < SCHEDULER_MAX_TASKS; i++)
    {
        g_taskTable[i].taskFunc  = Null;
        g_taskTable[i].periodMs  = 0U;
        g_taskTable[i].offsetMs  = 0U;
        g_taskTable[i].lastRunMs = 0U;
        g_taskTable[i].runCount  = 0U;
        g_taskTable[i].enabled   = false;
        for (uint8_t j = 0U; j < SCHEDULER_TASK_NAME_LEN; j++)
        {
            g_taskTable[i].name[j] = '\0';
        }
    }
    
    /* Initialize MCAL Timer2 in 10ms CTC mode */
    mTimer2_Init_CTC_10ms();

    /* Register SysTick ISR hook callback with MCAL Timer2 */
    mTimer2_SetCallback(Scheduler_Tick_ISR);


    g_schedulerRunning = True;

    /* we reenable the golobal interputs */
    GIE_Enable();
}

/**
 * @brief      Registers a periodic task into the static task table.
 * @details    Validates that the task function pointer and name string are non-null,
 *             verifies that the execution period is >= 10ms and an exact integer multiple
 *             of SCHEDULER_TICK_MS, and ensures the static task table capacity limit is
 *             not exceeded. The task is initialized in an enabled state with the configured
 *             initial execution offset.
 * @param[in]  taskFunc  Pointer to the task executable function (void (*)(void)).
 * @param[in]  periodMs  Task execution period in milliseconds (must be >= 10ms and multiple of 10ms).
 * @param[in]  offsetMs  Initial execution offset / phase shift in milliseconds.
 * @param[in]  name      Null-terminated string identifier for the task.
 * @return     Std_ReturnType
 * @retval     EOk      Task was successfully registered in the task table.
 * @retval     ENotOk   Registration rejected due to invalid parameters or full task table.
 */
Std_ReturnType Scheduler_AddTask(SchedulerTaskFunc_t taskFunc, uint16_t periodMs, uint16_t offsetMs, const char* name)
{
    /* Validate input parameters */
    if ((taskFunc == Null) || (name == Null))
    {
        return ENotOk;
    }
    
    /* Period must be >= 10ms and an integer multiple of SCHEDULER_TICK_MS */
    if ((periodMs < SCHEDULER_TICK_MS) || ((periodMs % SCHEDULER_TICK_MS) != 0U))
    {
        return ENotOk;
    }

    /* Check static task table capacity limit */
    if (g_activeTaskCount >= SCHEDULER_MAX_TASKS)
    {
        return ENotOk;
    }

    uint8_t idx = g_activeTaskCount;

    g_taskTable[idx].taskFunc = taskFunc;
    g_taskTable[idx].periodMs = periodMs;
    g_taskTable[idx].offsetMs = offsetMs;
    g_taskTable[idx].lastRunMs = (uint32_t)offsetMs;
    g_taskTable[idx].runCount = 0U;

    for (uint8_t i = 0 ; i < (SCHEDULER_TASK_NAME_LEN - 1) && (name[i] != '\0') ; i++)
    {
        g_taskTable[idx].name[i] = name[i]; // might be a bug
    }
    g_taskTable[idx].name[SCHEDULER_TASK_NAME_LEN - 1] = '\0';

    g_taskTable[idx].enabled = true;
    
    g_activeTaskCount++;

    return EOk;
}

/**
 * @brief      Continuous cyclic cooperative task dispatcher.
 * @details    Evaluates elapsed system time for each enabled task registered in the table.
 *             Invokes ready tasks synchronously under a Run-to-Completion (RTC) contract without
 *             preemption. Tracks execution timestamps and increments run counters. Detects task
 *             timing overruns if an execution cycle slips by twice the period, or if the dispatcher
 *             slot duration exceeds the 10ms SysTick heartbeat.
 * @note       Must be called repeatedly from the infinite superloop in main().
 * @return     void
 */
void Scheduler_Run(void)
{
    /* copy timestamp at start of dispatch cycle */
    uint32_t now = Scheduler_GetUptime_ms();
    for (uint8_t index = 0U ; index < g_activeTaskCount ; index++)
    {
        SchedulerTask_t * task = &g_taskTable[index];

        /* Skip disabled tasks */
        if (!task->enabled)
        {
            continue;
        }

        if (now < task->lastRunMs)
        {
            continue;
        }

        /* Check if period interval has elapsed */
        if ((now - task->lastRunMs) >= (uint32_t)task->periodMs)
        {
            /* Overrun detection: task lagged by >= 2x configured period */
            if ((task->runCount > 0U) && ((now - task->lastRunMs) >= (2U * (uint32_t)task->periodMs)))
            {
                g_overrunDetected = true;
            }

            /* Synchronous run-to-completion task execution */
            task->taskFunc();
            task->lastRunMs = now;
            task->runCount++;
        }
    }
    
    /* Overrun detection: whole dispatcher slot exceeded 10ms SysTick interval */
    if ((Scheduler_GetUptime_ms() - now) > SCHEDULER_TICK_MS)
    {
        g_overrunDetected = true;
    }
}

/**
 * @brief      10ms SysTick heartbeat callback invoked from MCAL Timer2 ISR context.
 * @details    Increments the 32-bit millisecond system tick counter (g_sysTick_ms) by
 *             SCHEDULER_TICK_MS (10ms) and tracks whole-second rollover for uptime calculation.
 *             Executes with minimal latency (< 1.5 us) without blocking operations or
 *             application business logic.
 * @return     void
 */
void Scheduler_Tick_ISR(void)
{
   /* Increment 10ms millisecond counter */
   g_sysTick_ms += SCHEDULER_TICK_MS;

   /* Check for whole second rollover */
   if ((g_sysTick_ms % 1000U) == 0U)
   {
        g_uptimeSeconds++;
   }

}

/**
 * @brief      Enables a registered task directly by table index (O(1) access).
 * @details    Checks whether the specified task index is within the active registered range.
 *             If valid, sets the task's enabled flag to true, permitting it to be dispatched
 *             by Scheduler_Run().
 * @param[in]  taskId  Zero-based index of the task in the static task table.
 * @return     Std_ReturnType
 * @retval     EOk     Task was found and successfully enabled.
 * @retval     ENotOk  Provided taskId exceeds current active task count.
 */
Std_ReturnType Scheduler_EnableTaskById(uint8_t taskId)
{
    if (taskId >= g_activeTaskCount)
    {
        return ENotOk;
    }

    g_taskTable[taskId].enabled = true;
    return EOk;
}

/**
 * @brief      Disables a registered task directly by table index (O(1) access).
 * @details    Checks whether the specified task index is within the active registered range.
 *             If valid, sets the task's enabled flag to false, immediately halting its
 *             execution during subsequent dispatcher cycles.
 * @param[in]  taskId  Zero-based index of the task in the static task table.
 * @return     Std_ReturnType
 * @retval     EOk     Task was found and successfully disabled.
 * @retval     ENotOk  Provided taskId exceeds current active task count.
 */
Std_ReturnType Scheduler_DisableTaskById(uint8_t taskId)
{
    if (taskId >= g_activeTaskCount)
    {
        return ENotOk;
    }

    g_taskTable[taskId].enabled = false;
    return EOk;
}

/**
 * @brief      Retrieves the total execution run count of a task by its table index.
 * @details    Provides diagnostic insight into how many times the specified task has executed
 *             since system startup.
 * @param[in]  taskId  Zero-based index of the task in the static task table.
 * @return     uint32_t Total execution count of the task, or 0 if taskId is invalid.
 */
uint32_t Scheduler_GetTaskRunCountById(uint8_t taskId)
{
    if (taskId >= g_activeTaskCount)
    {
        return 0U;
    }

    return g_taskTable[taskId].runCount;
}

/**
 * @brief      Enables a registered task by its string identifier.
 * @details    Performs a linear scan over the active task table to match the given task name.
 *             If located, delegates to Scheduler_EnableTaskById() to set the task's enabled flag.
 * @param[in]  name  Null-terminated string name of the target task.
 * @return     Std_ReturnType
 * @retval     EOk     Task with the given name was found and enabled.
 * @retval     ENotOk  Name is NULL or task was not found in the active table.
 */
Std_ReturnType Scheduler_EnableTask(const char* name)
{
    if(name = Null)
    {
        return ENotOk;
    }

    for (uint8_t i = 0U; i < g_activeTaskCount; i++)
    {
        if (Scheduler_StrEquals(g_taskTable[i].name, name))
        {
            return Scheduler_EnableTaskById(i);
        }
    }

    return ENotOk;
}

/**
 * @brief      Disables a registered task by its string identifier.
 * @details    Performs a linear scan over the active task table to match the given task name.
 *             If located, delegates to Scheduler_DisableTaskById() to clear the task's enabled flag.
 * @param[in]  name  Null-terminated string name of the target task.
 * @return     Std_ReturnType
 * @retval     EOk     Task with the given name was found and disabled.
 * @retval     ENotOk  Name is NULL or task was not found in the active table.
 */
Std_ReturnType Scheduler_DisableTask(const char* name)
{
    if (name == Null)
    {
        return ENotOk;
    }

    for (uint8_t i = 0U; i < g_activeTaskCount; i++)
    {
        if (Scheduler_StrEquals(g_taskTable[i].name, name))
        {
            return Scheduler_DisableTaskById(i);
        }
    }

    return ENotOk;
}

/**
 * @brief      Returns the current system uptime in milliseconds since boot.
 * @details    Provides an atomic 32-bit read of g_sysTick_ms on an 8-bit AVR architecture
 *             by briefly disabling global interrupts via GIE_Disable() and restoring them
 *             with GIE_Enable() to prevent register tearing during multi-byte reads.
 * @return     uint32_t System uptime in milliseconds.
 */
uint32_t Scheduler_GetUptime_ms(void)
{
    uint32_t uptime;
    uint8_t sreg = SREG_Reg;
    GIE_Disable();
    uptime = g_sysTick_ms;   /* Atomic 4-byte copy */
    SREG_Reg = sreg;         /* Restore prior interrupt state */
    return uptime;
}

/**
 * @brief      Returns the current system uptime in whole seconds since boot.
 * @details    Provides an atomic 32-bit read of g_uptimeSeconds on an 8-bit AVR architecture
 *             by briefly disabling global interrupts via GIE_Disable() and restoring them
 *             with GIE_Enable() to prevent register tearing.
 * @return     uint32_t System uptime in whole seconds.
 */
uint32_t Scheduler_GetUptime_s(void)
{
    uint32_t uptime;
    uint8_t sreg = SREG_Reg;
    GIE_Disable();
    uptime = g_uptimeSeconds;   /* Atomic 4-byte copy */
    SREG_Reg = sreg;         /* Restore prior interrupt state */
    return uptime;
}

/**
 * @brief      Retrieves the total execution run count of a task by its string identifier.
 * @details    Searches the registered task table for a matching name string and returns
 *             the accumulated execution count of that task.
 * @param[in]  name  Null-terminated string name of the task to query.
 * @return     uint32_t Total execution count of the task, or 0 if name is NULL or not found.
 */
uint32_t Scheduler_GetTaskRunCount(const char* name)
{
    if (name == Null)
    {
        return 0U;
    }

    for (uint8_t i = 0U; i < g_activeTaskCount; i++)
    {
        if (Scheduler_StrEquals(g_taskTable[i].name, name))
        {
            return Scheduler_GetTaskRunCountById(i);
        }
    }

    return 0U;
}


/**
 * @brief      Captures a coherent diagnostic snapshot of scheduler telemetry and health status.
 * @details    Populates the caller-supplied SchedulerTelemetry_t structure with atomic uptime
 *             readings (milliseconds and seconds), the active task count, and the timing overrun flag.
 * @param[out] statsPtr  Pointer to destination structure to receive telemetry metrics.
 * @return     void
 */
void Scheduler_GetTelemetry(SchedulerTelemetry_t* statsPtr)
{
    if (statsPtr == Null)
    {
        return;
    }

    statsPtr->sysTick_ms      = Scheduler_GetUptime_ms();
    statsPtr->uptimeSeconds   = Scheduler_GetUptime_s();
    statsPtr->activeTaskCount = g_activeTaskCount;
    statsPtr->overrunDetected = g_overrunDetected;
}


#endif  