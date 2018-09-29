

/**
 * @file    gpio.c
 * @brief   GPIO HAL for EFM32GG
 * @version 1.0
 */
#include <stdint.h>
#include "em_device.h"
#include "led.h"

/**
 * @brief   Use of Toggle, Set and Clear operation of the GPIO
 *
 * If not set, all operations are done using DOUT in a read-modify-write cyclus
 */
//#define PIO_ALTERNATE_OPERATIONS


/**
 * @brief   Pointer to GPIO registers
 */
static GPIO_P_TypeDef * const GPIOE = &(GPIO->P[4]);  // GPIOE

/**
 * @brief   Initializes system to enable LED control
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_Init(uint32_t leds) {

    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    if( leds&LED1 ) {
        GPIOE->MODEL &= ~_GPIO_P_MODEL_MODE2_MASK;       // Clear bits
        GPIOE->MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;  // Set bits
    }

    if( leds&LED2 ) {
        GPIOE->MODEL &= ~_GPIO_P_MODEL_MODE3_MASK;       // Clear bits
        GPIOE->MODEL |= GPIO_P_MODEL_MODE3_PUSHPULL;  // Set bits
    }


    // Turn Off LEDs
    LED_Off(leds);

}

/**
 * @brief   Turns on LEDs specified by leds
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_On(uint32_t leds) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTSET = leds;
#else
    GPIOE->DOUT |= leds;
#endif
}

/**
 * @brief   Turns off LEDs specified by leds
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_Off(uint32_t leds) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTCLR = leds;
#else
    GPIOE->DOUT &= ~leds;
#endif
}

/**
 * @brief   Toggles the LEDs specified by leds
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_Toggle(uint32_t leds) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTTGL = leds;
#else
    GPIOE->DOUT ^= leds;
#endif
}

/**
 * @brief   Controls status of LEDs.
 *
 * Turns off the LEDs specified by off, and then, turns on, the LEDs specified by on
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_Write(uint32_t off, uint32_t on) {

#ifdef GPIO_ALTERNATE_OPERATIONS
    GPIOE->DOUTCLR = off;
    GPIOE->DOUTSET = on;
#else
    GPIOE->DOUT = (GPIOE->DOUT&~off)|on;
#endif
}
