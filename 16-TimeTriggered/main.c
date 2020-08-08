/** ***************************************************************************
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.0
******************************************************************************/

#include <stdint.h>

#include "tt_tasks.h"

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
 * @note   Just calls Task_Update
 * @note   Called every 1 ms
 */

void SysTick_Handler(void) {
    Task_Update();
}

/*****************************************************************************
 * @brief  Blinker processing routine
 *
 * @note   Called every 1 second
 */

void Blinker(void) {
static int8_t state = 0;            // must be static

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

    /* Configure LEDs */
    LED_Init(LED1|LED2);

    /* Initialize Task Kernel */
    Task_Init();
    Task_Add(Blinker,1000,0);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/DIVIDER);

    /* Enable Interrupts */
    __enable_irq();

    /* Blink loop */
    while (1) {
        Task_Dispatch();
    }

}
