#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

const Timer_A_UpModeConfig upModeConfigP1 = {
    //CLK Source (SMCLK - 3MHz)
    TIMER_A_CLOCKSOURCE_SMCLK,
    //CLK Source Divider = 48 [3MHz/48 = 62500Hz CLK]
    TIMER_A_CLOCKSOURCE_DIVIDER_48,
    //Timer Period (Set Max Count Value [62500 = CLK = Interrupt Every Second)
    62500,
    //Interrupt Enable
    TIMER_A_TAIE_INTERRUPT_ENABLE,
    //Clear
    TIMER_A_DO_CLEAR
};

const Timer_A_UpModeConfig upModeConfigP11 = {
    //CLK Source (SMCLK - 3MHz)
    TIMER_A_CLOCKSOURCE_SMCLK,
    //CLK Source Divider = 48 [3MHz/48 = 62500Hz CLK]
    TIMER_A_CLOCKSOURCE_DIVIDER_48,
    //Timer Period (Set Max Count Value [62500 = CLK = Interrupt Every Second)
    7812,
    //Interrupt Enable
    TIMER_A_TAIE_INTERRUPT_ENABLE,
    //Clear
    TIMER_A_DO_CLEAR
};
void condition(void){
    if (UART_receiveData(EUSCI_A0_BASE) == 0x47){
        //Interrupt_disableInterrupt(INT_TA0_N);
        AlarmEnable();
    }
    else if (UART_receiveData(EUSCI_A0_BASE) == 0x72){
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
        //Interrupt_disableInterrupt(INT_TA0_N);
        AlarmTriggered();
    }
}

void buttons(void){
     if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);       //turn on red
        //UART_transmitData(EUSCI_A0_BASE, 0x72);
        //__delay_cycles(600000);
        //condition();
        //GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN7);
        AlarmEnable();
        //AlarmTriggered();

    }
     /*else if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7) == 0){
         //Interrupt_disableInterrupt(INT_TA0_N);
         //UART_transmitData(EUSCI_A0_BASE, 0x47);
         AlarmTriggered();
         //__delay_cycles(600000);
         //condition();
    }*/
    else if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == 0){
        Interrupt_disableInterrupt(INT_TA0_N);
        Interrupt_disableInterrupt(INT_TA2_N);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);       //turn on green
        //UART_transmitData(EUSCI_A0_BASE, 0x47);
        __delay_cycles(600000);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
    }
    else if(GPIO_getInputPinValue(GPIO_PORT_P2, GPIO_PIN4) == 1){
            Interrupt_disableInterrupt(INT_TA0_N);
            //Interrupt_disableInterrupt(INT_TA2_N);
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            //GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);       //turn on green
            //UART_transmitData(EUSCI_A0_BASE, 0x47);
            //__delay_cycles(600000);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);
            //GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
    }
    else{
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);

    }
}

void Declare(void){
    //Set up
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);

    //GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN7);
    //GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);
    //GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);

    //GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN4);
    //GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);

    GPIO_setAsInputPin(GPIO_PORT_P2, GPIO_PIN4);

    GPIO_setOutputLowOnPin(GPIO_PORT_P2, PIN_ALL16);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
}

void AlarmEnable(void);
void AlarmTriggered(void);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    /* Halting WDT  */
    MAP_WDT_A_holdTimer();

    Declare();

    while(1){

        buttons();
        //condition();
    }

}

void AlarmEnable(void){
        //Interrupt_disableInterrupt(INT_TA0_N);
        //Setup LED
        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

        GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);

        GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
        //Setup Timer_A0 (Can Choose Any Timer_A)
        Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfigP1);
        //Put to sleep and configure interrupt
        //Interrupt_enableSleepOnIsrExit();
        Interrupt_disableSleepOnIsrExit();
        //Enable interrupt
        Interrupt_enableInterrupt(INT_TA0_N);
        //Enabling MASTER interrupts
        Interrupt_enableMaster();
        //Start TimerA0
        Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

void AlarmTriggered(void){
        //Interrupt_disableInterrupt(INT_TA0_N);
        //Setup LED
        GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN6);
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);

        //GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);
        //GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN7);

        //Setup Timer_A0 (Can Choose Any Timer_A)
        Timer_A_configureUpMode(TIMER_A2_BASE, &upModeConfigP11);
        //Put to sleep and configure interrupt
        //Interrupt_enableSleepOnIsrExit();
        Interrupt_disableSleepOnIsrExit();
        //Enable interrupt
        Interrupt_enableInterrupt(INT_TA2_N);
        //Enabling MASTER interrupts
        Interrupt_enableMaster();
        //Start TimerA0
        Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);
        //GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN7);
}

void TA0_N_IRQHandler(void){
    MAP_Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN0);
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P5,GPIO_PIN6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
    //MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN6);
}
void TA2_N_IRQHandler(void){
    MAP_Timer_A_clearInterruptFlag(TIMER_A2_BASE);
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P5,GPIO_PIN6);
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1,GPIO_PIN7);
}


