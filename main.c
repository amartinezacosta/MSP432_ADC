#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "MSPIO.h"
#include "Hardware/TimerA_Driver.h"
#include "Hardware/ADC_Driver.h"

eUSCI_UART_Config UART0Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

volatile uint_fast16_t ADCBuffer[256];
volatile uint8_t index = 0;
volatile uint8_t print = 0;

void main(void)
{
    WDT_A_holdTimer();
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    UART_Init(EUSCI_A0_BASE, UART0Config);
    ADC_Init();
    TimerA_Init();

    TimerA_StartTimer();

    Interrupt_enableMaster();

    uint16_t ADCValue;

    while(1)
    {
        if(print)
        {
            ADCValue = ADCBuffer[index];
            UART_Write(EUSCI_A0_BASE, (uint8_t*)&ADCValue, sizeof(uint16_t));
            //MSPrintf(EUSCI_A0_BASE, "%i", ADCValue);
            print = 0;
        }
    }
}

/* This interrupt is fired whenever a conversion is completed and placed in
 * ADC_MEM0 */
void ADC14_IRQHandler(void)
{
    uint64_t status;

    status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);


    if (status & ADC_INT0)
    {
        if(index == UINT8_MAX)
        {
           index = 0;
        }

        ADCBuffer[index++] = ADC14_getResult(ADC_MEM0);
        print = 1;
    }

}
