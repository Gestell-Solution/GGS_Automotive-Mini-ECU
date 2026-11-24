/**
 * @file    Fault_Sensor.h
 * @brief   Analog sensor disconnect and harness fault detector interface.
 * @details Defines saturation thresholds, debounce parameters, channel identifiers,
 *          control block structure, and public APIs for detecting short-to-ground
 *          and open-circuit conditions on analog sensor harnesses.
 * @version 1.0.0
 * @date    2026-11-20
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */
#ifndef FAULT_SENSOR_H
#define FAULT_SENSOR_H
#include <stdint.h>
#include <stdbool.h>
#include "../../Common/Definition.h"
/**
 * @defgroup Fault_Sensor_Config Sensor Fault Detector Configuration
 * @brief Thresholds, debounce cycles, and channel count for harness monitoring.
 * @{
 */

/**
 * @def FAULT_SENSOR_ADC_SHORT_LIMIT
 * @brief Upper ADC count boundary classified as a short-to-ground condition.
 */
#define FAULT_SENSOR_ADC_SHORT_LIMIT    10U
/**
 * @def FAULT_SENSOR_ADC_OPEN_LIMIT
 * @brief Lower ADC count boundary classified as an open-circuit condition.
 */
#define FAULT_SENSOR_ADC_OPEN_LIMIT     1014U
/**
 * @def FAULT_SENSOR_ADC_VALID_MIN
 * @brief Minimum ADC count within the valid operating window for recovery.
 */
#define FAULT_SENSOR_ADC_VALID_MIN      50U
/**
 * @def FAULT_SENSOR_ADC_VALID_MAX
 * @brief Maximum ADC count within the valid operating window for recovery.
 */
#define FAULT_SENSOR_ADC_VALID_MAX      950U
/**
 * @def FAULT_SENSOR_DEBOUNCE_CYCLES
 * @brief Number of consecutive saturated samples required to trigger a fault.
 */
#define FAULT_SENSOR_DEBOUNCE_CYCLES    3U
/**
 * @def FAULT_SENSOR_NUM_CHANNELS
 * @brief Number of monitored analog ADC sensor channels.
 */
#define FAULT_SENSOR_NUM_CHANNELS       2U

/** @} */ /* End of Fault_Sensor_Config group */

/**
 * @enum SensorChannel_t
 * @brief Analog input channel identifiers monitored for harness faults.
 */
typedef enum {
    /** @var SensorChannel_t::SENSOR_CH_TEMP
     *  @brief Temperature sensor channel (ADC0 / PF0).
     */
    SENSOR_CH_TEMP = 0U,
    /** @var SensorChannel_t::SENSOR_CH_BATT
     *  @brief Battery voltage divider channel (ADC1 / PF1).
     */
    SENSOR_CH_BATT = 1U
} SensorChannel_t;


typedef struct {
    /**
 * @struct FaultSensorControlBlock_t
 * @brief Internal state and diagnostic tracking block for sensor fault detection.
 */
  /** @var FaultSensorControlBlock_t::rawAdcCounts
     *  @brief Latest raw ADC count readings per channel.
     */
    uint16_t rawAdcCounts[FAULT_SENSOR_NUM_CHANNELS];

    /** @var FaultSensorControlBlock_t::discTripCounter
     *  @brief Consecutive disconnect/saturation sample counter per channel.
     */
    uint8_t  discTripCounter[FAULT_SENSOR_NUM_CHANNELS];

    /** @var FaultSensorControlBlock_t::channelFaultActive
     *  @brief Active fault status flag per channel.
     */
    bool     channelFaultActive[FAULT_SENSOR_NUM_CHANNELS];

    /** @var FaultSensorControlBlock_t::faultActive
     *  @brief Overall sensor disconnect fault status flag.
     */
    bool     faultActive;
} FaultSensorControlBlock_t;

/**
 * @fn Fault_Sensor_Init
 * @brief Initializes the sensor disconnect fault detector state.
 * @details Resets raw ADC counts to nominal mid-scale (512), clears debounce
 *          counters and per-channel fault flags, and clears the active fault flag.
 * @return None.
 * @note Must be called before invoking any other Fault_Sensor_* API.
 */
void Fault_Sensor_Init(void);

/**
 * @fn Fault_Sensor_Monitor
 * @brief Monitors raw ADC readings for short-to-ground and open-circuit faults.
 * @details Updates stored channel counts, evaluates consecutive saturation cycles
 *          using debounce logic, latches DtcF003SensorError, and requests a
 *          transition to StateFault when the debounce limit is reached.
 * @param[in] rawAdcTemp Raw 10-bit ADC count for the temperature channel.
 * @param[in] rawAdcBatt Raw 10-bit ADC count for the battery channel.
 * @return Sensor fault detection status.
 * @retval true A sensor disconnect fault is active or has just tripped.
 * @retval false No sensor disconnect fault has tripped.
 * @warning Fault_Sensor_Init() must be called prior to using this function.
 */
bool Fault_Sensor_Monitor(uint16_t rawAdcTemp, uint16_t rawAdcBatt);

/**
 * @fn Fault_Sensor_IsRecoveryAllowed
 * @brief Checks whether both sensor channels are within the valid operating window.
 * @details Verifies that both temperature and battery ADC counts lie inclusively
 *          between FAULT_SENSOR_ADC_VALID_MIN and FAULT_SENSOR_ADC_VALID_MAX.
 * @param[in] rawAdcTemp Raw 10-bit ADC count for the temperature channel.
 * @param[in] rawAdcBatt Raw 10-bit ADC count for the battery channel.
 * @return Recovery eligibility status.
 * @retval true Both channels are within the valid non-saturated range.
 * @retval false At least one channel is outside the valid recovery window.
 */
bool Fault_Sensor_IsRecoveryAllowed(uint16_t rawAdcTemp, uint16_t rawAdcBatt);

/**
 * @fn Fault_Sensor_Clear
 * @brief Clears active sensor fault state and resets diagnostic tracking.
 * @details Clears per-channel debounce counters, per-channel fault flags, and
 *          the global fault flag, and resets activeDTC to DtcNone if set to
 *          DtcF003SensorError.
 * @return None.
 * @note Should be called after recovery conditions are satisfied and reset is triggered.
 */
void Fault_Sensor_Clear(void);

/**
 * @fn Fault_Sensor_IsActive
 * @brief Returns whether a sensor disconnect fault is currently active.
 * @details Reads the global faultActive flag from the internal control block.
 * @return Current sensor fault active state.
 * @retval true Sensor disconnect fault is active.
 * @retval false No sensor disconnect fault is active.
 */
bool Fault_Sensor_IsActive(void);

#endif /* FAULT_SENSOR_H */