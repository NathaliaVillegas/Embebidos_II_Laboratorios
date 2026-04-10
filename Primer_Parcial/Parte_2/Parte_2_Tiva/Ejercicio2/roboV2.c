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
#include "driverlib/interrupt.h"


volatile uint32_t ticks_giro = 0;
volatile int modo_giro = 0; 
uint32_t periodo = 120000;
int velocidad_global = 0;

void Timer1AIntHandler(void) {
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    if (ticks_giro > 0) ticks_giro--;
}


void MandarString(const char *texto) {
    while(*texto != '\0') { UARTCharPut(UART3_BASE, *texto); texto++; }
}

void NumeroATexto(uint32_t num, char* buffer) {
    int i = 0, j = 0; char temp[15];
    if (num == 0) { buffer[0] = '0'; buffer[1] = '\0'; return; }
    while (num > 0) { temp[i++] = (num % 10) + '0'; num /= 10; }
    while (i > 0) { buffer[j++] = temp[--i]; }
    buffer[j] = '\0';
}

int ExtraerNumero(char* texto) {
    int v = 0; int i = 1;
    while(texto[i] >= '0' && texto[i] <= '9') {
        v = v * 10 + (texto[i] - '0');
        i++;
    }
    return v;
}

uint32_t MedirDistancia(void) {
    GPIOPinWrite(GPIO_PORTN_BASE, 0x20, 0x20);
    SysCtlDelay(400); 
    GPIOPinWrite(GPIO_PORTN_BASE, 0x20, 0x00);
    uint32_t limite = 100000;
    while(GPIOPinRead(GPIO_PORTN_BASE, 0x10) == 0 && --limite);
    if(limite == 0) return 999;
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);
    TimerEnable(TIMER0_BASE, TIMER_A);
    limite = 400000;
    while(GPIOPinRead(GPIO_PORTN_BASE, 0x10) != 0 && --limite);
    TimerDisable(TIMER0_BASE, TIMER_A);
    return (0xFFFFFFFF - TimerValueGet(TIMER0_BASE, TIMER_A)) / 6996;
}

int main(void) {
    uint32_t reloj = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // Periféricos Enables
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    // UART
    GPIOPinConfigure(GPIO_PA4_U3RX); GPIOPinConfigure(GPIO_PA5_U3TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x30);
    UARTConfigSetExpClk(UART3_BASE, reloj, 9600, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

    // PWM Configuración
    GPIOPinConfigure(GPIO_PF1_M0PWM1); GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, 0x06);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, periodo);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, periodo);
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT | PWM_OUT_2_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0); PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    // Pines
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, 0x01); 
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, 0x20); 
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, 0x10); 
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, 0x03); 
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, 0x06);

    // Timer
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, reloj / 100); 
    TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER1_BASE, TIMER_A);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

    char buffer[20];
    int indice = 0;
    uint32_t cont_telem = 0;

    while(1) {
        uint32_t dist = MedirDistancia();

        if(++cont_telem >= 40) {
            char t[30] = "D:"; char n[15];
            NumeroATexto(dist, n); strcat(t, n); strcat(t, "cm\n");
            MandarString(t); cont_telem = 0;
        }


        if (velocidad_global == 0 && ticks_giro == 0) {
            // STOP
            GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
            GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 0); 
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 0);
        }
        else if (ticks_giro > 0) {
            uint32_t ancho_g = (periodo * 40) / 100;
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho_g);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, ancho_g);
            if(modo_giro == 1) {
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04);
            } else {
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
            }
        } 
        else {
            uint32_t ancho = (periodo * velocidad_global) / 100;
            
            if (ancho >= periodo) ancho = periodo - 1;
            if (velocidad_global <= 0) ancho = 0;

            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ancho);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, ancho);

            if (dist <= 10) {
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                SysCtlDelay(reloj / 5); 
                modo_giro = 2; ticks_giro = 100; 
            } else {
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04);
            }
        }

        if(UARTCharsAvail(UART3_BASE)) {
            char c = UARTCharGetNonBlocking(UART3_BASE);
            if(c == '\n' || c == '\r') {
                buffer[indice] = '\0'; indice = 0;
                if(buffer[0] == 'V') {
                    velocidad_global = ExtraerNumero(buffer);
                    if(velocidad_global > 100) velocidad_global = 100;
                } 
                else if(buffer[0] == 'L' || buffer[0] == 'R') {
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                    SysCtlDelay(reloj / 10); 
                    modo_giro = (buffer[0] == 'L') ? 1 : 2;
                    ticks_giro = (uint32_t)(ExtraerNumero(buffer) * 1.5);
                }
                else if(strcmp(buffer, "buzzer") == 0) {
                    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, 0x01);
                    SysCtlDelay(reloj / 20);
                    GPIOPinWrite(GPIO_PORTQ_BASE, 0x01, 0x00);
                }
            } else if(indice < 19) buffer[indice++] = c;
        }
    }
}