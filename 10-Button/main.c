/*************************************************************************//**
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
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
#include "clock_efm32gg.h"
#include "led.h"
#include "button.h"

/************************************************************************//**
 * @brief  Button callback
 *
 * @note   runs with disabled interrupts
 */

void buttoncallback(uint32_t v) {

    if( Button_ReadReleased() )
        LED_Toggle(LED1);

}

/*************************************************************************//**
 * @brief  Sys Tick Handler
 */
const int TickDivisor = 1000; // milliseconds

void SysTick_Handler (void) {
static int counter = 0;
    if( counter == 0 ) {
        counter = TickDivisor;
        // Process every second
        LED_Toggle(LED2);
    }
    counter--;
}

/*************************************************************************//**
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
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);

    /* Configure buttons */
    Button_Init(BUTTON1|BUTTON2);
    Button_SetCallback(buttoncallback);

    /* Enable interrupts */
    __enable_irq();

    /* Initial LED status = All on */
    LED_Write(0,LED1|LED2);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/TickDivisor);

    /*
     * Read button loop: ATTENTION: No debounce
     */
    while (1) {
        __WFI();        // Enter low power state
    }

}
