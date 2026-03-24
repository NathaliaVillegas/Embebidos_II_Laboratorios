#include "inc/hw_ints.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

uint32_t FS = 120000000; 

void timer0A_handler(void);

int main(void) {
    
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); 
    TimerLoadSet(TIMER0_BASE, TIMER_A, FS);

    IntMasterEnable();                               
    IntEnable(INT_TIMER0A);                          
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A); 

    while(1) {
    }
}

void timer0A_handler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_A);
    uint32_t estado_led = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, estado_led ^ GPIO_PIN_1);
}
