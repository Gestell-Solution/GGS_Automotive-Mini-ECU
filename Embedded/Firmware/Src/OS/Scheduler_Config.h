/**
* @file       Scheduler_Config.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer:  Yassen Ahmed )
* @brief      Configuration file for the Scheduler
* @details    This header file contains the configuration parameters for the Scheduler
* @version    1.0.0
* @date       16/11/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/


#ifndef SCHEDULER_CONFIG_H
#define SCHEDULER_CONFIG_H

/**
 * @def       SCHEDULER_MAX_TASKS
 * @brief     Maximum number of tasks supported in the static task table.
 * @details   Upper bound for registered periodic tasks (AC-1 & AC-5).
 */
#define SCHEDULER_MAX_TASKS        8U

/**
 * @def       SCHEDULER_TICK_MS
 * @brief     SysTick heartbeat resolution in milliseconds.
 * @details   Matches the MCAL Timer2 10ms CTC compare match period.
 */
#define SCHEDULER_TICK_MS          10U

/**
 * @def       SCHEDULER_TASK_NAME_LEN
 * @brief     Maximum string length of task names (including null terminator).
 */
#define SCHEDULER_TASK_NAME_LEN    16U

/**
 * @defgroup  TaskPeriods Core Cyclic Task Nominal Periods (ms)
 * @brief     Standard execution periods for Gestell Mini-ECU cyclic tasks.
 * @{
 */
#define TASK_PERIOD_CMD_PARSER     10U    /**< Serial command parser period (10 ms) */
#define TASK_PERIOD_FAULT_MONITOR  20U    /**< Safety fault monitor period (20 ms) */
#define TASK_PERIOD_ADC_SAMPLING   100U   /**< ADC sensor acquisition period (100 ms) */
#define TASK_PERIOD_LCD_REFRESH    200U   /**< Cockpit display refresh period (200 ms) */
#define TASK_PERIOD_TELEMETRY      500U   /**< Wireless telemetry broadcast period (500 ms) */
#define TASK_PERIOD_HEARTBEAT      1000U  /**< Status heartbeat LED period (1000 ms) */
/** @} */

#endif /* SCHEDULER_CONFIG_H */
