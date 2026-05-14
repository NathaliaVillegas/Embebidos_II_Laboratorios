//*****************************************************************************
// ROBOT TRACKER - TIVA C
// SOLO CONTROL DE MOTORES POR UART
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

//*****************************************************************************
// VARIABLES GLOBALES
//*****************************************************************************

volatile char buffer[20];

volatile int indice = 0;

volatile bool comando_listo = false;

//*****************************************************************************
// FUNCIONES MOTORES
//*****************************************************************************

void adelante(void)
{
    // Motor izquierdo adelante
    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02);

    // Motor derecho adelante
    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04);
}

void izquierda(void)
{
    // Motor izquierdo apagado
    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);

    // Motor derecho adelante
    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04);
}

void derecha(void)
{
    // Motor izquierdo adelante
    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02);

    // Motor derecho apagado
    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
}

void detener(void)
{
    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);

    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
}

//*****************************************************************************
// INTERRUPCIÓN UART
//*****************************************************************************

void UART3_Handler(void)
{
    uint32_t status;

    status = UARTIntStatus(UART3_BASE, true);

    UARTIntClear(UART3_BASE, status);

    while(UARTCharsAvail(UART3_BASE))
    {
        char c;

        c = UARTCharGet(UART3_BASE);

        // Fin de comando
        if(c == '\n' || c == '\r')
        {
            buffer[indice] = '\0';

            indice = 0;

            comando_listo = true;
        }

        // Guardar caracteres
        else if(indice < 19)
        {
            buffer[indice++] = c;
        }
    }
}

//*****************************************************************************
// MAIN
//*****************************************************************************

int main(void)
{
    uint32_t reloj;

    //=========================================================================
    // CLOCK
    //=========================================================================

    reloj = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),120000000);

    //=========================================================================
    // PERIFÉRICOS
    //=========================================================================

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART3));

    //=========================================================================
    // UART3
    // PA4 -> RX
    // PA5 -> TX
    //=========================================================================

    GPIOPinConfigure(GPIO_PA4_U3RX);

    GPIOPinConfigure(GPIO_PA5_U3TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_4 | GPIO_PIN_5);

    UARTConfigSetExpClk(UART3_BASE,reloj,9600,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //=========================================================================
    // INTERRUPCIONES UART
    //=========================================================================

    UARTIntEnable( UART3_BASE, UART_INT_RX | UART_INT_RT);

    IntEnable(INT_UART3);

    IntMasterEnable();

    //=========================================================================
    // GPIO MOTORES
    //=========================================================================

    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, 0x03);

    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, 0x06);

    //=========================================================================
    // ESTADO INICIAL
    //=========================================================================

    detener();

    //=========================================================================
    // LOOP PRINCIPAL
    //=========================================================================

    while(1)
    {
        if(comando_listo)
        {
            comando_listo = false;

            //=============================================================
            // FORWARD
            //=============================================================

            if(strcmp((char*)buffer, "FORWARD") == 0)
            {
                adelante();
            }

            //=============================================================
            // LEFT
            //=============================================================

            else if(strcmp((char*)buffer, "LEFT") == 0)
            {
                izquierda();
            }

            //=============================================================
            // RIGHT
            //=============================================================

            else if(strcmp((char*)buffer, "RIGHT") == 0)
            {
                derecha();
            }

            //=============================================================
            // STOP
            //=============================================================

            else if(strcmp((char*)buffer, "STOP") == 0)
            {
                detener();
            }
        }
    }
}