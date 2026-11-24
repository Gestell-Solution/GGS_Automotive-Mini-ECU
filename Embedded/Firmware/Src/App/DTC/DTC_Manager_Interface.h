#ifndef DTC_MANAGER_INTERFACE_H
#define DTC_MANAGER_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#include"../../Common/Definition.h"
#include"DTC_Config.h"
#include"DTC_Manager.h"
#include"../../Mcal/Atmega128Register.h"
#include <stddef.h>

#if 0
typedef enum {
    DTC_NONE                    = 0x00U,
    DTC_F001_OVER_TEMP          = 0x01U,
    DTC_F002_BATT_VOLT          = 0x02U,
    DTC_F003_SENSOR_DISC        = 0x03U,
    DTC_F004_COMMS_TIMEOUT      = 0x04U,
    DTC_F005_WATCHDOG_RESET     = 0x05U
} DtcCode_t;
#endif
typedef enum {
    DTC_SEVERITY_NONE           = 0x00U,
    DTC_SEVERITY_WARNING        = 0x01U,
    DTC_SEVERITY_CRITICAL       = 0x02U,
    DTC_SEVERITY_FATAL          = 0x03U
} DTC_Severity_t;

typedef struct __attribute__((packed)) {
    uint8_t  code;
    uint32_t timestamp_s;
    uint32_t lastTimestamp_s;
    uint8_t  occurrenceCount;
    uint8_t  severity;
    uint8_t  statusFlags;
    int16_t  freezeFrameData;
} DTC_Record_t;

typedef struct __attribute__((packed)) {
    uint8_t magic;
    uint8_t version;
    uint8_t checksum;
    uint8_t dtcCount;
} DTC_Header_t;

void DTC_Init(void);
void DTC_Log(DtcCode_t code, uint32_t timestamp_s, int16_t freezeFrame);
bool DTC_Clear(DtcCode_t code);
void DTC_ClearAll(void);

void DTC_GetAll(DTC_Record_t *outList, uint8_t *outCount);
uint8_t DTC_GetCount(void);
bool DTC_IsStored(DtcCode_t code);
DtcCode_t DTC_GetActiveCode(void);
const char* DTC_GetDescription(DtcCode_t code);
DTC_Severity_t DTC_GetSeverity(DtcCode_t code);

void DTC_LoadFromEEPROM(void);
void DTC_SaveToEEPROM(void);
void DTC_DumpAll(void);

uint8_t EEPROM_ReadByte(uint16_t address);
void EEPROM_WriteByte(uint16_t address, uint8_t data);
void EEPROM_ReadBlock(uint16_t address, void *dest, uint16_t length);
void EEPROM_WriteBlock(uint16_t address, const void *src, uint16_t length);

#endif /* DTC_MANAGER_INTERFACE_H */
