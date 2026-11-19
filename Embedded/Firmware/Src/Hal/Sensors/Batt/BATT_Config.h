/**
* 
* @file       BATT_Config.h
* @author     (Developer: Beshoy Esmat)
* @author     (Reviewer: Yassen Ahmed)
* @brief      Configuration file for the BATT driver
* @details    This header file defines the compile-time configuration of the BATT driver
* @version    1.0.0
* @date       22/10/2026
* @copyright  Copyright (c) 2026, Gestell Company
*/

#ifndef BATT_CONFIG_H_
#define BATT_CONFIG_H_


/* =========================================================================
   CONSTANTS & DEFINITIONS
   ========================================================================= */

   /**
 * @def BATT_ADC_CHANNEL
 * @brief ADC channel connected to battery voltage divider tap (PF1 / ADC1).
 */
#define BATT_ADC_CHANNEL            ADC_Channel1  /**< Connected to PF1 (ADC1) */

/**
 * @def BATT_SAMPLE_WINDOW_SIZE
 * @brief Number of samples in the rolling moving average noise filter.
 */
#define BATT_SAMPLE_WINDOW_SIZE     4U            /**< 4-sample circular filter */

/* =========================================================================
   OPERATING THRESHOLDS
   ========================================================================= */

/** @brief Under-voltage fault threshold in millivolts (10.5 V) */
#define BATT_VOLTAGE_MIN_MV         10500U

/** @brief Over-voltage fault threshold in millivolts (16.0 V) */
#define BATT_VOLTAGE_MAX_MV         16000U

/** @brief Under-voltage fault threshold in Volts (float) */
#define BATT_VOLTAGE_MIN_V          10.5f

/** @brief Over-voltage fault threshold in Volts (float) */
#define BATT_VOLTAGE_MAX_V          16.0f


#endif 