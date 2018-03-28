#include "ADC_Driver.h"

void ADC_Init(void)
{
    /* Initializing ADC (MCLK/1/4) */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    /* Configuring GPIOs (5.5 A0) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configuring ADC Memory */
    ADC14_configureSingleSampleMode(ADC_MEM0, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);

    /* Configuring the sample trigger to be sourced from Timer_A0  and setting it
    * to automatic iteration after it is triggered*/
    ADC14_setSampleHoldTrigger(ADC_TRIGGER_SOURCE1, false);

    /* Enabling the interrupt when a conversion on channel 1 is complete and
    * enabling conversions */
    ADC14_enableInterrupt(ADC_INT0);
    ADC14_enableConversion();

     /* Enabling Interrupts */
     Interrupt_enableInterrupt(INT_ADC14);
}

