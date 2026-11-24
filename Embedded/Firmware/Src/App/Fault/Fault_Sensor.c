/**
 * @file    Fault_Sensor.c
 * @brief   Analog sensor disconnect and harness fault detector implementation.
 * @details Implements debounce filtering, channel fault latching, recovery
 *          window verification, and fault clearing for analog sensor channels
 *          (ADC0 temperature and ADC1 battery voltage).
 * @version 1.0.0
 * @date    2026-11-20
 * @author  Rouaa Ayman <Rouaaayman2602@gmail.com>
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */
#include "Fault_Sensor.h"
/**
 * @struct ECU_SensorSharedData_t
 * @brief Shared ECU diagnostic data subset used by the sensor fault detector.
 */
typedef struct {
    DtcCode_t activeDTC;
} ECU_SensorSharedData_t;
/** @brief Shared ECU data instance defined externally. */
extern ECU_SensorSharedData_t g_ecuData;
/**
 * @fn FSM_Transition
 * @brief Requests an ECU state machine transition to targetState.
 * @param[in] targetState Target ECU state defined by EcuState_t.
 * @return Status of the requested transition.
 */
extern Std_ReturnType FSM_Transition(EcuState_t targetState);
/** @brief Internal control block instance for sensor fault detection. */
static FaultSensorControlBlock_t g_faultSensorControl;


/**
 * @fn Fault_Sensor_Init
 * @brief Initializes the sensor disconnect fault detector state.
 * @details Sets default ADC counts to 512, resets debounce counters and channel
 *          fault flags across all channels, and clears faultActive.
 * @return None.
 */

void Fault_Sensor_Init(void)
{
    uint8_t ch;
    for (ch = 0U ; ch < FAULT_SENSOR_NUM_CHANNELS; ch++)
    {
        g_faultSensorControl.rawAdcCounts[ch] = 512U;
        g_faultSensorControl.discTripCounter[ch] = 0U;
    g_faultSensorControl.channelFaultActive[ch] = false;   
    }
    g_faultSensorControl.faultActive = false;
return;
}

/**
 * @fn Fault_Sensor_Monitor
 * @brief Monitors raw ADC readings for short-to-ground and open-circuit faults.
 * @details Evaluates raw ADC counts on temperature and battery channels against
 *          saturation limits across consecutive cycles, updates per-channel fault
 *          flags, sets activeDTC to DtcF003SensorError, and calls FSM_Transition(StateFault).
 * @param[in] rawAdcTemp Raw 10-bit ADC count for the temperature channel.
 * @param[in] rawAdcBatt Raw 10-bit ADC count for the battery channel.
 * @return Sensor fault detection status.
 * @retval true A sensor disconnect fault is active or has just been detected.
 * @retval false No sensor disconnect fault has been detected.
 * @warning Fault_Sensor_Init() must be called before this function.
 */

