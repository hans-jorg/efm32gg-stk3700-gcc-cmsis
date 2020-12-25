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
 * @author  Hans
 * @date    01/09/2018
 */

#include <stdint.h>
#include "em_device.h"

/**
 * @brief       BIT macro
 *              Defines a constant (hopefully) with a bit 1 in the N position
 * @param       N : Bit index with bit 0 the Least Significant Bit
 */
#define BIT(N) (1U<<(N))



///@{
/// LEDs are on Port E
#define LED1 BIT(2)
#define LED2 BIT(3)
///@}

/// Default delay value.
#define DELAYVAL 3
/**
 * @brief  Quick and dirty delay function
 * @note   Do not use it in production code
 */

void Delay(uint32_t delay) {
volatile uint32_t counter;
int i;

    for(i=0;i<delay;i++) {
        counter = 100000;
        while( counter ) counter--;
    }
}

/**
 * @brief  Main function
 *
 * @note   Using default clock configuration
 * @note   HFCLK     = HFRCO 14 MHz
 * @note   HFCORECLK = HFCLK
 * @note   HFPERCLK  = HFCLK

 */

int main(void) {
/// Pointer to GPIO Port E registers
GPIO_P_TypeDef * const GPIOE = &(GPIO->P[4]);           // GPIOE

    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    /* Configure Pins in GPIOE */
    GPIOE->MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK|_GPIO_P_MODEL_MODE3_MASK);       // Clear bits
    GPIOE->MODEL |= (GPIO_P_MODEL_MODE2_PUSHPULL|GPIO_P_MODEL_MODE3_PUSHPULL);  // Set bits

    /* Initial values for LEDs */
    GPIOE->DOUT &= ~(LED1|LED2);                                               // Turn Off LEDs

    /* Blink loop */
    while (1) {

        GPIOE->DOUT ^= LED1;
        Delay(DELAYVAL);

        GPIOE->DOUT ^= LED2;
        Delay(DELAYVAL);

        GPIOE->DOUT |= LED1|LED2;
        Delay(2*DELAYVAL);

        GPIOE->DOUT &= ~(LED1|LED2);
        Delay(DELAYVAL);

    }
}
