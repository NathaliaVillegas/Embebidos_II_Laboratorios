#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"    // <-- IMPORTANTE: Para las macros de desbloqueo
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#define PERIOD_1S   120000000
#define PERIOD_05S   60000000

volatile uint32_t g_ui32Periodo = PERIOD_1S;

void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Lectura del botón USR_SW1 (PJ0)
    if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0)
    {
        g_ui32Periodo = PERIOD_05S; // Rápido
    }
    else
    {
        g_ui32Periodo = PERIOD_1S;  // Lento
    }

    TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32Periodo - 1);

    // Toggle LED PN1
    uint32_t ui32LED = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, ui32LED ^ GPIO_PIN_1);
}

int main(void)
{
    uint32_t ui32SysClock;

    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                       SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));

    // --- PASO CLAVE: DESBLOQUEO DEL PUERTO J ---
    // El pin PJ0 necesita esto para poder activar el Pull-up
    HWREG(GPIO_PORTJ_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTJ_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    // -------------------------------------------

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Señal de vida (PN0 encendido fijo)
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, g_ui32Periodo - 1);

    IntMasterEnable();
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);

    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1) {}
}
