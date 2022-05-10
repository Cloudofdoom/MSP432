#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        1,                                        // BRDIV = 1
        10,                                       // UCxBRF = 10
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

void UART(void){
     /* Setting DCO to 12MHz */
     CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3); // to select 3 MHZ

     /* Selecting P1.2 and P1.3 in UART mode */
     MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

     /* Configuring UART Module */
     MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

     /* Enable UART module */
     MAP_UART_enableModule(EUSCI_A0_BASE);
}

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

void buttons(void){
     if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);       //turn on red
        UART_transmitData(EUSCI_A0_BASE, 0x72);
        LED1();
        __delay_cycles(600000);
    }
    else if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == 0){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);       //turn on green
        UART_transmitData(EUSCI_A0_BASE, 0x47);
        __delay_cycles(600000);
    }
    else{
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
}

void Declare(void){
    //Set up
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, PIN_ALL16);
    GPIO_setAsOutputPin(GPIO_PORT_P2, 0x0007);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);

    //Setup LED
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void LED1(void);

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    /* Halting WDT  */
    MAP_WDT_A_holdTimer();

    UART();

    Declare();
    //LED1();
    //buttons();
    while(1){
        //LED1();
        buttons();

    }

}

void LED1(void){
    while(1){
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        __delay_cycles(600000);
        if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == 0){
            GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
            break;
        }
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        __delay_cycles(600000);
        if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == 0){
            break;
        }
    }
}
