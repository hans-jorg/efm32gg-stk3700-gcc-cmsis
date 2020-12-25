/** ***************************************************************************
 * @file    main.c
 * @brief   Simple UART Demo for EFM32GG_STK3700
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
#include "clock_efm32gg_ext.h"

#include "led.h"
#include "uart.h"


/*************************************************************************//**
 * @brief  Sys Tick Handler
 */
const int TickDivisor = 1000; // milliseconds

void SysTick_Handler (void) {
static int counter = 0;
    if( counter == 0 ) {
        counter = TickDivisor;
        // Process every second
        LED_Toggle(LED1);
    }
    counter--;
}


/*****************************************************************************
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {
ClockConfiguration_t clockconf;
int cntchar;
unsigned ch,checho;

    /* Configure LEDs */
    LED_Init(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

#if 1
    ClockGetConfiguration(&clockconf);
#endif
    /* Turn on LEDs */
    LED_Write(0,LED1|LED2);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/TickDivisor);

    /* Configure UART */
    UART_Init();

    cntchar = 0;
    ch = '*';
    checho = ch;
    while (1) {

        if( (ch = UART_GetCharNoWait()) != 0 ) {
            LED_Toggle(LED2);
            if( (ch != '\n') && (ch != '\r') )
                checho = ch;
        }
        if( (cntchar++%80)!=0 ) {
            UART_SendChar(checho);
        } else {
            UART_SendChar('\n');
            UART_SendChar('\r');
        }
    }

}
