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


void Delay(float time){
    SysCtlDelay((time * 120000000 / 3));
}


int main(void)
{
    
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    // Enable Peripherals
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK))
    {
    }

    // Enable the GPIO pin for the LED
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, 0x0E);

    //
    // Loop forever.
    //
    while(1)
    {
        GPIOPinWrite(GPIO_PORTK_BASE, 0x0E,  0x02);
        Delay(10);
        GPIOPinWrite(GPIO_PORTK_BASE, 0x0E,  0x04);
        Delay(10);
        GPIOPinWrite(GPIO_PORTK_BASE, 0x0E,  0x08);
        Delay(10);

    }
}