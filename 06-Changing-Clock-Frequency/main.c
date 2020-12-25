/**
 * @note    Just blinks the LEDs of the STK3700
 *
 * @note    LEDs are on pins 2 and 3 of GPIO Port E
 *
 * @note    It uses a primitive delay mechanism. Do not use it in production code.
 *
 * @note    It uses a LED HAL.
 *
 * @note    It uses code in system-efm32gg-ext to change clock frequency.
 *
 * @note    With this frequency change, the delays play havoc.
 *
 * @author  Hans
 * @date    01/09/2018
 */


#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "clock_efm32gg.h"
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

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);
    
    /* Configure Pins in GPIOE */
    LED_Init(LED1|LED2);

    /* Blink loop */
    while (1) {

        Delay(DELAYVAL);
        LED_Toggle(LED1);                                // Toggle LED1

        Delay(DELAYVAL);
        LED_Toggle(LED2);                                // Toggle LED2

        Delay(DELAYVAL);
        LED_Write(0,LED1|LED2);                          // Turn On All LEDs

    }
}
