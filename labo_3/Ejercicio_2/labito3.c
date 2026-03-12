#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif
// Blink the on-board LED.
//int cont=0;

int main(void)
{
    volatile uint32_t ui32Loop;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }
    
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE,0x03);
    
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x11);
    
    //
    // Loop forever.
    //
    while(1)
    {
        // Turn on the LED.
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x01);
        // Delay 4 000 000 x seconds.
        SysCtlDelay(8000000);
        //for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
        //{}

        // Turn on the LEDs.
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x10);
        // Delay.
        SysCtlDelay(8000000);
        //for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++)
        //{}
        
        // Turn off the LED.
        GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x0);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x10, 0x0);
        // Delay 4 000 000 x seconds.
        SysCtlDelay(8000000);

        // Turn off the LED.
        GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x0);
        GPIOPinWrite(GPIO_PORTF_BASE, 0x01, 0x0);
        // Delay for a bit.
        SysCtlDelay(8000000);
    }
}
