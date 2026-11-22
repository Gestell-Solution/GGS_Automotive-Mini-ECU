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

#include "../../Common/Config.h"
#include "../../Common/Definition.h"

#if ModuleFsmEnabled == Enable

void State_RUN_OnEntry(void);
void State_RUN_OnRun(void);
void State_RUN_OnExit(void);

#endif