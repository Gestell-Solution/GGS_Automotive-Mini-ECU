#include"Mcal/Dio/DIO_Interface.h"
void main()
{
    uint8_t Ingintion_button=0,
    Fault_rest_button=0;
    /*lcd pins*/
    mDIO_InitGroup(GroupC,DIO_Output);
    /*cooling fan*/
    mDIO_InitPin(GroupB,Pin4,DIO_Output);
    /*uart RX wifi esp-01*/
    mDIO_InitPin(GroupD,Pin2,DIO_Input);
    /*uart TX wifi esp-01*/
    mDIO_InitPin(GroupD,Pin3,DIO_Output);
    /*power heartbeat green led toggle at 1Hz*/
    mDIO_InitPin(GroupD,Pin4,DIO_Output);
    /*engine status blue led*/
    mDIO_InitPin(GroupD,Pin5,DIO_Output);
    /*warning yellow led work during fault or safe mode 2Hz*/
    mDIO_InitPin(GroupD,Pin6,DIO_Output);
    /*buzzer alarm work during fault or safe mode high priority*/
    mDIO_InitPin(GroupD,Pin7,DIO_Output);
    /* USART0 RX Bluetooth HC-05*/
    mDIO_InitPin(GroupE,Pin0,DIO_Input);
    /* USART0 TX (Bluetooth HC-05*/
    mDIO_InitPin(GroupE,Pin1,DIO_Output);
    /* External Interrupt input (Ignition push button with internal pull-up*/
    mDIO_InitPin(GroupE,Pin4,DIO_Input);
    /*External Interrupt input (Fault Reset push button with internal pull-up*/
    mDIO_InitPin(GroupE,Pin5,DIO_Input);
    /*LM35 temperature sensor*/
    mDIO_InitPin(GroupF,Pin0,DIO_Input);
    /*Battery voltage divider*/
    mDIO_InitPin(GroupF,Pin1,DIO_Input);
    while(1){
    /*cooling fan*/
    mDIO_WritePin(GroupB,Pin4,Dio_High);
    /*power heartbeat green led toggle at 1Hz*/
       mDIO_TogglePin(GroupD,Pin4);
    /* External Interrupt input Ignition push button with internal pull-up*/
    Ingintion_button=mDIO_ReadPin(GroupE,Pin4);
    do{
       /*engine status blue led*/
       mDIO_SetPinHigh(GroupD,Pin5);
    }while(Ingintion_button==0);
    /*External Interrupt input Fault Reset push button with internal pull-up*/
    Fault_rest_button=mDIO_ReadPin(GroupE,Pin5);
    do{
        /*buzzer alarm work during fault or safe mode high priority*/
        mDIO_SetPinHigh(GroupD,Pin7);
        /*warning yellow led work during fault or safe mode 2Hz*/
        mDIO_SetPinHigh(GroupD,Pin6);
    }while(Fault_rest_button==0);
    }
}