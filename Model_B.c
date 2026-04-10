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

// Enviar texto a la Rasp
void MandarStringRasp(const char *texto) {
    while(*texto != '\0') {
        UARTCharPut(UART3_BASE, *texto);
        texto++;
    }
}

// Convertir número a texto
void NumeroATexto(uint32_t num, char* buffer) {
    int i = 0, j = 0;
    char temp[15];
    if (num == 0) { buffer[0] = '0'; buffer[1] = '\0'; return; }
    while (num > 0) { temp[i++] = (num % 10) + '0'; num /= 10; }
    while (i > 0) { buffer[j++] = temp[--i]; }
    buffer[j] = '\0';
}

// Asignar PWM (Frecuencia 2 kHz -> 60000 Ticks)
void SetMotores(int velA, int velB) {
    uint32_t anchoA = (60000 * velA) / 100;
    uint32_t anchoB = (60000 * velB) / 100;
    if(anchoA >= 60000) anchoA = 59999;
    if(anchoB >= 60000) anchoB = 59999;
    if(anchoA == 0) anchoA = 1;
    if(anchoB == 0) anchoB = 1;
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, anchoA); // ENA (PF2)
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, anchoB); // ENB (PF1)
}

// Lectura de Sensor Ultrasónico
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

    uint32_t ciclos = 0xFFFFFFFF - TimerValueGet(TIMER0_BASE, TIMER_A);
    return (ciclos / 6996);
}

int main(void) {
    // 1. Reloj a 120 MHz
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    // 2. Habilitar Periféricos
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) {}

    // 3. UART3 (PA4 RX, PA5 TX)
    GPIOPinConfigure(GPIO_PA4_U3RX);
    GPIOPinConfigure(GPIO_PA5_U3TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    UARTConfigSetExpClk(UART3_BASE, 120000000, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // 4. Desbloqueo y Configuración de LEDs Internos
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0x00);

    // 5. Motor Direcciones (IN1=PH0, IN2=PH1, IN3=PM1, IN4=PM2)
    GPIOPinTypeGPIOOutput(GPIO_PORTH_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_1 | GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
    GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);

    // 6. Sensor Ultrasónico (PN5=Trig, PN4=Echo)
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_5); 
    GPIOPinTypeGPIOInput(GPIO_PORTN_BASE, GPIO_PIN_4);  
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_5, 0x00);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);

    // 7. PWM (PF1=ENB, PF2=ENA)
    GPIOPinConfigure(GPIO_PF1_M0PWM1);
    GPIOPinConfigure(GPIO_PF2_M0PWM2);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 60000);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 60000);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, 1);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 1);
    PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT | PWM_OUT_2_BIT, true);
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    // Variables de Estado
    bool nitro = false;
    int potencia = 70;
    bool yendo_adelante = false;
    uint32_t contador_telemetria = 0;

    while(1) {
        // --- LECTURA DE SENSOR ---
        uint32_t distancia = MedirDistancia();
        
        // Enviar telemetría a la Raspberry cada ~20 ciclos para no saturar la pantalla
        contador_telemetria++;
        if(contador_telemetria >= 20 && distancia != 999) {
            char txt_dist[40] = "Distancia: ";
            char num_str[15];
            NumeroATexto(distancia, num_str);
            strcat(txt_dist, num_str);
            strcat(txt_dist, " cm\n");
            MandarStringRasp(txt_dist);
            contador_telemetria = 0;
        }

        // --- SISTEMA DE FRENO AUTOMÁTICO ---
        if(distancia > 0 && distancia <= 5 && yendo_adelante) {
            // Apagar motores y LEDs
            GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
            GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
            SetMotores(0, 0);
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x00);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0x00);
            yendo_adelante = false;
            
            // Mandar alerta
            MandarStringRasp("[!] ALERTA: Obstaculo a 5cm. FRENO ACTIVADO.\n");
        }

        // --- LECTURA DE COMANDOS (UART) ---
        if(UARTCharsAvail(UART3_BASE)) {
            char c = UARTCharGetNonBlocking(UART3_BASE);

            if(c == 'n') {
                nitro = !nitro;
                potencia = nitro ? 100 : 70;
                MandarStringRasp(nitro ? "--> NITRO ON (100%)\n" : "--> NITRO OFF (70%)\n");
                
                // Si el auto ya se está moviendo hacia adelante, actualizamos su velocidad
                if(yendo_adelante) SetMotores(potencia, potencia);
            }
            else if(c == 's') { // ADELANTE
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x01); // IN1=1, IN2=0
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x02); // IN3=1, IN4=0
                SetMotores(potencia, potencia);
                
                yendo_adelante = false;
                // Encender LEDs
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_0 | GPIO_PIN_1);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_PIN_0 | GPIO_PIN_4);
                
            }
            else if(c == 'w') { // ATRAS
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02); // IN1=0, IN2=1
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04); // IN3=0, IN4=1
                SetMotores(potencia, potencia);
                
                // Apagar LEDs (Solo prenden hacia adelante)
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0x00);
                
                yendo_adelante = true;
            }
            else if(c == 'a') { // IZQUIERDA
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x02); 
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x02); 
                SetMotores(potencia, potencia);
                yendo_adelante = false;
            }
            else if(c == 'd') { // DERECHA
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x01); 
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x04); 
                SetMotores(potencia, potencia);
                yendo_adelante = false;
            }
            else if(c == ' ') { // DETENER
                GPIOPinWrite(GPIO_PORTH_BASE, 0x03, 0x00);
                GPIOPinWrite(GPIO_PORTM_BASE, 0x06, 0x00);
                SetMotores(0, 0);
                
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0x00);
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, 0x00);
                yendo_adelante = false;
            }
        }
        
        // Pequeño retardo estabilizador para el sensor y el ciclo
        SysCtlDelay(40000); 
    }
}
