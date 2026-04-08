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

#include "driverlib/interrupt.h"
#include "driverlib/uart.h"

//*****************************************************************************

void Delay(float time){
    SysCtlDelay((time * 120000000 / 3));
}

// 🔥 NECESARIO PARA EL LINKER
void Timer0A_Handler(void)
{
}

// 🔥 MEDIR DISTANCIA
float medir_distancia()
{
    int t = 0;

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
    Delay(0.000002);

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_PIN_2);
    Delay(0.00001);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);

    while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3) == 0);

    while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3) != 0){
        t++;
        Delay(0.000001);
    }

    return t / 58.0;
}

//*****************************************************************************
int main(void)
{
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    UARTConfigSetExpClk(UART3_BASE, 120000000, 9600,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // GPIO
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {}

    // LEDs
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x03);

    // Botones originales
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTJ_BASE,
        GPIO_PIN_0 | GPIO_PIN_1,
        GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // 🔥 BOTÓN PF4
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4,
        GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // 🔥 SENSOR
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_3);

    bool lastPJ0 = 1;
    bool lastPJ1 = 1;
    bool lastPF4 = 1;
    bool modo_auto = false;

    while(1)
    {
        bool PJ0 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);
        bool PJ1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1);
        bool PF4 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);

        // IZQUIERDA
        if(PJ0 == 0 && lastPJ0 == 1){
            char *msg = "motor1\r\n";
            for(int i = 0; msg[i] != '\0'; i++)
                UARTCharPut(UART3_BASE, msg[i]);
        }

        // DERECHA
        if(PJ1 == 0 && lastPJ1 == 1){
            char *msg = "motor2\r\n";
            for(int i = 0; msg[i] != '\0'; i++)
                UARTCharPut(UART3_BASE, msg[i]);
        }

        // 🔥 TOGGLE AUTOMÁTICO
        if(PF4 == 0 && lastPF4 == 1){
            modo_auto = !modo_auto;
        }

        // 🔥 ENVÍO DISTANCIA
        if(modo_auto){
            int d = (int)medir_distancia();

            UARTCharPut(UART3_BASE, (d / 10) + '0');
            UARTCharPut(UART3_BASE, (d % 10) + '0');
            UARTCharPut(UART3_BASE, '\r');
            UARTCharPut(UART3_BASE, '\n');
        }

        lastPJ0 = PJ0;
        lastPJ1 = PJ1;
        lastPF4 = PF4;

        Delay(0.05);
    }
}
