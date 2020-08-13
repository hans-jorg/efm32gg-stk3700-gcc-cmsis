/**
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.1
 *
 * @note    Just blinks the LEDs of the STK3700
 *
 * @note    LEDs are on pins 2 and 3 of GPIO Port E
 *
 * @note    It uses a primitive delay mechanism. Do not use it.
 *
 * @note    It uses a LED HAL over a GPIO HAL.
 *
 * @author  Hans
 * @date    01/09/2018
 */

#include <stdint.h>
#include "em_device.h"

#include "gpio.h"
#include "led.h"

#define DELAYVAL 1

/**
 * @brief  Quick and dirty delay function
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
 * @note         HFCLK = HFRCO
 * @note         HFCORECLK = HFCLK
 * @note         HFPERCLK  = HFCLK
 */

int main(void) {

    /* Configure Pins in GPIOE */
    LED_Init(LED1|LED2);

    /* Blink loop */
    while (1) {

        Delay(DELAYVAL);
        LED_Toggle(LED1);                                // Toggle LED2

        Delay(DELAYVAL);
        LED_Toggle(LED2);                                // Toggle LED2

        Delay(DELAYVAL);
        LED_Write(0,LED1|LED2);                          // Turn On All LEDs

    }
}
