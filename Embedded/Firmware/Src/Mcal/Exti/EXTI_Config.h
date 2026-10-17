/**
 * @file
 *    EXTI_Config.h
 * @brief
 *    EXTI driver Spre-compile configuration parameters.
 * @details
 *    Contains the trigger mode of INT4 / INT5 and the ISR callback switch.
 * @version
 *    1.0.0
 * @author
 *    Developer: Yasseen Ahmed EL-Sayed <yasseen.ahmed2005@gmail.com>
 * @author
 *    Reviewer: Beshoy Esmat
 * @date
 *    2026-10-17
 * @copyright
 *    Copyright (c) 2026 Gestell. All rights reserved.
 */

#ifndef EXTI_CONFIG_H
#define EXTI_CONFIG_H

#include "../../Common/Definition.h"
#include "EXTI_Private.h"

/**
 * @def EXTI_INT4_SENSE
 * @brief Trigger mode of INT4 (Ignition Push-Button) applied by mEXTI_Init().
 *
 * @details Available values:
 *          EXTI_Sense_LowLevel
 *          EXTI_Sense_FallingEdge
 *          EXTI_Sense_RisingEdge
 */
#define EXTI_INT4_SENSE        EXTI_Sense_FallingEdge

/**
 * @def EXTI_INT5_SENSE
 * @brief Trigger mode of INT5 (Fault Reset Push-Button) applied by mEXTI_Init().
 *
 * @details Available values: same as EXTI_INT4_SENSE.
 */
#define EXTI_INT5_SENSE        EXTI_Sense_FallingEdge

/**
 * @def EXTI_CALLBACK_ENABLE
 * @brief Compiles ISR callback support (Enable / Disable).
 *
 * @note Disable gives the shortest ISR: calling a function pointer forces
 *       the compiler to save all call-clobbered registers on ISR entry.
 */
#define EXTI_CALLBACK_ENABLE   Enable


#endif /* EXTI_CONFIG_H */