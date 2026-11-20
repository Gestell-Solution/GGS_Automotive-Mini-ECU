/**
 * @file      Fault_Temp.h
 * @brief     Public interface for the engine over-temperature fault detector (US-022).
 * @details   Defines thermal thresholds, debounce parameters, the internal control
 *            block structure, and public APIs for monitoring coolant temperature,
 *            evaluating hysteresis recovery eligibility, clearing latched faults,
 *            and querying active fault state.
 * @version   1.0.0
 * @date      2026-11-17
 * @author    Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef FAULT_TEMP_H
#define FAULT_TEMP_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @def   FAULT_TEMP_THRESHOLD_MAX
 * @brief Critical coolant over-temperature threshold in degrees Celsius.
 */
#define FAULT_TEMP_THRESHOLD_MAX        90.0f

/**
 * @def   FAULT_TEMP_HYSTERESIS_C
 * @brief Coolant recovery hysteresis temperature threshold in degrees Celsius.
 */
#define FAULT_TEMP_HYSTERESIS_C         75.0f

/**
 * @def   FAULT_TEMP_DEBOUNCE_CYCLES
 * @brief Number of consecutive over-temperature evaluations required to latch a fault.
 */
#define FAULT_TEMP_DEBOUNCE_CYCLES      2U

/**
 * @struct FaultTempControlBlock_t
 * @brief  Internal state control block for the over-temperature fault detector.
 * @details Tracks the most recently monitored coolant temperature, consecutive
 *          debounce trip counter, fault latch flag, and activation timestamp.
 */
typedef struct {
    float       currentTemp_C;            /**< Latest monitored coolant temperature (°C) */
    uint8_t     overTempTripCounter;      /**< Consecutive over-temperature cycle counter */
    bool        faultActive;              /**< Latched fault status flag */
    uint32_t    faultTripTimestamp_ms;    /**< System uptime timestamp when fault latched (ms) */
} FaultTempControlBlock_t;

/**
 * @fn    void Fault_Temp_Init(void)
 * @brief Initializes the over-temperature fault detector control block.
 * @details Resets monitored temperature, debounce counter, fault active flag,
 *          and trip timestamp to their initial default values.
 * @return None.
 */
void Fault_Temp_Init(void);

/**
 * @fn    bool Fault_Temp_Monitor(float currentTemp_C)
 * @brief Evaluates coolant temperature against threshold and debounce rules.
 * @param[in] currentTemp_C Current engine coolant temperature in degrees Celsius.
 * @return Fault activation status.
 * @retval true  Fault is active or newly latched.
 * @retval false No active fault.
 */
bool Fault_Temp_Monitor(float currentTemp_C);

/**
 * @fn    bool Fault_Temp_IsRecoveryAllowed(float currentTemp_C)
 * @brief Checks whether coolant temperature satisfies recovery hysteresis.
 * @param[in] currentTemp_C Candidate coolant temperature in degrees Celsius.
 * @return Recovery eligibility status.
 * @retval true  Temperature is strictly below FAULT_TEMP_HYSTERESIS_C.
 * @retval false Temperature is equal to or above FAULT_TEMP_HYSTERESIS_C.
 */
bool Fault_Temp_IsRecoveryAllowed(float currentTemp_C);

/**
 * @fn    void Fault_Temp_Clear(void)
 * @brief Clears internal fault state when fault is active and hysteresis is met.
 * @details Uses the last monitored temperature stored in the control block to
 *          verify recovery eligibility before resetting fault state fields.
 * @return None.
 */
void Fault_Temp_Clear(void);

/**
 * @fn    bool Fault_Temp_IsActive(void)
 * @brief Queries whether the over-temperature fault is currently latched.
 * @return Current fault status.
 * @retval true  Over-temperature fault is active.
 * @retval false Over-temperature fault is inactive.
 */
bool Fault_Temp_IsActive(void);

#endif /* FAULT_TEMP_H */