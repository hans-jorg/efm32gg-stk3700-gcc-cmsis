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
#define LED0 BIT(2)
#define LED1 BIT(3)

#define DELAYVAL 1



/**
 * @brief  Quick and dirty delay function
 * @param  delay: specified delay (not calibrated)
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
    GPIO_Init(GPIOE,0,LED0|LED1);       // LED pins configured as output

    GPIO_WritePins(GPIOE,0,LED0|LED1);  // Turn Off LEDs

    /* Blink loop */
    while (1) {

        Delay(DELAYVAL);
        GPIO_TogglePins(GPIOE,LED0);            // Toggle LED0

        Delay(DELAYVAL);
        GPIO_TogglePins(GPIOE,LED1);            // Toggle LED1

        Delay(DELAYVAL);
        GPIO_WritePins(GPIOE,0,LED0|LED1);      // Turn On All LEDs

    }
}
