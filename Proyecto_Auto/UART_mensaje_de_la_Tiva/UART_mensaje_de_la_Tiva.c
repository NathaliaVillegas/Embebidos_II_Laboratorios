//*****************************************************************************
#include <stdlib.h>

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/pin_map.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//Para PWM
//#include "driverlib/pwm.h"

//Para interrupciones y timer
//#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
//Para UART
#include "driverlib/uart.h"
#include "utils/uartstdio.c"

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
//*****************************************************************************
// Global variables 

// *********************** CAMBIO 1 *********************************
// Cambiamos a UART3 (aunque no usamos interrupciones)
void UART3_Handler(void)
{
    // VACÍO
}
// ******************************************************************

int main(void)
{
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    
    // *********************** CAMBIO 2 *********************************
    // ANTES UART0 + GPIOA
    // AHORA UART3 + GPIOA  <-- CORREGIDO (era GPIOC)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // ******************************************************************

    GPIOPinConfigure(GPIO_PA4_U3RX);

    // PA5 → TX
    GPIOPinConfigure(GPIO_PA5_U3TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    // ******************************************************************

    // *********************** CAMBIO 4 *********************************
    // Configurar UART3 manualmente (9600 8N1)
    UARTConfigSetExpClk(UART3_BASE, 120000000, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    // ******************************************************************

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {}

    // LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);

    // Botones PJ0 y PJ1
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Variables para anti-rebote (evitar spam en UART)
    bool lastPJ0 = 1;
    bool lastPJ1 = 1;

    while(1)
    {
        // Leer botones
        bool currentPJ0 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
        bool currentPJ1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1);


        if(currentPJ0 == 0)
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x01);

            if(lastPJ0 == 1)
            {
                char *msg = "motor1\r\n";
                for(int i = 0; msg[i] != '\0'; i++)
                {
                    UARTCharPut(UART3_BASE, msg[i]);
                }
            }
        }
        else
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x01, 0x00);
        }

        // *********************** CAMBIO 6 *********************************
        // BOTÓN PJ1 → LED + UART3
        if(currentPJ1 == 0)
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x02);

            if(lastPJ1 == 1)
            {
                char *msg = "motor2\r\n";
                for(int i = 0; msg[i] != '\0'; i++)
                {
                    UARTCharPut(UART3_BASE, msg[i]);
                }
            }
        }
        else
        {
            GPIOPinWrite(GPIO_PORTN_BASE, 0x02, 0x00);
        }
        // ******************************************************************

        lastPJ0 = currentPJ0;
        lastPJ1 = currentPJ1;

        Delay(0.05);
    }
}