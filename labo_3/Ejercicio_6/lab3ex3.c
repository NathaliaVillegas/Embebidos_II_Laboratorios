#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

int main(void)
{
    uint32_t ui32SysClock;
    uint8_t counter = 0; 

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

    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    while(1)
    {
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0)
        {
            SysCtlDelay(ui32SysClock/30); 
            if(counter < 15) { counter++; }
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) {}
            SysCtlDelay(ui32SysClock/30); 
        }

        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0)
        {
            SysCtlDelay(ui32SysClock/30);
            if(counter > 0) { counter--; }
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) {}
            SysCtlDelay(ui32SysClock/30); 
        }

        if(tabla[counter][1] == 1) GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        else GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);

        if(tabla[counter][0] == 1) GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
        else GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);

        if(tabla[counter][3] == 1) GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
        else GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);

        if(tabla[counter][2] == 1) GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
        else GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
    }
}
