#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

int main(void) {
    uint32_t reloj = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)) {}
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART3)) {}
    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)) {}
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x33);

    UARTConfigSetExpClk(UART3_BASE, reloj, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART0_BASE, reloj, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, 0x08);
    GPIOPinWrite(GPIO_PORTK_BASE, 0x08, 0);

    char buffer[20];
    int indice = 0;

    // Mensaje de inicio en tu laptop para confirmar que todo funciona
    char *msj_inicio = "TIVA lista. Esperando ordenes...\r\n";
    for(int i = 0; msj_inicio[i] != '\0'; i++) {
        UARTCharPut(UART0_BASE, msj_inicio[i]);
    }

    while(1) {
        // Escuchar a la Raspberry Pi en UART3
        if(UARTCharsAvail(UART3_BASE)) {
            char c = UARTCharGetNonBlocking(UART3_BASE);

            // ---> MÁNDAR ECO A LA LAPTOP POR UART0 <---
            UARTCharPut(UART0_BASE, c);
            
            // Lógica normal del programa
            if(c == '\n' || c == '\r') {
                buffer[indice] = '\0';
                indice = 0;

                if(strcmp(buffer, "buzzer") == 0) {
                    // Avisar a la laptop que la acción se ejecutó
                    char *msj_accion = " ¡Accion ejecutada!\r\n";
                    for(int i = 0; msj_accion[i] != '\0'; i++) {
                        UARTCharPut(UART0_BASE, msj_accion[i]);
                    }

                    GPIOPinWrite(GPIO_PORTK_BASE, 0x08, 0x08);
                    SysCtlDelay((reloj / 3) * 2);
                    GPIOPinWrite(GPIO_PORTK_BASE, 0x08, 0);
                }
            } else {
                if(indice < 19) {
                    buffer[indice] = c;
                    indice++;
                }
            }
        }
    }
}