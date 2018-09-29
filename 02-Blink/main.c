/**
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.0
 */

#include <stdint.h>
#include "gpio.h"
#include "em_device.h"


#define BIT(N) (1U<<(N))

// LEDs are on PE
#define LED1 BIT(2)
#define LED2 BIT(3)

#define DELAYVAL 1



/**
 * @brief  Quick and dirty delay function
 * @param  delay: specified delay (not calibrated)
 * @note   Do not use this in a production code
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
 * @note   HFCLK = HFRCO
 * @note   HFCORECLK = HFCLK
 * @note   HFPERCLK  = HFCLK
 */

int main(void) {

    /* Configure Pins in GPIOE */
    GPIO_Init(GPIOE,0,LED1|LED2);       // LED pins configured as output

    GPIO_WritePins(GPIOE,0,LED1|LED2);  // Turn Off LEDs

    /* Blink loop */
    while (1) {

        Delay(DELAYVAL);
        GPIO_TogglePins(GPIOE,LED1);            // Toggle LED2

        Delay(DELAYVAL);
        GPIO_TogglePins(GPIOE,LED2);            // Toggle LED2

        Delay(DELAYVAL);
        GPIO_WritePins(GPIOE,0,LED1|LED2);      // Turn On All LEDs

    }
}
