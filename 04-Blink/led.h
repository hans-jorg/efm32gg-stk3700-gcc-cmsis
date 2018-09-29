#ifndef LED_H
#define LED_H
/**
 * @file    led.h
 * @brief   LED HAL for EFM32GG STK3200
 * @version 1.0
 */
#include <stdint.h>

/**
 * @brief   create a bit mask with bit N set
 */
#ifndef BIT
#define BIT(N) (1U<<(N))
#endif

/**
 * @brief   Symbols for LEDs
 * @note    Values choosen to simplify access to pin in GPIO
 */
///@{
// LEDs are on PE
#define LED1 BIT(2)
#define LED2 BIT(3)
///@}

/**
 *  @brief  LED interface
 */
///@{
void LED_Init(uint32_t leds);
void LED_On(uint32_t leds);
void LED_Off(uint32_t leds);
void LED_Toggle(uint32_t leds);
void LED_Write(uint32_t off, uint32_t on);
///@}
#endif // LED_H
