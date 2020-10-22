/** ***************************************************************************
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.0
******************************************************************************/

#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "clock_efm32gg.h"
#include "timers.h"
#include "led.h"

#define SYSTICKDIVIDER 1000
#define SOFTDIVIDER    1000

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

void SysTick_Handler(void) {
static int counter = 0;             // must be static

    if( counter != 0 ) {
        counter--;
    } else {
        Timers_dispatch();          // Every 1 second
        counter = SOFTDIVIDER-1;
    }
}


void BlinkLED1(void) {
    LED_Toggle(LED1);
}

void BlinkLED2(void) {
    LED_Toggle(LED2);
}



/*****************************************************************************
 * @brief  Main function
 *
 * @note   Using default clock configuration
 *         HFCLK = HFRCO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {


    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    
    /* Configure Pins in GPIOE */
    LED_Init(LED1|LED2);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/SYSTICKDIVIDER);    // Every 1 ms

    Timers_add(2,BlinkLED1);
    Timers_add(3,BlinkLED2);
    
    /* Blink loop */
    while (1) {}

}
