/**
 * @file
 *     FAN_Private.h
 * @brief
 *     Private types and constants of the Actuator Fan PWM HAL driver.
 * @details
 *     Holds the fan operating mode enum, the runtime control block type and
 *     the internal duty-cycle constants used by FAN_Program.c (US-012).
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

#ifndef FAN_PRIVATE_H
#define FAN_PRIVATE_H

#include <stdint.h>

/**
 * @enum FanMode_t
 * @brief Operating modes of the cooling fan actuator.
 */
typedef enum {
    FAN_MODE_AUTO     = 0U,  /**< Speed automatically regulated by engine temperature */
    FAN_MODE_MANUAL   = 1U,  /**< Speed manually commanded (e.g. diagnostic tool / UI) */
    FAN_MODE_FAILSAFE = 2U   /**< Forced 100% full speed during faults or SAFE_MODE */
} FanMode_t;

/**
 * @struct FanControlBlock_t
 * @brief Runtime control descriptor for the radiator cooling fan.
 */
typedef struct {
    uint8_t   currentDutyPct;  /**< Currently active duty cycle percentage (0 to 100%) */
    FanMode_t mode;            /**< Current operating mode (AUTO, MANUAL, or FAILSAFE) */
} FanControlBlock_t;

/**
 * @def FAN_DUTY_MIN_PCT
 * @brief Duty cycle that stops the fan (0%).
 */
#define FAN_DUTY_MIN_PCT            (0U)

/**
 * @def FAN_DUTY_MAX_PCT
 * @brief Duty cycle for continuous full speed (100%).
 */
#define FAN_DUTY_MAX_PCT            (100U)

/**
 * @def FAN_PWM_FULL_SCALE_COUNT
 * @brief Timer0 compare value for 100% duty (OCR0 TOP = 255).
 * @details Passed to mTimer0_SetDutyCycle() by hFAN_ForceMax(). The Timer0
 *          driver handles 255 as a steady HIGH on PB4 (OC0 disconnected),
 *          which is the same end state as mTimer0_SetDutyPercentage(100U)
 *          without its software 16-bit division (AC-5 timing budget).
 */
#define FAN_PWM_FULL_SCALE_COUNT    (255U)

/**
 * @def FAN_TEMP_SCALED_PER_PCT
 * @brief Scaled temperature (x10) per 1% of duty in the modulation zone.
 * @details (FAN_TEMP_SCALED_MAX - FAN_TEMP_SCALED_MIN) / 100 = (900 - 400) / 100 = 5.
 */
#define FAN_TEMP_SCALED_PER_PCT     ((int16_t)(((int16_t)FAN_TEMP_SCALED_MAX - (int16_t)FAN_TEMP_SCALED_MIN) / (int16_t)100))

/**
 * @def FAN_PCT_SCALE_FLOAT
 * @brief Percentage scale factor used by the floating-point transfer function.
 */
#define FAN_PCT_SCALE_FLOAT         (100.0f)

#endif /* FAN_PRIVATE_H */
