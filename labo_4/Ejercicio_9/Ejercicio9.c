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

uint32_t FS = 120000000 * 5;
uint32_t counter = 1;

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


int main(void)
{
    //Clock Conifguration

    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    // Enable Peripherals
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }

    //Enable Timer Peripheral

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

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


    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);


    //
    // Loop forever.
    //
    while(1)
    {

    }
}
