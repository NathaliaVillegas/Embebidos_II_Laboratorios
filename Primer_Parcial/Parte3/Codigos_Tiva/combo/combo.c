#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "driverlib/interrupt.h"

volatile uint32_t ticks_giro = 0;
volatile int modo_giro = 0; 
uint32_t periodo = 60000;

int modo_tiva = 0;
int velocidad_global = 70;
bool nitro = false;
int potencia_manual = 70;
bool yendo_adelante = false;

char buffer_uart[20];
int indice_uart = 0;

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

void SetMotores(int velA, int velB) {
    uint32_t anchoA = (periodo * velA) / 100;
    uint32_t anchoB = (periodo * velB) / 100;
    if(anchoA >= periodo) anchoA = periodo - 1;
    if(anchoB >= periodo) anchoB = periodo - 1;
    if(anchoA == 0) anchoA = 1;
    if(anchoB == 0) anchoB = 1;
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, anchoA); 
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, anchoB); 
}

uint32_t MedirDistancia(void) {
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, GPIO_PIN_5);
    SysCtlDelay(400); 
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0);

    uint32_t limite = 100000;
    while(GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_4) == 0 && --limite);
    if(limite == 0) return 999;

    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);
    TimerEnable(TIMER0_BASE, TIMER_A);

    limite = 400000;
    while(GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_4) != 0 && --limite);
    TimerDisable(TIMER0_BASE, TIMER_A);
    return (0xFFFFFFFF - TimerValueGet(TIMER0_BASE, TIMER_A)) / 6996;
}

int main(void) {
    uint32_t reloj = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ); 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) {}

    // UART
    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x30);
    UARTConfigSetExpClk(UART3_BASE, reloj, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Botón Físico SW1 (PJ0)
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Direcciones y Sensor
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, 0x03);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, 0x06);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_5); 
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_4);  
    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

    // PWM
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, 0x06);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, periodo);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, periodo);
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT | PWM_OUT_2_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);


    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, reloj / 100); 
    TimerIntRegister(TIMER1_BASE, TIMER_A, Timer1AIntHandler);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER1_BASE, TIMER_A);

    uint32_t cont_telem = 0;
    bool btn_anterior = true; 

    MandarString("\r\n[TIVA] SISTEMA INICIADO\r\n");

    while(1) {
        

        bool btn_actual = (GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0); 
        if (btn_actual && !btn_anterior) {
            MandarString("CMD:CAMBIO\n"); // Envia la señal al Python
            SysCtlDelay(reloj / 10); 
        }
        btn_anterior = btn_actual;


        uint32_t dist = MedirDistancia();
        
        if(++cont_telem >= 20 && dist != 999) {
            char t[30] = "D:"; char n[15];
            NumeroATexto(dist, n); strcat(t, n); strcat(t, "cm\r\n");
            MandarString(t); cont_telem = 0;
        }

        
        if(modo_tiva == 0) {
            if (velocidad_global == 0 && ticks_giro == 0) {
                SetMotores(0,0);
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
            }
            else if (ticks_giro > 0) {
                SetMotores(40, 40); 
                if(modo_giro == 1) { 
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02);
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x02);
                } else { 
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x01);
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04);
                }
            } 
            else {
                SetMotores(velocidad_global, velocidad_global);
                if (dist > 0 && dist <= 10) {
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                    SysCtlDelay(reloj / 5); 
                    modo_giro = 2; 
                    ticks_giro = 100; 
                } else {
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x01);
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x02);
                }
            }
        } 
        else {
            
            if(dist > 0 && dist <= 5 && yendo_adelante) {
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                SetMotores(0, 0);
                yendo_adelante = false;
                MandarString("[!] Obstaculo a 5cm. FRENO.\r\n");
            }
        }

        while(UARTCharsAvail(UART3_BASE)) {
            char c = UARTCharGetNonBlocking(UART3_BASE);
            if(c == '1') {
                modo_tiva = 0; velocidad_global = 70; ticks_giro = 0; indice_uart = 0;
            } 
            else if(c == '2') {
                modo_tiva = 1; SetMotores(0,0); yendo_adelante = false; indice_uart = 0;
            } 
            
            
            else if (modo_tiva == 0) {
                if(c == '\n' || c == '\r') {
                    buffer_uart[indice_uart] = '\0';
                    
                    if(indice_uart > 0) {
                        if(buffer_uart[0] == 'V') {
                            velocidad_global = ExtraerNumero(buffer_uart);
                            if(velocidad_global > 100) velocidad_global = 100;
                            MandarString("[TIVA] Velocidad Aplicada!\r\n");
                        } 
                        else if(buffer_uart[0] == 'L' || buffer_uart[0] == 'R') {
                            GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                            GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                            SetMotores(0,0);
                            modo_giro = (buffer_uart[0] == 'L') ? 1 : 2;
                            ticks_giro = (uint32_t)(ExtraerNumero(buffer_uart) * 2); 
                            MandarString("[TIVA] Realizando Giro...\r\n");
                        }
                    }
                    indice_uart = 0;
                } else if(indice_uart < 19 && c != ' ') {
                    
                    if (c >= 'a' && c <= 'z') c = c - 32;
                    buffer_uart[indice_uart++] = c;
                }
            } 
            
            else if (modo_tiva == 1) {
                if(c == 'n') {
                    nitro = !nitro;
                    potencia_manual = nitro ? 100 : 70;
                    if(yendo_adelante) SetMotores(potencia_manual, potencia_manual);
                }
                else if(c == 's') { 
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x01); 
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x02); 
                    SetMotores(potencia_manual, potencia_manual);
                    yendo_adelante = false;
                }
                else if(c == 'w') { 
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02); 
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04); 
                    SetMotores(potencia_manual, potencia_manual);
                    yendo_adelante = true;
                }
                else if(c == 'd') { 
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02); 
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x02); 
                    SetMotores(potencia_manual, potencia_manual);
                    yendo_adelante = false;
                }
                else if(c == 'a') { 
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x01); 
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04); 
                    SetMotores(potencia_manual, potencia_manual);
                    yendo_adelante = false;
                }
                else if(c == ' ') { 
                    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                    SetMotores(0, 0);
                    yendo_adelante = false;
                }
            }
        }
        SysCtlDelay(40000); 
    }
}