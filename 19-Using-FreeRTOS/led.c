/** **************************************************************************
 * @file    led.c
 * @brief   LED HAL for EFM32GG STK
 * @version 1.0
 *****************************************************************************/

#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"

#include "led.h"


static GPIO_P_TypeDef * const GPIOE = &(GPIO->P[4]);    // GPIOE

void LED_Init(uint32_t leds) {

    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    if( leds&LED1 ) {
        GPIOE->MODEL &= ~_GPIO_P_MODEL_MODE2_MASK;      // Clear bits
        GPIOE->MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;    // Set bits
    }

    if( leds&LED2 ) {
        GPIOE->MODEL &= ~_GPIO_P_MODEL_MODE3_MASK;      // Clear bits
        GPIOE->MODEL |= GPIO_P_MODEL_MODE3_PUSHPULL;    // Set bits
    }

    // Turn Off LEDs
    LED_Off(LED1|LED2);

}

void LED_On(uint32_t leds) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTSET = leds;
#else
    GPIOE->DOUT |= leds;
#endif
}

void LED_Off(uint32_t leds) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTCLR = leds;
#else
    GPIOE->DOUT &= ~leds;
#endif
}

void LED_Toggle(uint32_t leds) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTTGL = leds;
#else
    GPIOE->DOUT ^= leds;
#endif
}

void LED_Write(uint32_t off, uint32_t on) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTCLR = off;
    GPIOE->DOUTSET = on;
#else
    GPIOE->DOUT = (GPIOE->DOUT&~off)|on;
#endif
}
