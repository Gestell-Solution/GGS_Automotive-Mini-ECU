/**
 * @file      State_RUN.h
 * @brief     Public interface header for the FSM State RUN.
 * @details   Contains public function APIs, configuration macros.
 * @version   1.0.0
 * @author    Developer: Naira Mohammed (naira.mohammed.76@gmail.com)
 * @author    Reviewer: Abdelrahman Ali
 * @date      2026-10-19
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef _STATE_RUN_H
#define _STATE_RUN_H

#define START_SELFTEST_DURATION_MS      2000U
#define RUN_HEARTBEAT_PERIOD_MS         1000U

typedef struct {
    uint32_t startEntryTimestamp_ms;
    uint8_t  selfTestStep;
    bool     selfTestComplete;
    bool     selfTestPassed;
} StartControlBlock_t;

void           State_START_OnEntry(void);
void           State_START_OnRun(void);
void           State_START_OnExit(void);

void           State_RUN_OnEntry(void);
void           State_RUN_OnRun(void);
void           State_RUN_OnExit(void);

bool           State_START_IsSelfTestPassed(void);

#endif