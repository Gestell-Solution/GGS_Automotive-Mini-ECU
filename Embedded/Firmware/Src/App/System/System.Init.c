/**
 * @file System.Init.c
 * @author salma mohamed (salmafouadothman@gmail.com)
 * @author roua ayman
 * @brief The implemintation for the system init
 * @details We run every layer and system we have here like mcal ,hal,app, os  
 * @version 0.1
 * @date 2026-11-19  
 * @copyright Copyright (c) 2026 GESTELL
 */
#include"System_Init.h"
#include"../../MCAL/Dio/DIO_Interface.h"
#include"../../MCAL/Adc/ADC_Interface.h"
#include"../../MCAL/Timer0/Timer0_Interface.h"
#include"../../MCAL/Timer2/TIMER2_interface.h"
#include"../../MCAL/Usart0/Usart0_Interface.h"
/**
 * @todo add the uart1 when it is wrote 
 */
#include"../../Mcal/Usart1/Usart1_Interface.h"
#include"../../MCAL/Exti/EXTI_Interface.h"
#include"../../HAL/Actuators/Buzzer/Buzzer_Interface.h"
#include"../../HAL/Actuators/Fan/FAN_Interface.h"
#include"../../HAL/Actuators/Led/LED_Interface.h"
#include"../../HAL/Lcd/Lcd_Interface.h"
#include"../../HAL/Sensors/Batt/BATT_Interface.h"
#include"../../HAL/Sensors/LM35/LM35_Interface.h"
/**
 * @todo add the bluetooth when it is wrote
 */
#include"../../HAL/Bluetooth/bluetooth_interface.h"
/**
 * @todo add the wifi when it is wrote 
 */
#include"../../HAL/Wifi/wifi_interface.h"
#include"../../APP/FMS/State_OFF.h"
#include"../../APP/DTC/DTC_Manager_Interface.h"
#include"../../APP/UI/LCD_Cockpit.h"
#include"../../OS/Scheduler/Scheduler_Interface.h"

static System_BootInfo_t systemBootInfo = {
    .resetReason     = RESET_REASON_UNKNOWN,
    .bootDuration_us = 0U,
    .initStatus      = SYS_INIT_OK,
    .watchdogArmed   = false
};

