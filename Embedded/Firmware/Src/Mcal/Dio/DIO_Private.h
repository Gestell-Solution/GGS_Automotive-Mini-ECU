/**
 * @file DIO_Private.h
 * @author DEV salma mohamed mahmoud fouad(salmafouadothman@gmail.com) 
 * @author REV Abdulrahman Ali 
 * @brief private defenation and enum
 * @details we ahve here group names from{A-G},pin number from{0-7} 
 *          driction state and output level
 * @version 0.1
 * @date 2026-10-14
 * @copyright Copyright (c) 2026 GESTELL Company
 */
#ifndef DIO_PRIVATE_H_
#define DIO_PRIVATE_H_

#include<stdint.h>
#include"../Atmega128Register.h"
#include"../../Common/Config.h"
#include"../../Common/Definition.h"
#include"../../Common/MacroFunction.h"

/**
 * @enum DIO_GroupName_t
 * @brief ATmega128 physical I/O ports.
 */
typedef enum {
    GroupA = 0,
    GroupB = 1,
    GroupC = 2,
    GroupD = 3,
    GroupE = 4,
    GroupF = 5,
    GroupG = 6
} DIO_GroupName_t;

/**
 * @enum DIO_PinNumber_t
 * @brief Physical pin index (0 to 7).
 */
typedef enum {
    Pin0 = 0,
    Pin1 = 1,
    Pin2 = 2,
    Pin3 = 3,
    Pin4 = 4,
    Pin5 = 5,
    Pin6 = 6,
    Pin7 = 7
} DIO_PinNumber_t;

/**
 * @enum DIO_DirectionState_t
 * @brief Data direction register modes.
 */
typedef enum {
    DIO_Input  = 0,
    DIO_Output = 1
} DIO_DirectionState_t;

/**
 * @enum DIO_OutputLevel_t
 * @brief Digital logic voltage levels.
 */
typedef enum {
    Dio_Low  = 0,
    Dio_High = 1
} DIO_OutputLevel_t;


#endif