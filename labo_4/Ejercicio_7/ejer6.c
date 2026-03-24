#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"

// Definición de constantes para los retardos (Reloj a 120MHz)
#define FS_1_SEG    120000000  // [cite: 114, 116]
#define FS_05_SEG    60000000  // [cite: 171]

// Variable global para mantener el valor actual de carga
uint32_t ui32Periodo = FS_1_SEG;

// --- Manejador de la Interrupción del Timer 0A ---
// [cite: 102, 128, 130]
void timer0A_handler(void)
{
    // Limpiar la interrupción del Timer [cite: 142]
    TimerIntClear(TIMER0_BASE, TIMER_A);

    // Leer el Switch del usuario (Puerto J, Pin 0) [cite: 134, 171]
    // Si se presiona el switch (lectura == 0), cambiamos el intervalo
    if (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) 
    {
        ui32Periodo = FS_05_SEG; // 0.5 segundos [cite: 171]
    } 
    else 
    {
        ui32Periodo = FS_1_SEG;  // 1.0 segundo [cite: 171]
    }

    // Actualizar la carga del Timer para el siguiente ciclo [cite: 120, 140]
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Periodo);

    // Alternar (Toggle) el LED PN1 [cite: 168]
    if (GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1)) {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
    } else {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);
    }
}

int main(void)
{
    uint32_t ui32SysClock;

    // 1. Configurar el reloj del sistema a 120 MHz [cite: 107]
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | 
                                       SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // 2. Habilitar periféricos: Puerto N (LEDs), Puerto J (Switch) y Timer 0 [cite: 109, 113]
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Esperar a que los periféricos estén listos
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));

    // 3. Configurar PN1 como salida (LED) y PJ0 como entrada con Pull-up (Switch)
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // 4. Configurar el Timer 0 como periódico de 32 bits [cite: 119]
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Periodo); // Carga inicial de 1s [cite: 120]

    // 5. Configurar interrupciones [cite: 117, 122]
    IntMasterEnable();                 // Habilitar interrupciones globales [cite: 122]
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // Interrupción por timeout [cite: 126]
    IntEnable(INT_TIMER0A);            // Habilitar interrupción específica en el NVIC [cite: 124]

    // 6. Activar el Timer [cite: 127]
    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {
        // El procesador puede hacer otras tareas aquí [cite: 100, 166]
    }
}
