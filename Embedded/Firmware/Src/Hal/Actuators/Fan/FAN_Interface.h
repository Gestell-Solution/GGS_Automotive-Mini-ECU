/**
 * @file
 *     FAN_Interface.h
 * @brief
 *     Actuator Fan PWM HAL driver interface APIs.
 * @details
 *     Controls the 12V radiator cooling fan on PB4 / OC0 through the MCAL
 *     Timer0 Fast PWM driver (US-003): proportional speed from engine
 *     temperature, manual speed, failsafe full speed and stop (US-012).
 * @version
 *     1.0.0
 * @author
 *     Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *     Reviewer:  Naira Mohammed
 * @date
 *     2026-10-19
 * @copyright
 *    Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef FAN_INTERFACE_H
#define FAN_INTERFACE_H

#include <stdint.h>
#include "../../../Common/Definition.h"
#include "FAN_Private.h"
#include "FAN_Config.h"

/**
 * @fn void hFAN_Init(void)
 * @brief Initializes the cooling fan actuator.
 * @details Calls mTimer0_Init_FastPWM(), sets the mode to FAN_MODE_AUTO and
 *          applies 0% duty (fan stopped, PB4 LOW).
 * @note - Call once during ECU initialization before any other hFAN_ API.
 *       - Also releases FAN_MODE_FAILSAFE (the only API that returns the
 *         driver to FAN_MODE_AUTO from FAILSAFE without a manual command).
 * @warning Task context only. All hFAN_ APIs share one non-volatile control
 *          block and must not be called from an ISR.
 */
void           hFAN_Init(void);

/**
 * @fn Std_ReturnType hFAN_SetSpeed(uint8_t percent)
 * @brief Sets the fan speed to a specific percentage (0% to 100%).
 * @details Switches the mode to FAN_MODE_MANUAL and applies the duty cycle.
 *          Values above 100 are clamped to 100% and still applied.
 * @param[in] percent Target fan speed percentage (0 = OFF, 100 = Full Speed).
 * @return Std_ReturnType
 * @retval EOk    The requested value was applied as given.
 * @retval ENotOk The requested value was above 100 and was clamped to 100%.
 * @note Leaves FAN_MODE_FAILSAFE as specified in US-012 section 4.2. The
 *       caller (FSM) is responsible for rejecting manual commands in SAFE MODE.
 * @warning Task context only.
 */
Std_ReturnType hFAN_SetSpeed(uint8_t percent);

/**
 * @fn void hFAN_SetSpeedFromTemp(float temp_c)
 * @brief Computes and sets the fan speed from the engine temperature in C.
 * @details Transfer function (US-012 section 1.3):
 *          - temp_c <= 40.0 C           : 0%
 *          - 40.0 C < temp_c < 90.0 C   : (temp_c - 40.0) / 50.0 x 100% (truncated)
 *          - temp_c >= 90.0 C           : 100%
 *          Sets the mode to FAN_MODE_AUTO.
 * @param[in] temp_c Engine temperature in degrees Celsius.
 * @note - No effect while the mode is FAN_MODE_FAILSAFE (failsafe has priority).
 *       - A NaN input is treated as an invalid reading and gives 100% (safe side).
 *       - Uses software floating point; prefer hFAN_SetSpeedFromTemp_x10()
 *         in periodic tasks.
 * @warning Task context only.
 */
void           hFAN_SetSpeedFromTemp(float temp_c);

/**
 * @fn void hFAN_SetSpeedFromTemp_x10(int16_t temp_x10)
 * @brief Computes and sets the fan speed from the engine temperature scaled x10.
 * @details Integer transfer function (US-012 section 1.3), e.g. 425 = 42.5 C:
 *          - temp_x10 <= 400            : 0%
 *          - 400 < temp_x10 < 900       : (temp_x10 - 400) / 5 (truncated)
 *          - temp_x10 >= 900            : 100%
 *          Sets the mode to FAN_MODE_AUTO.
 * @param[in] temp_x10 Engine temperature in degrees Celsius scaled x10.
 * @note No effect while the mode is FAN_MODE_FAILSAFE (failsafe has priority).
 * @warning Task context only.
 */
