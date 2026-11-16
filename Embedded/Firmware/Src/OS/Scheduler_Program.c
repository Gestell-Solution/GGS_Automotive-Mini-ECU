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

void Scheduler_Init(void)
{
    uint8_t count = 0 ;
    // cli();
    GIE_Disable();
    g_activeTaskCount = 0U ;
    g_sysTick_ms = 0U ;
    g_uptimeSeconds = 0U;

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

    mTimer2_SetCallback(Scheduler_Tick_ISR);
    mTimer2_Init_CTC_10ms();

    g_schedulerRunning = True;

    // sei();
    GIE_Enable();
}

Std_ReturnType Scheduler_AddTask(SchedulerTaskFunc_t taskFunc, uint16_t periodMs, uint16_t offsetMs, const char* name)
{
    if((taskFunc == Null) || 
    (periodMs < SCHEDULER_TICK_MS) || 
    ((periodMs % SCHEDULER_TICK_MS) != 0) ||
    (name == Null) ||
    (g_activeTaskCount >= SCHEDULER_MAX_TASKS))
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

void Scheduler_Run(void)
{
    uint32_t now = Scheduler_GetUptime_ms();
    for (uint8_t index = 0U ; index < g_activeTaskCount ; index++)
    {
        SchedulerTask_t * task = &g_taskTable[index];

        if (((task->enabled) == True) && ((now - task->lastRunMs) >= (uint32_t)task->periodMs))
        {
            task->taskFunc();
            task->lastRunMs = now;
            task->runCount++;
        }
    }
}

void Scheduler_Tick_ISR(void)
{
   g_sysTick_ms += SCHEDULER_TICK_MS;

   if ((g_sysTick_ms % 1000U) == 0U)
   {
        g_uptimeSeconds++;
   }

}

Std_ReturnType Scheduler_EnableTask(const char* name)
{
    if(name = Null)
    {
        return ENotOk;
    }


}

Std_ReturnType Scheduler_DisableTask(const char* name)
{
    if(name = Null)
    {
        return ENotOk;
    }
}

uint32_t Scheduler_GetUptime_ms(void)
{
    uint32_t uptime;
    uint8_t sreg = SREG_Reg;
    GIE_Disable();
    uptime = g_sysTick_ms;   /* Atomic 4-byte copy */
    SREG_Reg = sreg;         /* Restore prior interrupt state */
    return uptime;
}

uint32_t Scheduler_GetUptime_ms(void)
{
    uint32_t uptime;
    uint8_t sreg = SREG_Reg;
    GIE_Disable();
    uptime = g_uptimeSeconds;   /* Atomic 4-byte copy */
    SREG_Reg = sreg;         /* Restore prior interrupt state */
    return uptime;
}

uint32_t Scheduler_GetTaskRunCount(const char* name);

void Scheduler_GetTelemetry(SchedulerTelemetry_t* statsPtr)
{
    statsPtr->sysTick_ms      = Scheduler_GetUptime_ms();
    statsPtr->uptimeSeconds   = Scheduler_GetUptime_s();
    statsPtr->activeTaskCount = g_activeTaskCount;
    statsPtr->overrunDetected = g_overrunDetected;
}

#endif  