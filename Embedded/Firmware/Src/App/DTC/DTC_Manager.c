#include"DTC_Manager_Interface.h" 

uint8_t EEPROM_ReadByte(uint16_t address)
{
    /* Step 1: Wait for completion of previous write */
    while (EECR_Reg & (1U << 1U)) {
        /* Busy wait for EEWE to clear */
    }

    /* Step 2: Set up address registers */
    EEARH_Reg = (uint8_t)((address >> 8U) & 0x0FU);
    EEARL_Reg = (uint8_t)(address & 0xFFU);

    /* Step 3: Start EEPROM read by setting EERE (bit 0) */
    EECR_Reg |= (1U << 0U);

    /* Step 4: Return data from Data Register */
    return EEDR_Reg;
}

void EEPROM_WriteByte(uint16_t address, uint8_t data)
{
    /* Step 1: Wait for completion of previous write */
    while (EECR_Reg & (1U << 1U)) {
        /* Busy wait for EEWE to clear */
    }

    /* Step 2: Set up address and data registers */
    EEARH_Reg = (uint8_t)((address >> 8U) & 0x0FU);
    EEARL_Reg = (uint8_t)(address & 0xFFU);
    EEDR_Reg  = data;

    /* Step 3: Critical section to guarantee 4-clock execution */
    uint8_t sreg = SREG_Reg;
    __asm__ __volatile__ ("cli" ::: "memory");

    /* Step 4: Set Master Write Enable (EEMWE) bit 2 */
    EECR_Reg |= (1U << 2U);

    /* Step 5: Start EEPROM write by setting EEWE bit 1 (within 4 cycles) */
    EECR_Reg |= (1U << 1U);

    /* Step 6: Restore previous interrupt state */
    SREG_Reg = sreg;
}
void EEPROM_ReadBlock(uint16_t address, void *dest, uint16_t length)
{
    uint8_t *pDest = (uint8_t *)dest;
    for (uint16_t i = 0; i < length; i++) {
        pDest[i] = EEPROM_ReadByte(address + i);
    }
}

void EEPROM_WriteBlock(uint16_t address, const void *src, uint16_t length)
{
    const uint8_t *pSrc = (const uint8_t *)src;
    for (uint16_t i = 0; i < length; i++) {
        /* Optimization: Only write byte if content has actually changed */
        if (EEPROM_ReadByte(address + i) != pSrc[i]) {
            EEPROM_WriteByte(address + i, pSrc[i]);
        }
    }
}
static DTC_Record_t dtcCache[DTC_MAX_RECORDS];
static uint8_t      dtcCacheCount = 0;
static DtcCode_t   currentActiveDTC = DtcNone;

