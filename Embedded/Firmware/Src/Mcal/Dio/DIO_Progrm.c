/**
 * @file DIO_Program.c
 * @author DEV salma mohamed mahmoud fouad(salmafouadothman@gmail.com) 
 * @author REV Abdulrahman Ali
 * @brief   The DIO driver implementation 
 * @details Api used in dio driver like init pin or group
 *          write pin or group,read pin or group,toggle pin
 *          and lastly set pin high or low.
 * @version 0.1
 * @date 2026-10-14
 * @copyright Copyright (c) 2026,GESTELL Company
 */

#include"DIO_Interface.h"

/**
 * @brief Configures data direction (DIO_Input or DIO_Output) for an individual pin.
 * @param GroupName Target port group (GroupA through GroupG).
 * @param PinNo     Target pin index (Pin0 through Pin7).
 * @param DirectionState Desired direction (DIO_Input or DIO_Output).
 */
void mDIO_InitPin(uint8_t GroupName, uint8_t PinNo, uint8_t DirectionState){
    if(DirectionState==DIO_Output){
        switch (GroupName)
        {
        case GroupA:
            SetBit(DDRA_Reg,PinNo);
            break;
        case GroupB:
            SetBit(DDRB_Reg,PinNo);
            break;
        case GroupC:
            SetBit(DDRC_Reg,PinNo);
            break;
        case GroupD:
            SetBit(DDRD_Reg,PinNo);
            break;
        case GroupE:
            SetBit(DDRE_Reg,PinNo);
            break;
        case GroupF:
            SetBit(DDRF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            SetBit(DDRG_Reg,PinNo);
         }
            break;            
        }
    }
    else if(DirectionState==DIO_Input){
        switch (GroupName)
        {
        case GroupA:
            ClearBit(DDRA_Reg,PinNo);
            break;
        case GroupB:
            ClearBit(DDRB_Reg,PinNo);
            break;
        case GroupC:
            ClearBit(DDRC_Reg,PinNo);
            break;
        case GroupD:
            ClearBit(DDRD_Reg,PinNo);
            break;
        case GroupE:
            ClearBit(DDRE_Reg,PinNo);
            break;
        case GroupF:
            ClearBit(DDRF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            ClearBit(DDRG_Reg,PinNo);
         }
         break;            
        }    
   }
}

/**
 * @brief Writes a digital logic level (Dio_Low or Dio_High) to an output pin.
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 * @param OutputValue Logic level to write (Dio_Low or Dio_High).
 */
void mDIO_WritePin(uint8_t GroupName, uint8_t PinNo, uint8_t OutputValue){
    if(OutputValue==Dio_High){
        switch (GroupName)
        {
        case GroupA:
            SetBit(PORTA_Reg,PinNo);
            break;
        case GroupB:
            SetBit(PORTB_Reg,PinNo);
            break;
        case GroupC:
            SetBit(PORTC_Reg,PinNo);
            break;
        case GroupD:
            SetBit(PORTD_Reg,PinNo);
            break;
        case GroupE:
            SetBit(PORTE_Reg,PinNo);
            break;
        case GroupF:
            SetBit(PORTF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            SetBit(PORTG_Reg,PinNo);
         }
         break;            
        }
    }
    else if(OutputValue==Dio_Low){
        switch (GroupName)
        {
        case GroupA:
            ClearBit(PORTA_Reg,PinNo);
            break;
        case GroupB:
            ClearBit(PORTB_Reg,PinNo);
            break;
        case GroupC:
            ClearBit(PORTC_Reg,PinNo);
            break;
        case GroupD:
            ClearBit(PORTD_Reg,PinNo);
            break;
        case GroupE:
            ClearBit(PORTE_Reg,PinNo);
            break;
        case GroupF:
            ClearBit(PORTF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            ClearBit(PORTG_Reg,PinNo);
         }
         break;            
        }    
   }
}

/**
 * @brief Reads the current digital logic level of a pin from PINx register.
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 * @return uint8_t 1 (Dio_High) if HIGH, 0 (Dio_Low) if LOW.
 */
uint8_t mDIO_ReadPin(uint8_t GroupName, uint8_t PinNo){
    if((GroupName<=GroupG)&&(PinNo<=Pin7)){
     switch (GroupName)
        {
        case GroupA:
            return GetBit(PINA_Reg,PinNo);
            break;
        case GroupB:
            return GetBit(PINB_Reg,PinNo);
            break;
        case GroupC:
            return GetBit(PINC_Reg,PinNo);
            break;
        case GroupD:
            return GetBit(PIND_Reg,PinNo);
            break;
        case GroupE:
            return GetBit(PINE_Reg,PinNo);
            break;
        case GroupF:
            return GetBit(PINF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            return GetBit(PING_Reg,PinNo);
         }
            break;            
        }
    }
}

