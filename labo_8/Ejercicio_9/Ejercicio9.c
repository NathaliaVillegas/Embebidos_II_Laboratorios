//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"

//*****************************************************************************

#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

// ================= VARIABLE GLOBAL =================
volatile uint8_t objetos = 0;

// ================= INTERRUPCIÓN UART =================
void UART3_Handler(void)
{
    uint32_t status = UARTIntStatus(UART3_BASE, true);
    UARTIntClear(UART3_BASE, status);

    while(UARTCharsAvail(UART3_BASE))
    {
        char c = UARTCharGet(UART3_BASE);

        if(c == '1')
            objetos = 1;
        else if(c == '2')
            objetos = 2;
        else
            objetos = 0;
    }
}

// ================= MAIN =================
int main(void)
{
    // Clock a 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // ================= UART =================
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART3)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}

    // PA4 → RX, PA5 → TX
    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    UARTConfigSetExpClk(UART3_BASE, 120000000, 9600,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Habilitar interrupciones UART
    UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);
    IntEnable(INT_UART3);
    IntMasterEnable();

    // ================= LEDs =================
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}

    // PN0, PN1
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // PF0 y PF4 (ojo PF0 requiere unlock)
    GPIOUnlockPin(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // ================= LOOP =================
    while(1)
    {
        if(objetos == 1)
        {
            // 2 LEDs encendidos
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                                             GPIO_PIN_0 | GPIO_PIN_1);

            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4,
                                             0x00);
        }
        else if(objetos >= 2)
        {
            // 4 LEDs encendidos
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1,
                                             GPIO_PIN_0 | GPIO_PIN_1);

            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4,
                                             GPIO_PIN_0 | GPIO_PIN_4);
        }
        else
        {
            // Apagar todo
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0x00);
        }
    }
}