#ifndef _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_CONFIG_H
#define _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_CONFIG_H

/**
 * @file
 *      EXTI_Config.h
 * @brief
 *      Compile-time configuration of the External Interrupt (EXTI) driver.
 * @details
 *      Selects the trigger mode of INT4 (Ignition push-button on PE4) and INT5
 *      (Fault Reset push-button on PE5), which lines are armed during
 *      mEXTI_Init(), and whether ISR callbacks are compiled in.
 *      Sense modes are values of EXTI_SenseControl_t (EXTI_Sense_LowLevel,
 *      EXTI_Sense_FallingEdge, EXTI_Sense_RisingEdge). Options read by the
 *      preprocessor (#if) stay as macros because an #if cannot read an enum.
 * @version
 *      1.0.0
 * @author
 *      Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @date
 *      2026-10-13
 * @copyright
 *      Copyright (c) 2026 Gestell. All rights reserved.
 */

#include "Definition.h"

/**
 * @defgroup ExtiConfig EXTI Configuration Options
 * @brief    Compile-time options of the EXTI driver.
 * @{
 */

/**
 * @def ExtiIgnitionSenseMode
 * @brief Trigger mode of INT4 (Ignition button). Default: falling edge.
 */
#define ExtiIgnitionSenseMode EXTI_Sense_FallingEdge

/**
 * @def ExtiResetSenseMode
 * @brief Trigger mode of INT5 (Fault Reset button). Default: falling edge.
 */
#define ExtiResetSenseMode EXTI_Sense_FallingEdge

/**
 * @def ExtiIgnitionEnableOnInit
 * @brief Arms INT4 inside mEXTI_Init() when set to Enable.
 */
#define ExtiIgnitionEnableOnInit Enable

/**
 * @def ExtiResetEnableOnInit
 * @brief Arms INT5 inside mEXTI_Init() when set to Enable.
 */
#define ExtiResetEnableOnInit Enable

/**
 * @def ExtiCallbackEnabled
 * @brief Compiles ISR callback support when set to Enable.
 * @note  Set to Disable to obtain the shortest possible ISR execution time.
 */
#define ExtiCallbackEnabled Enable

/** @} */

#endif /* _EMBEDDED_FIRMWARE_SRC_MCAL_EXTI_EXTI_CONFIG_H */