/**
 * @brief Toggles the current logic output state of a pin.
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 */
void mDIO_TogglePin(uint8_t GroupName, uint8_t PinNo){
    if (GroupName<=GroupG)
    {
        switch (GroupName)
        {
        case GroupA:
            ToggleBit(PORTA_Reg,PinNo);
            break;
        case GroupB:
            ToggleBit(PORTB_Reg,PinNo);
            break;
        case GroupC:
            ToggleBit(PORTC_Reg,PinNo);
            break;
        case GroupD:
            ToggleBit(PORTD_Reg,PinNo);
            break;
        case GroupE:
            ToggleBit(PORTE_Reg,PinNo);
            break;
        case GroupF:
            ToggleBit(PORTF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            ToggleBit(PORTG_Reg,PinNo);
         }
         break;            
        }
    }
}

/**
 * @brief Drives an output pin directly HIGH (1).
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 */
void mDIO_SetPinHigh(uint8_t GroupName, uint8_t PinNo){
    switch (GroupName)
        {
        case GroupA:
            SetBit(PORTA_Reg,PinNo);
            break;
        case GroupB:
            SetBit(PORTB_Reg,PinNo);
            break;
        case GroupC:
            SetBit(PORTC_Reg,PinNo);
            break;
        case GroupD:
            SetBit(PORTD_Reg,PinNo);
            break;
        case GroupE:
            SetBit(PORTE_Reg,PinNo);
            break;
        case GroupF:
            SetBit(PORTF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            SetBit(PORTG_Reg,PinNo);
         }
            break;            
        }
}

/**
 * @brief Drives an output pin directly LOW (0).
 * @param GroupName Target port group.
 * @param PinNo     Target pin index.
 */
void mDIO_SetPinLow(uint8_t GroupName, uint8_t PinNo){
    switch (GroupName)
        {
        case GroupA:
            ClearBit(PORTA_Reg,PinNo);
            break;
        case GroupB:
            ClearBit(PORTB_Reg,PinNo);
            break;
        case GroupC:
            ClearBit(PORTC_Reg,PinNo);
            break;
        case GroupD:
            ClearBit(PORTD_Reg,PinNo);
            break;
        case GroupE:
            ClearBit(PORTE_Reg,PinNo);
            break;
        case GroupF:
            ClearBit(PORTF_Reg,PinNo);
            break;
        case GroupG:
         if(PinNo<=Pin4){
            ClearBit(PORTG_Reg,PinNo);
         }
            break;            
        }    
}

/**
 * @brief Configures data direction for an entire 8-bit port group.
 * @param GroupName Target port group.
 * @param DirectionState Desired 8-bit direction mask (or DIO_Input / DIO_Output).
 */
void mDIO_InitGroup(uint8_t GroupName, uint8_t DirectionState){
    switch (GroupName)
      {
      case GroupA:
          DDRA_Reg=DirectionState;
          break;
      case GroupB:
          DDRB_Reg=DirectionState;
          break;
      case GroupC:
          DDRC_Reg=DirectionState;
          break;
      case GroupD:
          DDRD_Reg=DirectionState;
          break;
      case GroupE:
          DDRE_Reg=DirectionState;
          break;
      case GroupF:
          DDRF_Reg=DirectionState;
          break;
      case GroupG:
          DDRG_Reg=DirectionState;
          break;            
      }
}

/**
 * @brief Writes an 8-bit value directly to an entire port group.
 * @param GroupName Target port group.
 * @param OutputValue 8-bit value to write to PORTx.
 */
void mDIO_WriteGroup(DIO_GroupName_t GroupName, uint8_t OutputValue){
    switch (GroupName)
        {
        case GroupA:
            PORTA_Reg=OutputValue;
            break;
        case GroupB:
            PORTB_Reg=OutputValue;
            break;
        case GroupC:
            PORTC_Reg=OutputValue;
            break;
        case GroupD:
            PORTD_Reg=OutputValue;
            break;
        case GroupE:
            PORTE_Reg=OutputValue;
            break;
        case GroupF:
            PORTF_Reg=OutputValue;
            break;
        case GroupG:
            PORTG_Reg=OutputValue;
            break;            
        }
}

/**
 * @brief Reads the full 8-bit digital input value of an entire port group.
 * @param GroupName Target port group.
 * @return uint8_t Current 8-bit logic level from PINx.
 */
uint8_t mDIO_ReadGroup(DIO_GroupName_t GroupName){
    if(GroupName<=GroupG){
        switch (GroupName)
        {
        case GroupA:
            return PINA_Reg;
            break;
        case GroupB:
            return PINB_Reg;
            break;
        case GroupC:
            return PINC_Reg;
            break;
        case GroupD:
            return PIND_Reg;
            break;
        case GroupE:
            return PINE_Reg;
            break;
        case GroupF:
            return PINF_Reg;
            break;
        case GroupG:
            return PING_Reg;
            break;            
        }
    }
}