void DTC_Init(void)
{
    DTC_Header_t header;
    EEPROM_ReadBlock(DTC_EEPROM_BASE_ADDR, &header, sizeof(DTC_Header_t));

    /* Verify Magic Byte and Schema Version */
    if ((header.magic != DTC_MAGIC_BYTE) || (header.version != DTC_LAYOUT_VERSION)) {
        /* Uninitialized or corrupted EEPROM: format cleanly */
        header.magic = DTC_MAGIC_BYTE;
        header.version = DTC_LAYOUT_VERSION;
        header.checksum = (uint8_t)(header.magic ^ header.version);
        header.dtcCount = 0;

        EEPROM_WriteBlock(DTC_EEPROM_BASE_ADDR, &header, sizeof(DTC_Header_t));

        /* Clear all record slots in EEPROM */
        DTC_Record_t emptyRecord = {0};
        for (uint8_t i = 0; i < DTC_MAX_RECORDS; i++) {
            uint16_t addr = DTC_EEPROM_BASE_ADDR + sizeof(DTC_Header_t) + (i * sizeof(DTC_Record_t));
            EEPROM_WriteBlock(addr, &emptyRecord, sizeof(DTC_Record_t));
        }

        dtcCacheCount = 0;
        currentActiveDTC = DtcNone;
    } else {
        /* Valid storage found: Load records into RAM mirror */
        dtcCacheCount = (header.dtcCount > DTC_MAX_RECORDS) ? DTC_MAX_RECORDS : header.dtcCount;
        uint16_t recordsBaseAddr = DTC_EEPROM_BASE_ADDR + sizeof(DTC_Header_t);

        EEPROM_ReadBlock(recordsBaseAddr, dtcCache, (uint16_t)(dtcCacheCount * sizeof(DTC_Record_t)));

        /* Scan for previously active DTC that survived reboot */
        currentActiveDTC = DtcNone;
        for (uint8_t i = 0; i < dtcCacheCount; i++) {
            if ((dtcCache[i].statusFlags & DTC_FLAG_ACTIVE) && !(dtcCache[i].statusFlags & DTC_FLAG_CLEARED)) {
                currentActiveDTC = (DtcCode_t)dtcCache[i].code;
                break;
            }
        }
    }
}
void DTC_Log(DtcCode_t code, uint32_t timestamp_s, int16_t freezeFrame)
{
    if (code == DtcNone) {
        return;
    }

    int8_t foundIndex = -1;

    /* 1. Search for existing entry in RAM cache */
    for (uint8_t i = 0; i < dtcCacheCount; i++) {
        if (dtcCache[i].code == (uint8_t)code) {
            foundIndex = (int8_t)i;
            break;
        }
    }

    if (foundIndex >= 0) {
        /* Deduplicate: update occurrence counter and timestamps */
        if (dtcCache[foundIndex].occurrenceCount < 255U) {
            dtcCache[foundIndex].occurrenceCount++;
        }
        dtcCache[foundIndex].lastTimestamp_s = timestamp_s;
        dtcCache[foundIndex].freezeFrameData = freezeFrame;
        dtcCache[foundIndex].statusFlags |= (DTC_FLAG_ACTIVE | DTC_FLAG_MIL_REQ);
        dtcCache[foundIndex].statusFlags &= ~DTC_FLAG_CLEARED;

        /* Write updated record back to EEPROM */
        uint16_t recordAddr = DTC_EEPROM_BASE_ADDR + sizeof(DTC_Header_t) + 
                              ((uint16_t)foundIndex * sizeof(DTC_Record_t));
        EEPROM_WriteBlock(recordAddr, &dtcCache[foundIndex], sizeof(DTC_Record_t));
    } else {
        /* New DTC: Allocate slot */
        uint8_t targetSlot;
        if (dtcCacheCount < DTC_MAX_RECORDS) {
            targetSlot = dtcCacheCount;
            dtcCacheCount++;
        } else {
            /* FIFO / Eviction: overwrite slot 0 or oldest cleared entry */
            targetSlot = 0;
            for (uint8_t i = 0; i < DTC_MAX_RECORDS; i++) {
                if (dtcCache[i].statusFlags & DTC_FLAG_CLEARED) {
                    targetSlot = i;
                    break;
                }
            }
        }

        /* Populate record */
        dtcCache[targetSlot].code            = (uint8_t)code;
        dtcCache[targetSlot].timestamp_s     = timestamp_s;
        dtcCache[targetSlot].lastTimestamp_s = timestamp_s;
        dtcCache[targetSlot].occurrenceCount = 1U;
        dtcCache[targetSlot].severity        = (uint8_t)DTC_GetSeverity(code);
        dtcCache[targetSlot].statusFlags     = (DTC_FLAG_ACTIVE | DTC_FLAG_MIL_REQ);
        dtcCache[targetSlot].freezeFrameData = freezeFrame;

        /* Commit record to EEPROM */
        uint16_t recordAddr = DTC_EEPROM_BASE_ADDR + sizeof(DTC_Header_t) + 
                              ((uint16_t)targetSlot * sizeof(DTC_Record_t));
        EEPROM_WriteBlock(recordAddr, &dtcCache[targetSlot], sizeof(DTC_Record_t));

        /* Commit updated header count */
        uint16_t countAddr = DTC_EEPROM_BASE_ADDR + offsetof(DTC_Header_t, dtcCount);
        EEPROM_WriteByte(countAddr, dtcCacheCount);
    }

    currentActiveDTC = code;
}
bool DTC_Clear(DtcCode_t code)
{
    bool found = false;

    for (uint8_t i = 0; i < dtcCacheCount; i++) {
        if (dtcCache[i].code == (uint8_t)code) {
            dtcCache[i].statusFlags &= ~DTC_FLAG_ACTIVE;
            dtcCache[i].statusFlags &= ~DTC_FLAG_MIL_REQ;
            dtcCache[i].statusFlags |= DTC_FLAG_CLEARED;

            uint16_t recordAddr = DTC_EEPROM_BASE_ADDR + sizeof(DTC_Header_t) + 
                                  ((uint16_t)i * sizeof(DTC_Record_t));
            EEPROM_WriteBlock(recordAddr, &dtcCache[i], sizeof(DTC_Record_t));

            found = true;
            break;
        }
    }

    if (currentActiveDTC == code) {
        /* Re-evaluate active DTC */
        currentActiveDTC = DtcNone;
        for (uint8_t i = 0; i < dtcCacheCount; i++) {
            if ((dtcCache[i].statusFlags & DTC_FLAG_ACTIVE) && 
                !(dtcCache[i].statusFlags & DTC_FLAG_CLEARED)) {
                currentActiveDTC = (DtcCode_t)dtcCache[i].code;
                break;
            }
        }
    }

    return found;
}

