#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

uint32_t ui32SysClock;

void UARTSend(const char *str)
{   while(*str)
        UARTCharPut(UART0_BASE, *str++);}

void UARTSendNumber(uint32_t num)
{
    char buf[10];
    int i = 0;

    if(num == 0)
    {UARTCharPut(UART0_BASE, '0');
        return;}

    while(num > 0)
    {buf[i++] = (num % 10) + '0';
        num /= 10;}

    while(i--)
        UARTCharPut(UART0_BASE, buf[i]);
}

void Timer0A_Handler(void)
{
    uint32_t adcValue;

    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false));
    ADCSequenceDataGet(ADC0_BASE, 3, &adcValue);
    ADCIntClear(ADC0_BASE, 3);

    uint32_t load = (adcValue * ui32SysClock) / 4095;

    if(load < ui32SysClock / 20)
        load = ui32SysClock / 20;

    TimerLoadSet(TIMER0_BASE, TIMER_A, load);

    UARTSend("ADC: ");
    UARTSendNumber(adcValue);
    UARTSend("\r\n");
}

int main(void)
{
    ui32SysClock = SysCtlClockFreqSet(
        SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
        SYSCTL_USE_PLL | SYSCTL_CFG_VCO_240,
        120000000
    );

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0));

    GPIOPinTypeADC(GPIO_PORTE_BASE, 0x08);

    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(
        ADC0_BASE, 3, 0,
        ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
        
    ADCSequenceEnable(ADC0_BASE, 3);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, 0x03);

    UARTConfigSetExpClk(
        UART0_BASE,
        ui32SysClock,
        115200,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32SysClock);

    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    IntMasterEnable();

    TimerEnable(TIMER0_BASE, TIMER_A);

    while(1)
    {}
}
