
/**
 * @file    gpio.c
 * @brief   GPIO HAL for EFM32GG
 * @version 1.0
 */

#include <stdint.h>
#include "gpio.h"

/**
 * @brief   Configure pins
 * @param   gpio: pointer to gpio registers
 * @param   pins: bit mask containing 1 in the position to be configured
 * @param   mode: GPIO_MODE_INPUT, GPIO_MODE_PUSHPULL, etc.
 */

void
GPIO_ConfigPins(GPIO_t gpio, uint32_t pins, uint32_t mode) {
const uint32_t mask = 0xF;  // Fields are 4 bits wide
int i;
uint32_t mbit;


    mbit = 0x0001; // Bit 0 set
    /* Configure low order bits/pins  0-7 */
    for(i=0;i<32;i+=4) {
        if( (pins&mbit) != 0 ) {
            gpio->MODEL &= ~(mask<<i);
            gpio->MODEL |= mode<<i;
        }
        mbit <<= 1;
    }
    /* Configure High order bits/pins 8-15 */
    for(i=0;i<32;i+=4) {
        if( (pins&mbit) != 0 ) {
            gpio->MODEH &= ~(mask<<i);
            gpio->MODEH |= mode<<i;
        }
        mbit <<= 1;
    }

}

/**
 * @brief   Configure pins for input or output
 * @param   gpio:    pointer to gpio registers
 * @param   inputs:  bit mask containing 1 in the position to be configured as input
 * @param   outputs: bit mask containing 1 in the position to be configured as output
 */

void
GPIO_Init(GPIO_t gpio, uint32_t inputs, uint32_t outputs) {


    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    GPIO_ConfigPins(gpio,outputs,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(gpio,inputs,GPIO_MODE_INPUT);       // Safer when both specified

}
