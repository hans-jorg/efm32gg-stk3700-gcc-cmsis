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
#include <stdio.h>

#include "em_device.h"
#include "led.h"
#include "daconverter.h"

/**
 * @brief  SysTick is called every 1 ms
 */
#define DIVIDER 1000

/**
 * @brief standard delay is 1 sec
 */
#define DELAYVAL 1000



const unsigned VMIN = DAC_VMIN;
const unsigned VMAX = DAC_VMAX;

unsigned v0 = VMAX/2;
unsigned v1 = 0;

void GenerateValues(void) {

    DAC_SetCombOutput(v0,v1);
    v0++;
    v1++;
    if( v0 == VMAX ) v0 = 0;
    if( v1 == VMAX ) v1 = 0;
}
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
    GenerateValues();
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

    // Initialize DAC
    unsigned conf = DAC_CONFIG_VREF_VDD
                   |DAC_CONFIG_SINGLE_ENDED_OUTPUT
                   |DAC_CONFIG_ENABLE_CH0
                   |DAC_CONFIG_ENABLE_CH1;

    DAC_Init(conf,500000);

    /* Enable interrupts */
    __enable_irq();

    /* Main  loop */
    char c;
    while (1) {
        c = getchar();
        DAC_SetOutput(0,VMAX);
        c = getchar();
        DAC_SetOutput(VMAX/2,VMAX/2);
        c = getchar();
        DAC_SetOutput(VMAX,0);
        (void) c;
    }
}
