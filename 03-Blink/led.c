
/**
 * @file    gpio.c
 * @brief   GPIO HAL for EFM32GG
 * @version 1.0
 */
#include <stdint.h>
#include "gpio.h"
#include "led.h"

/**
 * @brief   Initializes system to enable LED control
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_Init(uint32_t leds) {

    // Configure Pins in GPIOE
    GPIO_Init(GPIOE,0,leds);

    // Turn Off LEDs
    GPIO_WritePins(GPIOE,leds,0);

}

/**
 * @brief   Turns on LEDs specified by leds
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_On(uint32_t leds) {

    GPIO_WritePins(GPIOE,0,leds);
}

/**
 * @brief   Turns off LEDs specified by leds
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_Off(uint32_t leds) {

    GPIO_WritePins(GPIOE,leds,0);
}

/**
 * @brief   Toggles the LEDs specified by leds
 *
 * @note    LEDs are defined to easy access the corresponding pins in GPIO port
 */
void LED_Toggle(uint32_t leds) {

    GPIO_TogglePins(GPIOE,leds);
}

void LED_Write(uint32_t off, uint32_t on) {

    GPIO_WritePins(GPIOE,off,on);
}