static void System_EvaluateResetSource(void)
{
    /* Step 1: Read MCUCSR register */
    uint8_t mcucsrValue = MCUCSR_Reg;

    /* Step 2: Clear MCUCSR register to prevent flag persistence */
    MCUCSR_Reg = 0x00U;

    /* Step 3: Decode reset reason */
    if (mcucsrValue & (1U << 0U)) {
        systemBootInfo.resetReason = RESET_REASON_POWER_ON;
    } else if (mcucsrValue & (1U << 3U)) {
        systemBootInfo.resetReason = RESET_REASON_WATCHDOG;
        /* Critical: Disable WDT early to prevent reset cycling */
        WDTCR_Reg |= (1U << 4U) | (1U << 3U);
        WDTCR_Reg = 0x00U;
    } else if (mcucsrValue & (1U << 2U)) {
        systemBootInfo.resetReason = RESET_REASON_BROWNOUT;
    } else if (mcucsrValue & (1U << 1U)) {
        systemBootInfo.resetReason = RESET_REASON_EXTERNAL;
    } else if (mcucsrValue & (1U << 4U)) {
        systemBootInfo.resetReason = RESET_REASON_JTAG;
    } else {
        systemBootInfo.resetReason = RESET_REASON_UNKNOWN;
    }
}
void System_Init_MCAL(void)
{
    /* 1. DIO Driver: Configure Pin Directions & Default Logic Levels */
   /* GROUP B*/
    mDIO_InitPin(GroupB,Pin4,DIO_Output);//fan 
    mDIO_WritePin(GroupB,Pin4,Dio_Low);
    /* GROUP C*/
    mDIO_InitPin(GroupC,Pin0,DIO_Output);//lcd rs
    mDIO_InitPin(GroupC,Pin1,DIO_Output);//lcd rw
    mDIO_WritePin(GroupC,Pin1,Dio_Low);//lcd rw
    mDIO_InitPin(GroupC,Pin2,DIO_Output);//lcd en
    mDIO_InitPin(GroupC,Pin4,DIO_Output);//lcd data bus
    mDIO_InitPin(GroupC,Pin5,DIO_Output);//lcd data bus
    mDIO_InitPin(GroupC,Pin6,DIO_Output);//lcd data bus
    mDIO_InitPin(GroupC,Pin7,DIO_Output);//lcd data bus
    /* GROUP D*/
    mDIO_InitPin(GroupD,Pin2,DIO_Input);//wifi rx
    mDIO_InitPin(GroupD,Pin3,DIO_Output);//wifi tx
    mDIO_InitPin(GroupD,Pin4,DIO_Output);//heart beat led
    mDIO_InitPin(GroupD,Pin5,DIO_Output);//engin states led
    mDIO_InitPin(GroupD,Pin6,DIO_Output);//warning led
    mDIO_InitPin(GroupD,Pin7,DIO_Output);//buzzer
    mDIO_WritePin(GroupD,Pin7,Dio_Low);//buzzer
    /* GROUP E*/
    mDIO_InitPin(GroupE,Pin0,DIO_Input);//BLUETOOTH RX
    mDIO_InitPin(GroupE,Pin1,DIO_Output);//BLUETOOTH TX
    mDIO_InitPin(GroupE,Pin4,DIO_Input);//IGNITION BUTTON INTERNAL PULL UP
    mDIO_InitPin(GroupE,Pin5,DIO_Input);//FAULT BUTTON
    /* GROUP F*/
    mDIO_InitPin(GroupF,Pin0,DIO_Input);//LM35
    mDIO_InitPin(GroupF,Pin1,DIO_Input);//BATTERY 

    /* 2. Analog-to-Digital Converter:
     *    - Reference: AVCC with external capacitor on AREF pin (ADMUX bit 6)
     *    - Prescaler: Division factor 128 -> 16 MHz / 128 = 125 kHz ADC Clock
     *    - Enable ADC module (ADCSRA bit 7)
     */
    mADC_Init();

    /* 3. Timer0: Radiator Fan PWM Controller:
     *    - Fast PWM Mode (WGM01:WGM00 = 1:1)
     *    - Non-inverting compare output on OC0 / PB4 (COM01:COM00 = 1:0)
     *    - Prescaler: Clock / 64 (CS02:CS00 = 0:1:1) -> 976.5 Hz PWM frequency
     *    - Initial Duty Cycle: 0 (Fan Stopped)
     */
    mTimer0_Init_FastPWM();


    /* 4. Timer2: Operating System SysTick Heartbeat:
     *    - CTC Mode (Clear Timer on Compare Match)
     *    - Prescaler: Clock / 1024
     *    - OCR2 Value: 155 -> Target Tick: (156 * 1024) / 16,000,000 = 9.984 ms (~10 ms)
     *    - Enable Output Compare Match Interrupt (TIMSK bit OCIE2)
     */
    mTimer2_Init_CTC_10ms();

    /* 5. USART0: Bluetooth HC-05 Interface:
     *    - Baud Rate: 9600 bps (UBRR0 = 103 at 16 MHz)
     *    - Frame Format: 8 Data bits, No Parity, 1 Stop bit (8N1)
     *    - Enable Transmitter, Receiver, and RX Complete Interrupt (RXCIE0)
     */
    mUSART0_Init();

    /* 6. USART1: Wi-Fi ESP-01 Interface:
     *    - Baud Rate: 115200 bps (UBRR1 = 8 at 16 MHz, U2X1 = 1)
     *    - Frame Format: 8N1
     *    - Enable Transmitter, Receiver, and RX Complete Interrupt (RXCIE1)
     */
    /**
     * @todo not done yet 
     */
    #if 0
    mUSART1_Init();
    #endif
    /* 7. External Interrupt Controller (EXTI):
     *    - INT4 (PE4 - Ignition Pushbutton): Falling-Edge Trigger
     *    - INT5 (PE5 - Fault Reset Pushbutton): Falling-Edge Trigger
     *    - Enable INT4 and INT5 mask bits in EIMSK
     */
    mEXTI_Init();
}

void System_Init_HAL(void)
{
    /* 1. Actuators Subsystem:
     *    - Ensure Radiator Fan is OFF (OCR0 = 0)
     *    - Ensure Piezoceramic Warning Buzzer is SILENT
     *    - Set Power Status LED (PD4) ON
     *    - Set Warning LED (PD5) OFF
     */
    hBUZZER_Init();
    hFAN_Init();
    hLED_Init();
    hLED_SetState(LED_POWER_GREEN,LED_STATE_ON);
    hLED_SetState(LED_WARNING_YELLOW,LED_STATE_ON);


    /* 2. HD44780 20x4 Character LCD:
     *    - Apply 40 ms hardware power-on delay
     *    - Send 4-bit bus synchronization sequence (0x03, 0x03, 0x03, 0x02)
     *    - Configure 2-line mode, 5x8 font (0x28)
     *    - Display ON, Cursor OFF, Blink OFF (0x0C)
     *    - Clear display and reset DDRAM address
     */
    hLCD_Init();

    /* 3. Upload Custom Automotive Glyphs to CGRAM:
     *    - Icon 0: Degree Symbol (°)
     *    - Icon 1: Battery Outline
     *    - Icon 2: Radiator Fan Propeller
     *    - Icon 3: Bluetooth Logo
     *    - Icon 4: Wi-Fi Signal Bars
     */
    /**
     * @todo not done yet
     */
    #if 0
    hLCD_LoadCustomGlyphs(); 
    #endif

    /* 4. Sensors Subsystem:
     *    - Perform dummy conversions on ADC0 (Temp) and ADC1 (Batt)
     *    - Establish initial analog baselines
     *    - Verify sensor disconnection thresholds
     */
    hLM35_Init();

    #if 0
    hBATT_Init();

    #endif
    /* 5. Communication Bridges:
     *    - Bind USART0 interrupt buffers to Bluetooth RingBuffer
     *    - Bind USART1 interrupt buffers to Wi-Fi RingBuffer
     */
    /**
     * @todo not done yet they are for the next phase
     */
    #if 0
    hBT_Init();
    hWiFi_Init();
    #endif
}
/**
 * @todo the os will be written in next phase
 */
