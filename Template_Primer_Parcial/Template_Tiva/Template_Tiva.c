//*****************************************************************************
//
// Template_Tiva.c
// 

// INCLUDES
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>

// Librerías de bajo nivel
#include "inc/hw_memmap.h"


// GPIO --------------------------- 
// Librerías driverlib - GPIO
#include "driverlib/sysctl.h"   // Control del sistema (clock, periféricos)
#include "driverlib/gpio.h"     // Manejo de GPIO
#include "driverlib/debug.h"    // Para debug (opcional)

// TIMMERS E INTERRUPCIONES -------------------------------

// //Para timmers
// #include "driverlib/timer.h"
// //Para interrupciones
// #include "driverlib/interrupt.h"

// ADC ------------------------
// #include "driverlib/adc.h"
// #include "driverlib/pin_map.h"

// UART ------------------------
// #include "driverlib/uart.h"
// #include "driverlib/pin_map.h"
// #include "utils/uartstdio.c"

// PWM ---------------------------
// #include "driverlib/pwm.h"
// #include "driverlib/pin_map.h"

//*****************************************************************************
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

//*****************************************************************************
//
//

//Variables globales

// PARA TIMMER ------------------------
//volatile uint32_t FS = 120000000 * 1;

//Funciones: TIMMER
// Recuerda agregarla a startup_gcc como: extern void interrupcion1(void); abajo de extern int main(void); y en el combre de la interrupcion
/* void interrupcion1(void){
    
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Código de interrupción en timmer: 
} */ 



// Variables Para UART:

// #define BUFFER_SIZE 50
// volatile char buffer[BUFFER_SIZE];
// volatile uint8_t index = 0;
// volatile uint8_t mensajeCompleto = 0;

//Función UART
/*void UART3IntHandler(void) {
    uint32_t status;
    status = UARTIntStatus(UART3_BASE, true);
    UARTIntClear(UART3_BASE, status);

    // Mientras haya datos disponibles
    while(UARTCharsAvail(UART3_BASE))
    {
        char c = UARTCharGet(UART3_BASE);
        if(c == '\n' || c == '\r')
        {
            buffer[index] = '\0';
            index = 0;
            mensajeCompleto = 1;
        }
        else
        {
            if(index < BUFFER_SIZE - 1)
            {
                buffer[index++] = c;
            }
        }
    }
}*/

//
//*****************************************************************************


// Función Delay:
void Delay(float time){
    SysCtlDelay((time * 120000000 / 3));
}

int main(void) {


    // TIMMERS E INTERRUPCIONES (Ejemplo timmer 0A)
    //Clock Conifguration
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    // // Enable Timmer Peripheral
    // SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // // check the pheripheral
    // while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) { }
    // // Set Timmer
    // TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // El timer se reinicia automáticamente después de llegar a 0, para una one shot usar: TIMER_CFG_ONE_SHOT
    // // Set the count time for timer
    // TimerLoadSet(TIMER0_BASE, TIMER_A, FS);
    // // Enable processor interrupts
    // IntMasterEnable();
    // // Enable Interrupt
    // IntEnable(INT_TIMER0A);
    // // Enable timer A interrupt
    // TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // // Enable the timer
    // TimerEnable(TIMER0_BASE, TIMER_A);

    
    // UART (PA4 y PA5, UART3)
    // SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    // SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    // SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // GPIOPinConfigure(GPIO_PA4_U3RX);
    // GPIOPinConfigure(GPIO_PA5_U3TX);
    // GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    // // Configurar UART3 manualmente (9600 8N1)
    // UARTConfigSetExpClk(UART3_BASE, 120000000, 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    // // - Base: UART0
    // // - Clock: 120 MHz
    // // - Baud rate: 9600
    // // - Configuración: 8 bits, sin paridad, 1 stop bit
    // Para UART0 Puedes usar UARTStudioConfig(0, 9600, 120000000)
    // Para interrupción:
    // UARTIntEnable(UART3_BASE, UART_INT_RX);
    // IntEnable(INT_UART3);
    // IntMasterEnable();
    

    // ADC
    // // Activar el periferico ADC
    // SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); 
    // SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Activa el puerto GPIOE porque el pin analógico que usamos (PE3) pertenece a ese puerto
    // // Espera a que el ADC esté listo antes de configurarlo
    // while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)){}
    // // Enables pin - Cambia el pin de modo digital → modo analógico para usar ADC
    // GPIOPinTypeADC(GPIO_PORTE_BASE, 0x08); // Configura el pin PE3 como entrada analógica (0x08 = PIN 3)
    // // Configura el secuenciador 3 del ADC:
    // ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    // // - Secuenciador 3: el más simple (solo 1 muestra)  - ADC_TRIGGER_PROCESSOR = la conversión se inicia por software (no automático)
    // ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
    // // Configura el paso 0 del secuenciador:
    // - ADC_CTL_CH0 → usa el canal analógico 0 (AIN0 = PE3)
    // - ADC_CTL_IE → habilita interrupción al terminar (opcional pero común)
    // - ADC_CTL_END → indica que es el último paso de la secuencia
    // ADCSequenceEnable(ADC0_BASE, 3); // Activa el secuenciador para que pueda empezar a convertir
    // ADCIntClear(ADC0_BASE, 3);   // Limpia la bandera de interrupción del ADC (evita lecturas falsas iniciales)

    // PWM
    // // Activar módulo PWM0  (M0PWM1)
    // SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    // Activar GPIO
    // SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // // Configurar pin PF1 como PWM (M0PWM1)
    // GPIOPinConfigure(GPIO_PF1_M0PWM1);
    // GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1); // Pdroa usar tmb GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
    // // Esperar a que el PWM esté listo
    // while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0)) { }
    // // Configurar clock del PWM (divide el del sistema)
    // SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
    // // Configurar generador PWM (modo down count)
    // PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    // // Definir periodo (frecuencia)
    // PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 1000);
    // // Duty inicial
    // width = 300; // 30%
    // PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, width);
    // // Habilitar generador
    // PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    // // Activar salida
    // PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);


    // GPIO 
    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    // Para outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    // // Para inputs
    // GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0); // BOTON J0 interno de la Tiva
    // GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); // BOTON J0 interno de la Tiva



    //
    // Loop forever.
    //
    while(1)
    {
        // Turn on the pin 0N, con GPIO
        // GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);


        // Lectura ADC
        // ADCProcessorTrigger(ADC0_BASE, 3); // Inicia manualmente la conversión ADC (porque usamos trigger por software)
        // while(!ADCIntStatus(ADC0_BASE, 3, false)){} // Espera hasta que la conversión haya terminado
        // ADCIntClear(ADC0_BASE, 3); // Limpia la bandera de interrupción para la siguiente lectura
        // ADCSequenceDataGet(ADC0_BASE, 3, &adcValue); // Guarda el valor convertido (0–4095) en la variable adcValue


        // UART envio de datos
        // char *msg = "MENSAJE\r\n";
        // for(int i = 0; msg[i] != '\0'; i++){
        //     UARTCharPut(UART3_BASE, msg[i]);
        // }
        // Uso variable recibida:
        /*if(mensajeCompleto){
            mensajeCompleto = 0;
            // lo que quiera hacer con el mensaje
        }*/

        // Para Bouncing efect
        // Detecta cuando el botón se PRESIONA
        /* if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0){
            Delay(0.02);
            // Verifica que sigue presionado después del delay
            if(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0)
            {
                // Espera a que el botón se SUELTE (evita múltiples activaciones)
                while(GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0) == 0) { }
            }
        }*/
        
    }
}