void DTC_ClearAll(void)
{
    DTC_Record_t emptyRecord = {0};

    for (uint8_t i = 0; i < DTC_MAX_RECORDS; i++) {
        uint16_t addr = DTC_EEPROM_BASE_ADDR + sizeof(DTC_Header_t) + (i * sizeof(DTC_Record_t));
        EEPROM_WriteBlock(addr, &emptyRecord, sizeof(DTC_Record_t));
        dtcCache[i] = emptyRecord;
    }

    dtcCacheCount = 0;
    currentActiveDTC = DtcNone;

    /* Reset Header count in EEPROM */
    uint16_t countAddr = DTC_EEPROM_BASE_ADDR + offsetof(DTC_Header_t, dtcCount);
    EEPROM_WriteByte(countAddr, 0);
}
void DTC_GetAll(DTC_Record_t *outList, uint8_t *outCount)
{
    if ((outList != 0) && (outCount != 0)) {
        *outCount = dtcCacheCount;
        for (uint8_t i = 0; i < dtcCacheCount; i++) {
            outList[i] = dtcCache[i];
        }
    }
}

uint8_t DTC_GetCount(void)
{
    return dtcCacheCount;
}

bool DTC_IsStored(DtcCode_t code)
{
    for (uint8_t i = 0; i < dtcCacheCount; i++) {
        if (dtcCache[i].code == (uint8_t)code) {
            return true;
        }
    }
    return false;
}

DtcCode_t DTC_GetActiveCode(void)
{
    return currentActiveDTC;
}

const char* DTC_GetDescription(DtcCode_t code)
{
    switch (code) {
        case DtcF001OverTemp:      return "ENG OVER-TEMP >90C";
        case DtcF002BatteryFault:      return "BATT VOLT OUT-OF-RANGE";
        case DtcF003SensorError:    return "SENSOR HARNESS DISC";
        case DtcF004CommsTimeout:  return "COMMS LINK TIMEOUT >10S";
        case DtcF005Watchdog: return "WATCHDOG RESET EVENT";
        case DtcNone:
        default:                      return "NO FAULT DETECTED";
    }
}

DTC_Severity_t DTC_GetSeverity(DtcCode_t code)
{
    switch (code) {
        case DtcF001OverTemp:      return DTC_SEVERITY_CRITICAL;
        case DtcF002BatteryFault:      return DTC_SEVERITY_CRITICAL;
        case DtcF003SensorError:    return DTC_SEVERITY_FATAL;
        case DtcF004CommsTimeout:  return DTC_SEVERITY_WARNING;
        case DtcF005Watchdog: return DTC_SEVERITY_FATAL;
        case DtcNone:
        default:                      return DTC_SEVERITY_NONE;
    }
}
