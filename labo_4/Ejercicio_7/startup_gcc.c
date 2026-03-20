//*****************************************************************************
//
// startup_gcc.c - Código de arranque para GNU modificado para Ejercicio 7.
//
//*****************************************************************************

#include <stdint.h>
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

//*****************************************************************************
// Declaración de los manejadores de fallas predeterminados.
//*****************************************************************************
void ResetISR(void);
static void NmiSR(void);
static void FaultISR(void);
static void IntDefaultHandler(void);

//*****************************************************************************
// Declaración externa del punto de entrada (main) y tu manejador de Timer.
//*****************************************************************************
extern int main(void);
extern void Timer0IntHandler(void); // <-- TU FUNCIÓN DECLARADA AQUÍ

//*****************************************************************************
// Espacio para la pila del sistema (Stack).
//*****************************************************************************
static uint32_t pui32Stack[128];

//*****************************************************************************
// La tabla de vectores.
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((uint32_t)pui32Stack + sizeof(pui32Stack)),
                                            // El puntero de pila inicial
    ResetISR,                               // El manejador de reinicio
    NmiSR,                                  // El manejador NMI
    FaultISR,                               // El manejador de falla dura
    IntDefaultHandler,                      // El manejador de falla MPU
    IntDefaultHandler,                      // El manejador de falla de bus
    IntDefaultHandler,                      // El manejador de falla de uso
    0,                                      // Reservado
    0,                                      // Reservado
    0,                                      // Reservado
    0,                                      // Reservado
    IntDefaultHandler,                      // SVCall
    IntDefaultHandler,                      // Monitor de depuración
    0,                                      // Reservado
    IntDefaultHandler,                      // PendSV
    IntDefaultHandler,                      // SysTick
    IntDefaultHandler,                      // GPIO Puerto A
    IntDefaultHandler,                      // GPIO Puerto B
    IntDefaultHandler,                      // GPIO Puerto C
    IntDefaultHandler,                      // GPIO Puerto D
    IntDefaultHandler,                      // GPIO Puerto E
    IntDefaultHandler,                      // UART0
    IntDefaultHandler,                      // UART1
    IntDefaultHandler,                      // SSI0
    IntDefaultHandler,                      // I2C0
    IntDefaultHandler,                      // PWM Fault
    IntDefaultHandler,                      // PWM Generator 0
    IntDefaultHandler,                      // PWM Generator 1
    IntDefaultHandler,                      // PWM Generator 2
    IntDefaultHandler,                      // Quadrature Encoder 0
    IntDefaultHandler,                      // ADC Sequence 0
    IntDefaultHandler,                      // ADC Sequence 1
    IntDefaultHandler,                      // ADC Sequence 2
    IntDefaultHandler,                      // ADC Sequence 3
    IntDefaultHandler,                      // Watchdog timer
    Timer0IntHandler,                       // <-- CAMBIADO: Timer 0 subtimer A
    IntDefaultHandler,                      // Timer 0 subtimer B
    IntDefaultHandler,                      // Timer 1 subtimer A
    IntDefaultHandler,                      // Timer 1 subtimer B
    IntDefaultHandler,                      // Timer 2 subtimer A
    IntDefaultHandler,                      // Timer 2 subtimer B
    IntDefaultHandler,                      // Analog Comparator 0
    IntDefaultHandler,                      // Analog Comparator 1
    IntDefaultHandler,                      // Analog Comparator 2
    IntDefaultHandler,                      // System Control
    IntDefaultHandler,                      // FLASH Control
    IntDefaultHandler,                      // GPIO Port F
    IntDefaultHandler,                      // GPIO Port G
    IntDefaultHandler,                      // GPIO Port H
    IntDefaultHandler,                      // UART2
    IntDefaultHandler,                      // SSI1
    IntDefaultHandler,                      // Timer 3 subtimer A
    IntDefaultHandler,                      // Timer 3 subtimer B
    IntDefaultHandler,                      // I2C1
    IntDefaultHandler,                      // CAN0
    IntDefaultHandler,                      // CAN1
    IntDefaultHandler,                      // Ethernet
    IntDefaultHandler,                      // Hibernate
    IntDefaultHandler,                      // USB0
    IntDefaultHandler,                      // PWM Generator 3
    IntDefaultHandler,                      // uDMA Software Transfer
    IntDefaultHandler,                      // uDMA Error
    IntDefaultHandler,                      // ADC1 Sequence 0
    IntDefaultHandler,                      // ADC1 Sequence 1
    IntDefaultHandler,                      // ADC1 Sequence 2
    IntDefaultHandler,                      // ADC1 Sequence 3
    IntDefaultHandler,                      // External Bus Interface 0
    IntDefaultHandler,                      // GPIO Port J
    IntDefaultHandler,                      // GPIO Port K
    IntDefaultHandler,                      // GPIO Port L
    IntDefaultHandler,                      // SSI2
    IntDefaultHandler,                      // SSI3
    IntDefaultHandler,                      // UART3
    IntDefaultHandler,                      // UART4
    IntDefaultHandler,                      // UART5
    IntDefaultHandler,                      // UART6
    IntDefaultHandler,                      // UART7
    IntDefaultHandler,                      // I2C2
    IntDefaultHandler,                      // I2C3
    IntDefaultHandler,                      // Timer 4 subtimer A
    IntDefaultHandler,                      // Timer 4 subtimer B
    IntDefaultHandler,                      // Timer 5 subtimer A
    IntDefaultHandler,                      // Timer 5 subtimer B
    IntDefaultHandler,                      // FPU
    0,                                      // Reservado
    0,                                      // Reservado
    IntDefaultHandler,                      // I2C4
    IntDefaultHandler,                      // I2C5
    IntDefaultHandler,                      // GPIO Port M
    IntDefaultHandler,                      // GPIO Port N
    0,                                      // Reservado
    IntDefaultHandler,                      // Tamper
    IntDefaultHandler,                      // GPIO Port P0
    IntDefaultHandler,                      // GPIO Port P1
    IntDefaultHandler,                      // GPIO Port P2
    IntDefaultHandler,                      // GPIO Port P3
    IntDefaultHandler,                      // GPIO Port P4
    IntDefaultHandler,                      // GPIO Port P5
    IntDefaultHandler,                      // GPIO Port P6
    IntDefaultHandler,                      // GPIO Port P7
    IntDefaultHandler,                      // GPIO Port Q0
    IntDefaultHandler,                      // GPIO Port Q1
    IntDefaultHandler,                      // GPIO Port Q2
    IntDefaultHandler,                      // GPIO Port Q3
    IntDefaultHandler,                      // GPIO Port Q4
    IntDefaultHandler,                      // GPIO Port Q5
    IntDefaultHandler,                      // GPIO Port Q6
    IntDefaultHandler,                      // GPIO Port Q7
    IntDefaultHandler,                      // GPIO Port R
    IntDefaultHandler,                      // GPIO Port S
    IntDefaultHandler,                      // SHA/MD5 0
    IntDefaultHandler,                      // AES 0
    IntDefaultHandler,                      // DES3DES 0
    IntDefaultHandler,                      // LCD Controller 0
    IntDefaultHandler,                      // Timer 6 subtimer A
    IntDefaultHandler,                      // Timer 6 subtimer B
    IntDefaultHandler,                      // Timer 7 subtimer A
    IntDefaultHandler,                      // Timer 7 subtimer B
    IntDefaultHandler,                      // I2C6
    IntDefaultHandler,                      // I2C7
    IntDefaultHandler,                      // HIM Scan Matrix Keyboard 0
    IntDefaultHandler,                      // One Wire 0
    IntDefaultHandler,                      // HIM PS/2 0
    IntDefaultHandler,                      // HIM LED Sequencer 0
    IntDefaultHandler,                      // HIM Consumer IR 0
    IntDefaultHandler,                      // I2C8
    IntDefaultHandler,                      // I2C9
    IntDefaultHandler                       // GPIO Port T
};

