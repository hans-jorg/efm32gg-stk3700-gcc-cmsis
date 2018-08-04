/** ***************************************************************************
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.0
******************************************************************************/

#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "led.h"

#define DIVIDER 1000

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
            LED_Toggle(LED0);
            state = 1;
            break;
        case 1:
            LED_Toggle(LED1);
            state = 2;
            break;
        case 2:
            LED_Write(0,LED0|LED1);
            state = 0;
            break;
        }
        counter = DIVIDER-1;
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

    /* Configure Pins in GPIOE */
    LED_Init(LED0|LED1);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/DIVIDER);

    /* Blink loop */
    while (1) {}

}
