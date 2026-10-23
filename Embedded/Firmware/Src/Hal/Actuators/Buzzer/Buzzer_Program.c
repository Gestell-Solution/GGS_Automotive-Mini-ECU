/**
 * @file Buzzer_Program.c
 * @author DEV salma mohamed mahmoud fouad(salmafouadothman@gmail.com) 
 * @author REV Abdulrahman Ali 
 * @brief  Driver control the buzzer 
 * @details this driver control the buzzer intilization it being 
 *          on or off it read the buzzer state  it can toggle the buzzer 
 *          we can make the buzzer "beep"at a certain frequency we can set it state
 *          to either on or off 
 * @version 0.1
 * @date 2026-10-21
 * @copyright Copyright (c) 2026 GESTELL
 */
#include"Buzzer_Interface.h"
 /**
  * @brief driver maintains a single static control block
  */
static BuzzerControlBlock_t g_buzzerControl;


/**
 * @brief Initializes GroupD Pin7 as a digital output and sets initial silent state.
 */
void hBUZZER_Init(void){
    g_buzzerControl=(BuzzerControlBlock_t){
        .group=Buzzer_GroupD,
        .pin=Buzzer_Pin7,
        .state =BUZZER_STATE_OFF,
        .mode =BUZZER_MODE_CONTINUOUS,
        .beepPeriod_ms=Low,
        .tickCounter_ms =Low,
    };
    mDIO_InitPin(Buzzer_GroupD,Buzzer_Pin7,DIO_Output);
    mDIO_WritePin(Buzzer_GroupD,Buzzer_Pin7,Dio_Low);
}

/**
 * @brief Sets the static state of the buzzer (ON or OFF).
 * @param state Desired state (BUZZER_STATE_OFF or BUZZER_STATE_ON).
 * @return Std_ReturnType EOk if successful, ENotOk if parameter invalid.
 */
Std_ReturnType hBUZZER_SetState(BuzzerState_t state){
    if((state!= BUZZER_STATE_OFF)&&(state!=BUZZER_STATE_ON)){
        return ENotOk;
    }
    g_buzzerControl=(BuzzerControlBlock_t){
        .state=state,
        .mode=BUZZER_MODE_CONTINUOUS,
        .tickCounter_ms =Low,
    };
    mDIO_WritePin(Buzzer_GroupD,Buzzer_Pin7,(state==BUZZER_STATE_ON)?Dio_High:Dio_Low);
    return EOk;
}

/**
 * @brief Activates continuous acoustic alarm tone.
 */
void hBUZZER_On(void){
    Buzzer_SetState(BUZZER_STATE_ON);
}

/**
 * @brief Deactivates and silences the buzzer immediately.
 */
void hBUZZER_Off(void){
    Buzzer_SetState(BUZZER_STATE_OFF);
}

/**
 * @brief Inverts the physical output pin logic level (used for tone synthesis).
 */
void hBUZZER_Toggle(void){
    mDIO_TogglePin(g_buzzerControl.group,g_buzzerControl.pin);
}

/**
 * @brief Configures intermittent beep mode at specified frequency.
 * @param beepFreq_hz Beep repetition frequency in Hz (e.g. 2 for 2 Hz beep).
 * @return Std_ReturnType EOk if valid, ENotOk if beepFreq_hz is 0.
 */
Std_ReturnType hBUZZER_SetBeep(uint8_t beepFreq_hz){
    if(beepFreq_hz==Low){
        return ENotOk;
    }
    uint16_t halfperiod=1000/(2*beepFreq_hz);
    g_buzzerControl=(BuzzerControlBlock_t){
        .beepPeriod_ms=halfperiod,
        .tickCounter_ms=Low,
        .mode = BUZZER_MODE_BEEP,
        .state = BUZZER_STATE_ON,
    };
    mDIO_WritePin(Buzzer_GroupD,Buzzer_Pin7,Dio_High);
    return EOk;
}

/**
 * @brief Inquires the current operational state of the buzzer.
 * @return BuzzerState_t BUZZER_STATE_ON if active, BUZZER_STATE_OFF if silent.
 */
BuzzerState_t hBUZZER_GetState(void){
    return g_buzzerControl.state;
}

/**
 * @brief Periodic time-slice handler for non-blocking pulsed beeping.
 *        Called by OS scheduler (e.g. every 10 ms).
 * @param step_ms Time elapsed in milliseconds since last call.
 */
void hBUZZER_UpdatePeriodic(uint16_t step_ms){
    if((step_ms==0)|(g_buzzerControl.mode!=BUZZER_MODE_BEEP)|g_buzzerControl.state!=BUZZER_STATE_ON){
        g_buzzerControl.tickCounter_ms+=step_ms;
    }
    if(g_buzzerControl.tickCounter_ms>=g_buzzerControl.beepPeriod_ms){
        g_buzzerControl.tickCounter_ms=Low;
        mDIO_TogglePin(g_buzzerControl.group,g_buzzerControl.pin);
    }
    return;
}