bool Fault_Sensor_Monitor(uint16_t rawAdcTemp, uint16_t rawAdcBatt)
{
    g_faultSensorControl.rawAdcCounts[SENSOR_CH_TEMP] = rawAdcTemp;
    g_faultSensorControl.rawAdcCounts[SENSOR_CH_BATT] = rawAdcBatt;

    if (g_faultSensorControl.faultActive == true)
    {
        return true;
    }

    if ((rawAdcTemp <= FAULT_SENSOR_ADC_SHORT_LIMIT) ||
        (rawAdcTemp >= FAULT_SENSOR_ADC_OPEN_LIMIT))
    {
        g_faultSensorControl.discTripCounter[SENSOR_CH_TEMP]++;
    }
    else
    {
        g_faultSensorControl.discTripCounter[SENSOR_CH_TEMP] = 0U;
    }

    if ((rawAdcBatt <= FAULT_SENSOR_ADC_SHORT_LIMIT) ||
        (rawAdcBatt >= FAULT_SENSOR_ADC_OPEN_LIMIT))
    {
        g_faultSensorControl.discTripCounter[SENSOR_CH_BATT]++;
    }
    else
    {
        g_faultSensorControl.discTripCounter[SENSOR_CH_BATT] = 0U;
    }

    if ((g_faultSensorControl.discTripCounter[SENSOR_CH_TEMP] >= FAULT_SENSOR_DEBOUNCE_CYCLES) ||
        (g_faultSensorControl.discTripCounter[SENSOR_CH_BATT] >= FAULT_SENSOR_DEBOUNCE_CYCLES))
    {
        if (g_faultSensorControl.discTripCounter[SENSOR_CH_TEMP] >= FAULT_SENSOR_DEBOUNCE_CYCLES)
        {
            g_faultSensorControl.channelFaultActive[SENSOR_CH_TEMP] = true;
        }
        else
        {
            g_faultSensorControl.channelFaultActive[SENSOR_CH_TEMP] = false;
        }

        if (g_faultSensorControl.discTripCounter[SENSOR_CH_BATT] >= FAULT_SENSOR_DEBOUNCE_CYCLES)
        {
            g_faultSensorControl.channelFaultActive[SENSOR_CH_BATT] = true;
        }
        else
        {
            g_faultSensorControl.channelFaultActive[SENSOR_CH_BATT] = false;
        }

        g_faultSensorControl.faultActive = true;
        g_ecuData.activeDTC              = DtcF003SensorError;
        (void)FSM_Transition(StateFault);
        return true;
    }
    else
    {
        return false;
    }
}
/**
 * @fn Fault_Sensor_IsRecoveryAllowed
 * @brief Checks whether both sensor channels are within the valid operating window.
 * @details Checks that both rawAdcTemp and rawAdcBatt fall within
 *          [FAULT_SENSOR_ADC_VALID_MIN, FAULT_SENSOR_ADC_VALID_MAX].
 * @param[in] rawAdcTemp Raw 10-bit ADC count for the temperature channel.
 * @param[in] rawAdcBatt Raw 10-bit ADC count for the battery channel.
 * @return Recovery eligibility status.
 * @retval true Both channels are within the valid range.
 * @retval false Either channel is outside the valid range.
 */
bool Fault_Sensor_IsRecoveryAllowed(uint16_t rawAdcTemp, uint16_t rawAdcBatt)
{
bool ch0_ok = (rawAdcTemp >= FAULT_SENSOR_ADC_VALID_MIN && rawAdcTemp <= FAULT_SENSOR_ADC_VALID_MAX);
bool ch1_ok = (rawAdcBatt >= FAULT_SENSOR_ADC_VALID_MIN && rawAdcBatt <= FAULT_SENSOR_ADC_VALID_MAX);
if(ch0_ok && ch1_ok)
{
    return true;
}
return false;
}
/**
 * @fn Fault_Sensor_Clear
 * @brief Clears active sensor fault state and resets diagnostic tracking.
 * @details Resets debounce counters and channel fault flags, clears faultActive,
 *          and restores activeDTC to DtcNone when it matches DtcF003SensorError.
 * @return None.
 */
void Fault_Sensor_Clear(void)

{
    uint8_t ch;

    for (ch = 0U; ch < FAULT_SENSOR_NUM_CHANNELS; ch++)
    {
        g_faultSensorControl.discTripCounter[ch]    = 0U;
        g_faultSensorControl.channelFaultActive[ch] = false;
    }

    g_faultSensorControl.faultActive = false;

    if (g_ecuData.activeDTC == DtcF003SensorError)
    {
        g_ecuData.activeDTC = DtcNone;
    }
}
/**
 * @fn Fault_Sensor_IsActive
 * @brief Returns whether a sensor disconnect fault is currently active.
 * @details Reads and returns the faultActive flag from g_faultSensorControl.
 * @return Current sensor fault active state.
 * @retval true Sensor disconnect fault is active.
 * @retval false No sensor disconnect fault is active.
 */

bool Fault_Sensor_IsActive(void)
{
    return g_faultSensorControl.faultActive;
}