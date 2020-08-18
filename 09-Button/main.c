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
#include "clock_efm32gg.h"
#include "led.h"
#include "button.h"

/*****************************************************************************
 * @brief  Main function
 *
 * @note   Using default clock configuration
 *         HFCLK = HFRCO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {
uint32_t b;


    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);

    /* Configure buttons */
    Button_Init(BUTTON1|BUTTON2);

    /*
     * Read button loop
     *
     * ATTENTION: No debounce
     */
    while (1) {
        b = Button_ReadReleased();
        if( b&BUTTON1 ) {
            LED_Toggle(LED1);
        }
        if( b&BUTTON2 ) {
            LED_Toggle(LED2);
        }
    }

}
