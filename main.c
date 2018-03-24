#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "Hardware/CS_Driver.h"
#include "MSPIO.h"

/* Timer_A Continuous Mode Configuration Parameter */
const Timer_A_UpModeConfig upModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,            // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,       // ACLK/1 = 32Khz
        200,
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0
        TIMER_A_DO_CLEAR                     // Clear Counter
};

/* Timer_A Compare Configuration Parameter */
const Timer_A_CompareModeConfig compareConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        200                                          // 266 Period
};

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

    /* Initializing ADC (MCLK/1/4) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    /* Configuring GPIOs (5.5 A0) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);

    /* Configuring Timer_A in continuous mode and sourced from ACLK */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfig);

    /* Configuring Timer_A0 in CCR1 to trigger at 260 (0.0083s AKA 120Hz) */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig);

    /* Configuring the sample trigger to be sourced from Timer_A0  and setting it
     * to automatic iteration after it is triggered*/
    ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);

    /* Enabling the interrupt when a conversion on channel 1 is complete and
     * enabling conversions */
    ADC14_enableInterrupt(ADC_INT0);
    ADC14_enableConversion();

    /* Enabling Interrupts */
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    /* Starting the Timer */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

    uint32_t ADCValue;

    while(1)
    {
        if(print)
        {
            ADCValue = ADCBuffer[index] & 0x0000FFFF;
            MSPrintf(EUSCI_A0_BASE, "%i\n\r", ADCValue);
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
