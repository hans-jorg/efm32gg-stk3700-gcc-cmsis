/**
 * @note    Just blinks the LEDs of the STK3700
 *
 * @note    LEDs are on pins 2 and 3 of GPIO Port E
 *
 * @note    It uses a LED HAL.
 *
 * @note    It uses code in system-efm32gg-ext to change clock frequency.
 *
 * @note    All processing is done in the interrupt routine.
 *
 * @author  Hans
 * @date    01/09/2018
 */
 
#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "clock_efm32gg.h"
#include "led.h"

#define SYSTICKDIVIDER 1000
#define SOFTDIVIDER 1000

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

void SysTick_Handler(void) {
static int counter = 0;             // must be static
static int8_t state = 0;            // must be static

    if( counter != 0 ) {
        counter--;
    } else {
        switch(state) {
        case 0:
            LED_Toggle(LED1);
            state = 1;
            break;
        case 1:
            LED_Toggle(LED2);
            state = 2;
            break;
        case 2:
            LED_Write(0,LED1|LED2);
            state = 0;
            break;
        }
        counter = SOFTDIVIDER-1;
    }
}

/*****************************************************************************
 * @brief  Main function
 *
 * @note   Using default clock configuration
 *         HFCLK = HFRCO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {


    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    
    /* Configure Pins in GPIOE */
    LED_Init(LED1|LED2);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/SYSTICKDIVIDER);

    /* Blink loop */
    while (1) {}

}
