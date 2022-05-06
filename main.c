#include "msp.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
/*
*
* This program toggles  LED1 when S1 is pressed
*
*/
void LED1_Initialization(){
    // initialization - P1_0 for LED1 output
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}
void S1_initialization(){
    // initialization - P1_1 for switch button input
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
}
void main(void)
{
    // initializing GPIO (P1.0) for LED1
    LED1_Initialization();
    // initializing P1_1 for switch button input
    S1_initialization();
    // initializing GPIO Interrupt on P1.1
    Interrupt_disableMaster();                                                          // 1. start critical section by disabling all interrupts
    P1interrupt();
    Interrupt_enableMaster();                                                           // 7. end critical section by enabling all interrupts
    while(1){
        __WFI();    // (WatchForInterrupt) put MSP432 in sleep mode to save power until interrupt (S1 closed) kicks in
    }
}
void PORT1_IRQHandler(void)
{
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_clearInterruptFlag (GPIO_PORT_P1, GPIO_PIN1);
}

void P1interrupt(void)
{
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);      // 2. set GPIO interrupt on P1.1 with Falling edge trigger
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);                                   // 3. clear GPIO interrupt Flag corresponding to P1.1
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);                                      // 4. enable GPIO interrupt on P1.1
    Interrupt_setPriority(INT_PORT1, 2);                                                // 5. set priority of interrupt on P1.1
    Interrupt_enableInterrupt(INT_PORT1);                                               // 6. enable P1 interrupt in NVIC
}
