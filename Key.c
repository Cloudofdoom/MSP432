#include "msp.h"
#include <stdint.h>                            // Standard Integer - data type definitions
#include <stdio.h>                             // Standard I/O definitions
#include <stdarg.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

inline void uart_println(const char* str, ...)
{
  static char print_buffer[256];
  va_list lst;
  va_start(lst, str);
  vsnprintf(print_buffer, 256, str, lst);
  str = print_buffer;
  while (*str)
  {
    while (!(UCA0IFG & EUSCI_A_IFG_TXIFG));
    UCA0TXBUF = *str;
    str++;
  }
  while (!(UCA0IFG & EUSCI_A_IFG_TXIFG));
  UCA0TXBUF = '\r';
  while (!(UCA0IFG & EUSCI_A_IFG_TXIFG));
  UCA0TXBUF = '\n';
}

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

void buttons(void){
     if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == 0){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);       //turn on red
        uart_println("Switch 1 Pressed. RGB Set To Red");
        __delay_cycles(600000);
    }
    else if(GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == 0){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);       //turn on green
        uart_println("Switch 2 Pressed. RGB Set To Green");
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
}

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    /* Halting WDT  */
        MAP_WDT_A_holdTimer();

        /* Setting DCO to 12MHz */
           CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_3); // to select 3 MHZ

        /* Selecting P1.2 and P1.3 in UART mode */
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

             /* Configuring UART Module */
        MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

        /* Enable UART module */
        MAP_UART_enableModule(EUSCI_A0_BASE);

        Declare();
        while(1){
            buttons();
        }

}