void           hFAN_SetSpeedFromTemp_x10(int16_t temp_x10);

/**
 * @fn void hFAN_ForceMax(void)
 * @brief Forces the cooling fan to 100% maximum speed immediately.
 * @details Sets the mode to FAN_MODE_FAILSAFE and drives PB4 continuously HIGH.
 *          Temperature-based updates are ignored until hFAN_Init() or
 *          hFAN_SetSpeed() is called. Used during SAFE_MODE and DTC fault handling.
 * @note Calls mTimer0_SetDutyCycle(255U) instead of
 *       mTimer0_SetDutyPercentage(100U): identical PB4 output without the
 *       Timer0 software division, to meet AC-5 (<= 2 us).
 * @warning Task context only.
 */
void           hFAN_ForceMax(void);

/**
 * @fn void hFAN_Stop(void)
 * @brief Completely stops the cooling fan (0% duty cycle).
 * @details Applies 0% duty (PB4 LOW). The operating mode is not changed.
 * @note When called after hFAN_ForceMax() the mode stays FAN_MODE_FAILSAFE
 *       (US-012 section 4.6), so temperature updates remain blocked until
 *       hFAN_Init() or hFAN_SetSpeed() is called.
 * @warning Task context only.
 */
void           hFAN_Stop(void);

/**
 * @fn uint8_t hFAN_GetSpeed(void)
 * @brief Retrieves the currently active fan speed percentage.
 * @return uint8_t Current duty cycle (0 to 100%).
 */
uint8_t        hFAN_GetSpeed(void);

/**
 * @fn FanMode_t hFAN_GetMode(void)
 * @brief Retrieves the currently active fan operating mode.
 * @return FanMode_t Current operating mode (AUTO, MANUAL, or FAILSAFE).
 */
FanMode_t      hFAN_GetMode(void);

/**
 * @def Fan_Init
 * @brief Compatibility alias of hFAN_Init() (UML Actuators HAL naming).
 */
#define Fan_Init()                     hFAN_Init()

/**
 * @def Fan_SetDuty
 * @brief Compatibility alias of hFAN_SetSpeed().
 * @param pct Target fan speed percentage (0 to 100).
 */
#define Fan_SetDuty(pct)               hFAN_SetSpeed((pct))

/**
 * @def Fan_SetSpeed
 * @brief Compatibility alias of hFAN_SetSpeed().
 * @param pct Target fan speed percentage (0 to 100).
 */
#define Fan_SetSpeed(pct)              hFAN_SetSpeed((pct))

/**
 * @def Fan_SetSpeedFromTemp
 * @brief Compatibility alias of hFAN_SetSpeedFromTemp().
 * @param t Engine temperature in degrees Celsius.
 */
#define Fan_SetSpeedFromTemp(t)        hFAN_SetSpeedFromTemp((t))

/**
 * @def Fan_ForceMax
 * @brief Compatibility alias of hFAN_ForceMax().
 */
#define Fan_ForceMax()                 hFAN_ForceMax()

/**
 * @def Fan_Stop
 * @brief Compatibility alias of hFAN_Stop().
 */
#define Fan_Stop()                     hFAN_Stop()

/**
 * @def Fan_GetDutyPct
 * @brief Compatibility alias of hFAN_GetSpeed().
 */
#define Fan_GetDutyPct()               hFAN_GetSpeed()

/**
 * @def Fan_GetSpeed
 * @brief Compatibility alias of hFAN_GetSpeed().
 */
#define Fan_GetSpeed()                 hFAN_GetSpeed()

/**
 * @def Fan_GetMode
 * @brief Compatibility alias of hFAN_GetMode().
 */
#define Fan_GetMode()                  hFAN_GetMode()

#endif /* FAN_INTERFACE_H */
