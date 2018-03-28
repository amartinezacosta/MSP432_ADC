#include "TimerA_Driver.h"

/* Timer_A Continuous Mode Configuration Parameter */
const Timer_A_UpModeConfig upModeConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,               // ACLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // ACLK/1 = 32Khz
        200,
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer ISR
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE,    // Disable CCR0
        TIMER_A_DO_CLEAR                        // Clear Counter
};

/* Timer_A Compare Configuration Parameter */
const Timer_A_CompareModeConfig compareConfig =
{
        TIMER_A_CAPTURECOMPARE_REGISTER_1,          // Use CCR1
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,   // Disable CCR interrupt
        TIMER_A_OUTPUTMODE_SET_RESET,               // Toggle output but
        200                                         // 266 Period
};

void TimerA_Init(void)
{
    /* Configuring Timer_A in continuous mode and sourced from ACLK */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upModeConfig);

    /* Configuring Timer_A0 in CCR1 to trigger at 260 (0.0083s AKA 120Hz) */
    Timer_A_initCompare(TIMER_A0_BASE, &compareConfig);
}

void TimerA_StartTimer(void)
{
    /* Starting the Timer */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