//*****************************************************************************
// Variables creadas por el enlazador (Linker)
//*****************************************************************************
extern uint32_t _ldata;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;

//*****************************************************************************
// Función ResetISR: Punto de entrada tras el reinicio físico.
//*****************************************************************************
void
ResetISR(void)
{
    uint32_t *pui32Src, *pui32Dest;

    // Copiar el segmento DATA de FLASH a SRAM
    pui32Src = &_ldata;
    for(pui32Dest = &_data; pui32Dest < &_edata; )
    {
        *pui32Dest++ = *pui32Src++;
    }

    // Llenar con ceros el segmento BSS
    __asm("    ldr     r0, =_bss\n"
          "    ldr     r1, =_ebss\n"
          "    mov     r2, #0\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    // Habilitar la FPU (Floating Point Unit)
    HWREG(NVIC_CPAC) = ((HWREG(NVIC_CPAC) &
                         ~(NVIC_CPAC_CP10_M | NVIC_CPAC_CP11_M)) |
                        NVIC_CPAC_CP10_FULL | NVIC_CPAC_CP11_FULL);

    // Llamar al main de la aplicación
    main();
}

static void NmiSR(void) { while(1) {} }
static void FaultISR(void) { while(1) {} }
static void IntDefaultHandler(void) { while(1) {} }
