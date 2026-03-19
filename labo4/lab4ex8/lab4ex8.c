#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

uint8_t counter = 0;
uint32_t time15 = 180000000; //120000000*1.5
uint32_t time3 = 120000000*3;
const uint8_t tabla[16][4] = {
        {0, 0, 0, 0},
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {1, 1, 0, 0},
        {0, 0, 1, 0},
        {1, 0, 1, 0},
        {0, 1, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 1},
        {1, 0, 0, 1},
        {0, 1, 0, 1},
        {1, 1, 0, 1},
        {0, 0, 1, 1},
        {1, 0, 1, 1},
        {0, 1, 1, 1},
        {1, 1, 1, 1} 
    };

void timer0A_handler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    counter ++;
    if (counter > 15) {counter = 0;}
    if (tabla[counter][1] == 1)
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
    else GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x00);
    if (tabla[counter][0] == 1)
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);
    else GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x00);
    if (tabla[counter][3] == 1)
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
    else GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x00);
    if (tabla[counter][2] == 1)
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
    else GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x00);
}

int main(void)
{
    uint32_t ui32SysClock;

    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x03);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, 0x03, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, time15);

    IntMasterEnable();
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
        if(GPIOPinRead(GPIO_PORTJ_BASE, 0X01) == 0)
        {
            TimerLoadSet(TIMER0_BASE, TIMER_A, time3);
            SysCtlDelay(ui32SysClock/30);
            while(GPIOPinRead(GPIO_PORTJ_BASE, 0x01) == 0) {}
            SysCtlDelay(ui32SysClock/30); 
        }

        if(GPIOPinRead(GPIO_PORTJ_BASE, 0x02) == 0)
        {
            TimerLoadSet(TIMER0_BASE, TIMER_A, time15);
            SysCtlDelay(ui32SysClock/30);
            while(GPIOPinRead(GPIO_PORTJ_BASE, 0X02) == 0) {}
            SysCtlDelay(ui32SysClock/30); 
        }
    }
}
