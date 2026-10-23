/**
 * @file
 *     FAN_Config.h
 * @brief
 *     Pre-compile configuration of the Actuator Fan PWM HAL driver.
 * @details
 *     Defines the thermal thresholds of the temperature-to-PWM transfer
 *     function (US-012 section 1.3) in degrees C and scaled x10.
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

#ifndef FAN_CONFIG_H
#define FAN_CONFIG_H

#include "../../../Common/Config.h"

/**
 * @def FAN_TEMP_THRESHOLD_MIN
 * @brief Lower temperature threshold for fan start (C).
 * @details Taken from EngineTempMin in Common/Config.h (40.0 C) so the
 *          system has a single source for the thermal limits.
 */
#define FAN_TEMP_THRESHOLD_MIN     (EngineTempMin)

/**
 * @def FAN_TEMP_THRESHOLD_MAX
 * @brief Upper temperature threshold for 100% fan (C).
 * @details Taken from EngineTempMax in Common/Config.h (90.0 C).
 */
#define FAN_TEMP_THRESHOLD_MAX     (EngineTempMax)

/**
 * @def FAN_TEMP_SCALED_MIN
 * @brief Lower threshold scaled x10 (40.0 C).
 * @warning Must stay equal to FAN_TEMP_THRESHOLD_MIN x 10.
 */
#define FAN_TEMP_SCALED_MIN        (400)

/**
 * @def FAN_TEMP_SCALED_MAX
 * @brief Upper threshold scaled x10 (90.0 C).
 * @warning Must stay equal to FAN_TEMP_THRESHOLD_MAX x 10, and
 *          (FAN_TEMP_SCALED_MAX - FAN_TEMP_SCALED_MIN) must be a multiple of 100.
 */
#define FAN_TEMP_SCALED_MAX        (900)

#endif /* FAN_CONFIG_H */