void System_Init_OS(void)
{
    #if 0

    /* 1. Initialize Ring Buffers */
    RingBuf_Init();

    /* 2. Clear Event Flags Queue */
    EventFlags_Init();
    
    /* 3. Initialize SysTick Cooperative Scheduler */
    Scheduler_Init();

    /* 4. Register Cyclic Tasks (Task ID, Function, Period_ms, Priority) */
    Scheduler_RegisterTask(0U, Task_CmdParser,    10U,   1U);
    Scheduler_RegisterTask(1U, Task_FaultMonitor, 20U,   2U);
    Scheduler_RegisterTask(2U, Task_AdcSampling,  100U,  3U);
    Scheduler_RegisterTask(3U, Task_LcdRefresh,   200U,  4U);
    Scheduler_RegisterTask(4U, Task_Telemetry,    500U,  5U);
    Scheduler_RegisterTask(5U, Task_Heartbeat,    1000U, 6U);
    #endif
    /* 5. Configure & Arm Hardware Watchdog Timer (Timeout = 2048 ms) */
    /* Timed sequence: Set WDTOE and WDE, then write prescaler bits */

    uint8_t sreg = SREG_Reg;
    __asm__ __volatile__ ("cli" ::: "memory");
    WDTCR_Reg |= (1U << 4U) | (1U << 3U);
    WDTCR_Reg = (1U << 3U) | (1U << 2U) | (1U << 1U) | (1U << 0U);
    SREG_Reg = sreg;
    
    systemBootInfo.watchdogArmed = true;
    
}

void System_Init_APP(void)
{
    /* 1. Diagnostic Trouble Code Manager:
     *    - Read EEPROM header at 0x0000
     *    - If magic != 0xA5, format cleanly
     *    - Load stored records into SRAM cache
     */
    DTC_Init();

    /* 2. Anomaly Check: If reboot was caused by Watchdog, record DTC_F005 */
    if (systemBootInfo.resetReason == RESET_REASON_WATCHDOG) {
        DTC_Log(DtcF005Watchdog, 0U, (int16_t)systemBootInfo.resetReason);
    }

    /* 3. Cockpit UI Renderer:
     *    - Clear display buffer
     *    - Render Boot Splash Screen
     */
    /**
     * @todo waiting for the driver not sure what to write
     */
    #if 0
    LCD_Cockpit_Init();

    #endif
    /* 4. Telemetry Manager:
     *    - Reset transmission buffers
     */
    /**
     * @todo waiting for the driver not sure what to write
     */
    #if 0
    Telemetry_Init();
    #endif

    /* 5. Finite State Machine Core:
     *    - Initialize state variable to STATE_OFF
     *    - Set ignition event listeners
     */
    /**
     * @todo waiting for the driver not sure what to write
     */
    #if 0
    FSM_Init();
    State_OFF_OnEntry();
    #endif
}

void System_Init(void)
{
    
    /* Phase 0: Reset source analysis */
    System_EvaluateResetSource();

    /* Phase 1: Microcontroller Abstraction Layer */
    System_Init_MCAL();

    /* Phase 2: Hardware Abstraction Layer */
    System_Init_HAL();

    
    /* Phase 3: Operating System Services */
    System_Init_OS();

    
    /* Phase 4: Application Core Logic */
    System_Init_APP();

    systemBootInfo.initStatus = SYS_INIT_OK;
    
}

void System_EmergencyHalt(System_InitStatus_t errorReason)
{
    /* 1. Disable global interrupts */
    __asm__ __volatile__ ("cli" ::: "memory");

    /* 2. Force all actuators into safe de-energized state */
    mTimer0_SetDutyCycle(0U);  /* Cut fan PWM */
    mDIO_WritePin(GroupB, PIN4, 0U);
    mDIO_WritePin(GroupD, PIN7, 0U); /* Buzzer OFF */

    /* 3. Assert Warning LED solid */
    mDIO_WritePin(GroupD, Pin6, 1U); /* LED_WARNING ON */
    mDIO_WritePin(GroupD, PIN4, 0U); /* LED_PWR OFF */

    /* 4. Update system status */
    systemBootInfo.initStatus = errorReason;

    /* 5. Infinite hang to force hardware Watchdog reboot */
    while (1) {
        /* Do nothing; WDT will fire within 2048 ms */
    }
}
