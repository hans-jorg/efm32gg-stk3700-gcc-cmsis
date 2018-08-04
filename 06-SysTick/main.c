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
#include "led.h"

#define DELAYVAL 1

/**
 * @brief  Quick and dirty delay function
 * @note   Do not use in production code
 */

void Delay(uint32_t delay) {
volatile uint32_t counter;
int i;

    for(i=0;i<delay;i++) {
        counter = 1000000;
        while( counter ) counter--;
    }
}

/**
 * @brief  Main function
 *
 * @note   Using default clock configuration
 *         HFCLK = HFRCO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {

    /* Configure Pins in GPIOE */
    LED_Init(LED0|LED1);

    /* Blink loop */
    while (1) {

        Delay(DELAYVAL);
        LED_Toggle(LED0);                                // Toggle LED0

        Delay(DELAYVAL);
        LED_Toggle(LED1);                                // Toggle LED1

        Delay(DELAYVAL);
        LED_Write(0,LED0|LED1);                          // Turn On All LEDs

    }
}
