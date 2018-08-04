/** ***************************************************************************
 * @file    main.c
 * @brief   Simple UART Demo for EFM32GG_STK3700
 * @version 1.0
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "clock_efm32gg.h"

#include "ucos_ii.h"

#include "led.h"
#include "uart.h"


/**************************************************************************//**
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {
char line[100];

    /* Configure LEDs */
    LED_Init(LED0|LED1);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    /* Turn on LEDs */
    LED_Write(0,LED0|LED1);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/1000);

    /* Configure UART */
    UART_Init();

    __enable_irq();

    printf("\r\n\n\n\rHello\n\r");
    while (1) {
        printf("\r\n\n\n\rWhat is your name?\n");
        fgets(line,99,stdin);
        printf("Hello %s\n",line);
    }

}
