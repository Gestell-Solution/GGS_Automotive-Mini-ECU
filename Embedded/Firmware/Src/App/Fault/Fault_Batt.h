/**
 * @file Fault_Batt.h
 * @brief Defines the battery voltage fault detector interface.
 *
 * @details This file provides the voltage thresholds, debounce configuration,
 *          control block data structure, and public function declarations
 *          required for detecting battery under-voltage and over-voltage
 *          conditions and checking recovery eligibility.
 *
 * @version 1.0.0
 * @date 2026-11-19
 * @author Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef FAULT_BATT_H
#define FAULT_BATT_H

#include "Definition.h"
#include <stdint.h>
#include <stdbool.h>
/**
 * @defgroup Fault_Batt Battery Voltage Fault Detector
 * @brief Battery voltage fault detection and recovery interface.
 * @{
 */

/**
 * @def FAULT_BATT_THRESHOLD_MIN_V
 * @brief Defines the minimum allowed battery voltage in volts.
 * @details A voltage strictly below this threshold indicates an
 *          under-voltage condition.
 */

#define FAULT_BATT_THRESHOLD_MIN_V      10.5f
/**
 * @def FAULT_BATT_THRESHOLD_MAX_V
 * @brief Defines the maximum allowed battery voltage in volts.
 * @details A voltage strictly above this threshold indicates an
 *          over-voltage condition.
 */

#define FAULT_BATT_THRESHOLD_MAX_V      16.0f
/**
 * @def FAULT_BATT_NOMINAL_LOW_V
 * @brief Defines the lower boundary of the nominal battery voltage range.
 * @details Recovery voltage must be greater than or equal to this value.
 */

#define FAULT_BATT_NOMINAL_LOW_V        12.0f
/**
 * @def FAULT_BATT_NOMINAL_HIGH_V
 * @brief Defines the upper boundary of the nominal battery voltage range.
 * @details Recovery voltage must be less than or equal to this value.
 */

#define FAULT_BATT_NOMINAL_HIGH_V       14.5f
/**
 * @def FAULT_BATT_DEBOUNCE_CYCLES
 * @brief Defines the number of consecutive fault cycles required for detection.
 * @details Three consecutive under-voltage or over-voltage evaluation
 *          cycles are required before the fault is confirmed.
 */

#define FAULT_BATT_DEBOUNCE_CYCLES      3U
/**
 * @struct FaultBattControlBlock_t
 * @brief Stores the battery voltage fault detector state.
 * @details Maintains the current battery voltage, under-voltage and
 *          over-voltage debounce counters, fault activation status,
 *          and fault activation timestamp.
 */

typedef struct {
    float       currentVoltage_V;
    uint8_t     underVoltTripCounter;
    uint8_t     overVoltTripCounter;
    bool        faultActive;
    uint32_t    faultTripTimestamp_ms;
} FaultBattControlBlock_t;

/**
 * @fn Fault_Batt_Init
 * @brief Initializes the battery voltage fault detector.
 * @details Initializes the monitored voltage to 12.6 V, resets both
 *          debounce counters, clears the fault activation flag,
 *          and resets the fault timestamp.
 * @return None.
 * @warning Must be called before using the battery fault monitoring APIs.
 */

void           Fault_Batt_Init(void);
/**
 * @fn Fault_Batt_Monitor
 * @brief Monitors battery voltage and detects voltage faults.
 * @details Evaluates the supplied battery voltage against the configured
 *          under-voltage and over-voltage thresholds. Updates the
 *          corresponding debounce counter and activates the fault
 *          when the required number of consecutive cycles is reached.
 * @param[in] currentVoltage_V Current battery voltage in volts.
 * @return Fault detection status.
 * @retval true A battery voltage fault is active or has been detected.
 * @retval false No active battery voltage fault has been detected.
 * @note The fault remains latched until Fault_Batt_Clear() is called.
 */

bool           Fault_Batt_Monitor(float currentVoltage_V);
/**
 * @fn Fault_Batt_IsRecoveryAllowed
 * @brief Checks whether the battery voltage is within the recovery range.
 * @details Checks whether the supplied voltage is within the inclusive
 *          nominal range defined by FAULT_BATT_NOMINAL_LOW_V and
 *          FAULT_BATT_NOMINAL_HIGH_V.
 * @param[in] currentVoltage_V Battery voltage to be checked in volts.
 * @return Recovery eligibility status.
 * @retval true Voltage is within the nominal recovery range.
 * @retval false Voltage is outside the nominal recovery range.
 * @note This function checks the voltage range only; it does not verify
 *       the required 200 ms stability duration.
 */

bool           Fault_Batt_IsRecoveryAllowed(float currentVoltage_V);
/**
 * @fn Fault_Batt_Clear
 * @brief Clears the battery voltage fault state.
 * @details Resets the under-voltage and over-voltage debounce counters,
 *          clears the fault activation flag, and resets the fault timestamp.
 * @return None.
 * @warning The implementation must enforce the recovery conditions
 *          before clearing an active fault.
 * @note Recovery requires a stable nominal voltage and operator reset
 *       according to the system requirements.
 */

void           Fault_Batt_Clear(void);
/**
 * @fn Fault_Batt_IsActive
 * @brief Checks whether the battery voltage fault is active.
 * @details Returns the current fault activation status maintained by
 *          the battery voltage fault detector.
 * @return Fault activation status.
 * @retval true The battery voltage fault is active.
 * @retval false The battery voltage fault is inactive.
 */
 
bool           Fault_Batt_IsActive(void);

#endif /* FAULT_BATT_H */