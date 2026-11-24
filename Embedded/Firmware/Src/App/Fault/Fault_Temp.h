#include <stdint.h>
#include <stdbool.h>
#ifndef FAULT_TEMP_H
#define FAULT_TEMP_H
#define FAULT_TEMP_THRESHOLD_MAX        90.0f
#define FAULT_TEMP_HYSTERESIS_C         75.0f
#define FAULT_TEMP_DEBOUNCE_CYCLES      2U

typedef struct {
    float       currentTemp_C;
    uint8_t     overTempTripCounter;
    bool        faultActive;
    uint32_t    faultTripTimestamp_ms;
} FaultTempControlBlock_t;

void           Fault_Temp_Init(void);
bool           Fault_Temp_Monitor(float currentTemp_C);
bool           Fault_Temp_IsRecoveryAllowed(float currentTemp_C);
void           Fault_Temp_Clear(void);
bool           Fault_Temp_IsActive(void);
#endif /* FAULT_TEMP_INTERFACE_H */