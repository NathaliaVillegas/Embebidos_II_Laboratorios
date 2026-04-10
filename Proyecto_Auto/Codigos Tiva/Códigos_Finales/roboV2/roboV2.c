#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"

void MandarString(const char *texto) {
    while(*texto != '\0') {
        UARTCharPut(UART3_BASE, *texto);
        texto++;
    }
}

void NumeroATexto(uint32_t num, char* buffer) {
    int i = 0, j = 0;
    char temp[15];

    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    while (num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }
    while (i > 0) {
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
}

int ExtraerVelocidad(char* texto) {
    int v = 0;
    int i = 1;
    while(texto[i] != '\0') {
        if(texto[i] >= '0' && texto[i] <= '9') {
            v = v * 10 + (texto[i] - '0');
        }
        i++;
    }
    if(v > 100) v = 100;
    return v;
}

uint32_t MedirDistancia(void) {
    GPIOPinWrite(GPIO_PORTN_BASE, 0x10, 0x10);
    SysCtlDelay(400);
    GPIOPinWrite(GPIO_PORTN_BASE, 0x10, 0x00);

    uint32_t limite = 100000;
    while(GPIOPinRead(GPIO_PORTN_BASE, 0x20) == 0 && --limite);
    if(limite == 0) return 999;

    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);
    TimerEnable(TIMER0_BASE, TIMER_A);

    limite = 400000;
    while(GPIOPinRead(GPIO_PORTN_BASE, 0x20) != 0 && --limite);
    TimerDisable(TIMER0_BASE, TIMER_A);

    uint32_t ciclos = 0xFFFFFFFF - TimerValueGet(TIMER0_BASE, TIMER_A);
    return (ciclos / 6996);
}

int main(void) {
    uint32_t reloj = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART3)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {}
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) {}

    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x30);
    UARTConfigSetExpClk(UART3_BASE, reloj, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, 0x06);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 120000);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 120000);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 1);
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT | PWM_OUT_2_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x03);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, 0x03, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, 0x01);
    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, 0);

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x10);
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, 0x20);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

    bool boton1_anterior = 1;
    bool boton2_anterior = 1;
    char buffer[20];
    int indice = 0;
    uint32_t contador_telemetria = 0;
    
    int velocidad_m1 = 0;
    int velocidad_m2 = 0;
    bool moviendose = false;

    while(1) {
        bool boton1_actual = GPIOPinRead(GPIO_PORTJ_BASE, 0x01);
        bool boton2_actual = GPIOPinRead(GPIO_PORTJ_BASE, 0x02);
        uint32_t distancia = MedirDistancia();

        contador_telemetria++;
        if(contador_telemetria >= 20) { 
            char texto_distancia[40] = "Distancia: ";
            char num_str[15];
            NumeroATexto(distancia, num_str); 
            strcat(texto_distancia, num_str); 
            strcat(texto_distancia, " cm\n"); 
            MandarString(texto_distancia);    
            contador_telemetria = 0; 
        }

        uint32_t ancho_m1 = (120000 * velocidad_m1) / 100;
        if(ancho_m1 == 0) ancho_m1 = 1;
        uint32_t ancho_m2 = (120000 * velocidad_m2) / 100;
        if(ancho_m2 == 0) ancho_m2 = 1;

        if(distancia <= 3) {
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 1);
            velocidad_m1 = 0;
            velocidad_m2 = 0;
            if(moviendose) {
                MandarString("stop\n");
                moviendose = false;
            }
        } else {
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho_m1);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, ancho_m2);
        }

        if(distancia > 3) {
            if(boton1_actual == 0 && boton1_anterior == 1) {
                MandarString("motor1\n");
                moviendose = true;
            }
            if(boton2_actual == 0 && boton2_anterior == 1) {
                MandarString("motor2\n");
                moviendose = true;
            }
        }

        boton1_anterior = boton1_actual;
        boton2_anterior = boton2_actual;

        if(UARTCharsAvail(UART3_BASE)) {
            char c = UARTCharGetNonBlocking(UART3_BASE);
            if(c == '\n' || c == '\r') {
                buffer[indice] = '\0';
                indice = 0;
                
                if(strcmp(buffer, "buzzer") == 0) {
                    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, 0x01);
                    SysCtlDelay((reloj / 3) * 2);
                    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, 0);
                } 
                else if(buffer[0] == 'A') {
                    velocidad_m1 = ExtraerVelocidad(buffer);
                    velocidad_m2 = 0;
                }
                else if(buffer[0] == 'B') {
                    velocidad_m2 = ExtraerVelocidad(buffer);
                    velocidad_m1 = 0;
                }
                else if(buffer[0] == 'S') {
                    velocidad_m1 = ExtraerVelocidad(buffer);
                    velocidad_m2 = ExtraerVelocidad(buffer);
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