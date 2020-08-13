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
#include "button.h"



void buttoncallback(uint32_t v) {

    if( Button_ReadReleased() )
        LED_Toggle(LED1);

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
volatile uint32_t b;

    /* Configure LEDs */
    LED_Init(LED1|LED2);

    /* Configure buttons */
    Button_Init(BUTTON1|BUTTON2);
    Button_SetCallback(buttoncallback);

    /* Enable interrupts */
    __enable_irq();

    LED_Write(0,LED1|LED2);
    /*
     * Read button loop
     *
     * ATTENTION: No debounce
     */
    while (1) {
#if 0
        b = Button_ReadReleased();
        if( b&BUTTON0 ) {
            LED_Toggle(LED1);
        }
        if( b&BUTTON1 ) {
            LED_Toggle(LED2);
        }
#else
        b = 1000000;
        while(b) {b--;}

        // Wait until button is pressed
        while (Button_Read()!=BUTTON_PRESSED) {}
        cnt = 0;
        do {
            Delay(T1MS);
            cnt++;
            if ( Button_Read()!=BUTTON_PRESSED)
                cnt = 0;    // Start again
        } while (cnt < 3);

        LED_Toggle(LED2);
#endif
    }

}
