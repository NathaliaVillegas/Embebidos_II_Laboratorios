//*****************************************************************************


#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"


//Para desbloquear PF0
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

//Para interrupciones y timer
//#include "driverlib/timer.h"
//#include "driverlib/interrupt.h"

//*****************************************************************************

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

void Delay(float time){
    SysCtlDelay((time * 120000000 / 3));
}

//*****************************************************************************
// Global variables 


int main(void)
{
    //Clock Conifguration

    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    // Enable Peripherals
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //PWM configure
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);


    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
    {
    }

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }
    //Habilitar el clock para pwm
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
    
    //Configurar PWM
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 400);
    //Configurar DutyCycle
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 400*0); //0%de duty cycle

    //Activar PWM
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);

    // Desbloquear PF0
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;

    //Enable Timer Peripheral
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x04);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 0x08);
    //

    //
    GPIOPinWrite(GPIO_PORTF_BASE, 0x0C, 0x04);
    // Loop forever.
    //
    while(1)
    {
        for(int duty = 0; duty <= 100; duty++)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, (400 * duty)/100);
        Delay(0.3);
        
    }
    }
}