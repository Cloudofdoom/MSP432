#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include <stdbool.h>

//![Simple Timer_A Config]
/* Timer_A PWM Configuration Parameter */
/*Timer_A_PWMConfig pwmConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,
        TIMER_A_CLOCKSOURCE_DIVIDER_1,
        32000,
        TIMER_A_CAPTURECOMPARE_REGISTER_1,
        TIMER_A_OUTPUTMODE_RESET_SET,
        3200
};*/
//![Simple Timer_A Config]

const Timer_A_ContinuousModeConfig continuousModeConfig =
{
     TIMER_A_CLOCKSOURCE_ACLK,
     TIMER_A_CLOCKSOURCE_DIVIDER_1,
     TIMER_A_TAIE_INTERRUPT_ENABLE,
     TIMER_A_DO_CLEAR
};

void setup_timer(int duty);

void setup_timer(int duty){
    // P6.6 in PSEL=01 mode and set as output
    P6->SEL0 |= 0x40;
    P6->SEL1 &= ~0x40;
    P6->DIR |= 0x40;

    TIMER_A2->CTL = 0x0210;
    TIMER_A2->CCR[0] = 3000;
    TIMER_A2->CCR[3] = 3000*(duty)/100;
    TIMER_A2->CCTL[3] = 0x00E0;

    return;
 }

int main(void)
{
    /* Halting the watchdog */
    MAP_WDT_A_holdTimer();

    setup_timer(60);

    /* Configuring P1.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    //![Simple Timer_A Example]
    /* Setting MCLK to REFO at 128Khz for LF mode
     * Setting SMCLK to 64Khz */
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    //MAP_CS_initClockSignal(CS_MCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

    /* Configuring GPIO2.4 as peripheral output for PWM  and P6.7 for button
     * interrupt */
    //MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring Continuous Mode */
    MAP_Timer_A_configureContinuousMode(TIMER_A0_BASE, &continuousModeConfig);

    /* Enabling interrupts and going to sleep */
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableInterrupt(INT_TA0_N);

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();

    /* Starting the Timer_A0 in continuous mode */
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);


    /* Configuring Timer_A to have a period of approximately 500ms and
     * an initial duty cycle of 10% of that (3200 ticks)  */
    //MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfig);
    //![Simple Timer_A Example]


    while (1)
    {
        __WFI();
    }
}

void TA0_N_IRQHandler(void){
    MAP_Timer_A_clearInterruptFlag(TIMER_A0_BASE);
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
}
