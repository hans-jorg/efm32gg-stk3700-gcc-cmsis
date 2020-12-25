/**
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.1
 *
 * @note    Just blinks the LEDs of the STK3700
 *
 * @note    LEDs are on pins 2 and 3 of GPIO Port E
 *
 * @note    It uses a delay based on SysTick.
 *
 * @note    It uses a LED HAL.
 *
 * @author  Hans
 * @date    01/09/2018
 */

#include <stdint.h>
#include "em_device.h"
#include "led.h"

/**
 * @brief  SysTick is called every 1 ms
 */
#define DIVIDER 1000

/**
 * @brief standard delay is 1 sec
 */
#define DELAYVAL 1000

/**
 * @brief Tick counter
 * @note  Incremented every 1 ms
 */
uint32_t volatile TickCounter = 0;



/**
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

void SysTick_Handler(void) {
    TickCounter++;
}

/**
 * @brief  Better delay function
 *
 * @param  delay is in ms
 */

void Delay(uint32_t delay) {
volatile uint32_t initialvalue = TickCounter;

    while( (TickCounter-initialvalue) < delay ) {}
}

/**
 * @brief  Better delay function
 *
 * @param  delay is in clock pulses
 */

void DelayPulses(uint32_t delay) {

    SysTick->LOAD = delay;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

    while( (SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk) == 0 ) {}
    SysTick->CTRL = 0;
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

   /* Configure SysTick */
    SysTick_Config(SystemCoreClock/DIVIDER);

    /* Enable interrupts */
    __enable_irq();

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
