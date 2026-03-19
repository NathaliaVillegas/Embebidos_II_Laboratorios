//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

//*****************************************************************************

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
// Global variables 

volatile uint32_t FS = 120000000 * 1;
uint32_t counter = 1;
int c = 1;

void interrupcion1(void){
    
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    switch (counter) {
        case 1:
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x01);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10,  0x00);
        counter ++;
        break;

        case 2:
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x00);
        counter ++;
        break;
        
        case 3:
        GPIOPinWrite(GPIO_PORTN_BASE, 0x03, 0x03);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
        counter = 1;
        break;
    }
}

void botoncito(void){
    GPIOIntClear(GPIO_PORTJ_BASE, GPIO_PIN_0);

    FS = 120000000 * c;
    c++;
    if (c == 6){
        c = 1;
    }

    TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
}

int main(void)
{
    //Clock Conifguration

    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    // Enable Peripherals
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ))
    {
    }

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    //Enable Timer Peripheral

    //Set Timmer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //Set the count time for timer
    TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    //Enable processor interrupts
    IntMasterEnable();
    //Enable Interrupt
    IntEnable(INT_TIMER0A);
    //Enable timer A interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //Enable the timer
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Enable the GPIO pin for the LED
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0);
    IntEnable(INT_GPIOJ);

    //
    // Loop forever.
    //
    while(1)
    {

    }
}