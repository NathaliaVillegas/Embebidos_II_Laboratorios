#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

uint8_t counter = 0;

void Timer0A_Handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    counter++;
    if(counter > 10) counter = 0;

    GPIOPinWrite(GPIO_PORTA_BASE, 0x10, (counter >= 1) ? 0x10 : 0);
    GPIOPinWrite(GPIO_PORTA_BASE, 0x20, (counter >= 2) ? 0x20 : 0);
    GPIOPinWrite(GPIO_PORTN_BASE, 0x10, (counter >= 3) ? 0x10 : 0);
    GPIOPinWrite(GPIO_PORTN_BASE, 0x20, (counter >= 4) ? 0x20 : 0);
    GPIOPinWrite(GPIO_PORTK_BASE, 0x01, (counter >= 5) ? 0x01 : 0);
    GPIOPinWrite(GPIO_PORTK_BASE, 0x02, (counter >= 6) ? 0x02 : 0);
    GPIOPinWrite(GPIO_PORTK_BASE, 0x04, (counter >= 7) ? 0x04 : 0);
    GPIOPinWrite(GPIO_PORTK_BASE, 0x08, (counter >= 8) ? 0x08 : 0);
    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, (counter >= 9) ? 0x01 : 0);
    GPIOPinWrite(GPIO_PORTP_BASE, 0x10, (counter >= 10) ? 0x10 : 0);
}

int main(void)
{
    uint32_t ui32SysClock;

    ui32SysClock = SysCtlClockFreqSet(
        SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240,
        120000000
    );

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP));

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, 0x30);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x30);
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, 0x0F);
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, 0x01);
    GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE, 0x10);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32SysClock * 2);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    IntMasterEnable();

    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {}
}
