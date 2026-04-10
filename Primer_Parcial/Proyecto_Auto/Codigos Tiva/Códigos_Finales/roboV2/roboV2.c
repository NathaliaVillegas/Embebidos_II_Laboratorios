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

// --- FUNCIONES AUXILIARES ---
void MandarString(const char *texto) {
    while(*texto != '\0') {
        UARTCharPut(UART3_BASE, *texto);
        texto++;
    }
}

void NumeroATexto(uint32_t num, char* buffer) {
    int i = 0, j = 0;
    char temp[15];
    if (num == 0) { buffer[0] = '0'; buffer[1] = '\0'; return; }
    while (num > 0) { temp[i++] = (num % 10) + '0'; num /= 10; }
    while (i > 0) { buffer[j++] = temp[--i]; }
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
    // TRIGGER: N5
    GPIOPinWrite(GPIO_PORTN_BASE, 0x20, 0x20);
    SysCtlDelay(400); 
    GPIOPinWrite(GPIO_PORTN_BASE, 0x20, 0x00);

    uint32_t limite = 100000;
    // ECHO: N4
    while(GPIOPinRead(GPIO_PORTN_BASE, 0x10) == 0 && --limite);
    if(limite == 0) return 999;

    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);
    TimerEnable(TIMER0_BASE, TIMER_A);

    limite = 400000;
    while(GPIOPinRead(GPIO_PORTN_BASE, 0x10) != 0 && --limite);
    TimerDisable(TIMER0_BASE, TIMER_A);

    uint32_t ciclos = 0xFFFFFFFF - TimerValueGet(TIMER0_BASE, TIMER_A);
    return (ciclos / 6996);
}

int main(void) {
    uint32_t reloj = SysCtlClockFreqSet(
        (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
        120000000);

    // Habilitar Periféricos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    // UART Config
    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x30);
    UARTConfigSetExpClk(UART3_BASE, reloj, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // PWM Config (Pines F1 y F2)
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, 0x06);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
    
    // Periodo para 1KHz aprox a 120MHz
    uint32_t periodo = 120000; 
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, periodo);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, periodo);
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT | PWM_OUT_2_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    // Botones, Buzzer y Sensores (Usando tus direcciones originales 0x01, 0x03, etc)
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, 0x03);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, 0x03, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, 0x01); // Buzzer en Q0 (0x01)
    
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x20); // N5 Salida
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, 0x10);  // N4 Entrada

    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, 0x06);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

    bool boton1_anterior = 1, boton2_anterior = 1;
    char buffer[20];
    int indice = 0, velocidad = 0;
    uint32_t contador_telemetria = 0;

    while(1) {
        bool boton1_actual = GPIOPinRead(GPIO_PORTJ_BASE, 0x01);
        bool boton2_actual = GPIOPinRead(GPIO_PORTJ_BASE, 0x02);
        uint32_t distancia = MedirDistancia();

        // Telemetría
        if(++contador_telemetria >= 30) {
            char texto[40] = "Distancia: ";
            char num[15];
            NumeroATexto(distancia, num);
            strcat(texto, num); strcat(texto, " cm\n");
            MandarString(texto);
            contador_telemetria = 0;
        }

        // PWM Unificado (CORRECCIÓN: Asegurar que el ancho no sea igual al periodo)
        uint32_t ancho = (periodo * velocidad) / 100;
        if (ancho >= periodo) ancho = periodo - 1; 
        if (velocidad == 0) ancho = 0;
        
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, ancho);

        // Seguridad
        if(distancia <= 3) {
            velocidad = 0;
            GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
            GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
        } else {
            // Motores
            if(boton1_actual == 0 && boton1_anterior == 1) {
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                MandarString("motor1\n");
            }
            if(boton2_actual == 0 && boton2_anterior == 1) {
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04);
                MandarString("motor2\n");
            }
        }
        boton1_anterior = boton1_actual;
        boton2_anterior = boton2_actual;

        // UART
        if(UARTCharsAvail(UART3_BASE)) {
            char c = UARTCharGetNonBlocking(UART3_BASE);
            if(c == '\n' || c == '\r') {
                buffer[indice] = '\0';
                indice = 0;
                if(strcmp(buffer, "buzzer") == 0) {
                    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, 0x01); // Encender
                    SysCtlDelay(reloj / 5);                   // Delay corto
                    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, 0x00); // Apagar
                } else if(buffer[0] == 'V') {
                    velocidad = ExtraerVelocidad(buffer);
                }
            } else if(indice < 19) {
                buffer[indice++] = c;
            }
        }
    }
}