#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

int main(void) {
    uint32_t ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                               SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    int contador = 0; 

    while(1) {
        
        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) {
            SysCtlDelay(ui32SysClock / 12); /
            if(contador < 4) contador++;
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0); 
        }


        if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0) {
            SysCtlDelay(ui32SysClock / 12); /
            if(contador > 0) contador--;
            while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1) == 0); 
        }



        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, (contador >= 1) ? GPIO_PIN_1 : 0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, (contador >= 2) ? GPIO_PIN_0 : 0);
        
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, (contador >= 3) ? GPIO_PIN_4 : 0);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, (contador >= 4) ? GPIO_PIN_0 : 0); 
    }
}
