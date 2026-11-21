#include "App/DTC/DTC_Manager_Interface.h"
void main(void)
{
}

DTC_Code_t DTC_GetActiveCode(void)
{
    for (uint8_t i = dtcCount; i > 0U; i--)
    {
        uint8_t idx = i - 1U;
        uint8_t flags = dtcLog[idx].statusFlags;

        if (((flags & DTC_FLAG_ACTIVE) != 0U) && ((flags & DTC_FLAG_CLEARED) == 0U))
        {
            if (dtcLog[idx].code != DTC_NONE)
            {
                return (DTC_Code_t)dtcLog[idx].code;
            }
        }
    }

    return DTC_NONE;
}