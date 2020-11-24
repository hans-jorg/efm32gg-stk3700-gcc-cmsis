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

/** ***************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Just calls Task_Update
 * @note   Called every 1 ms
 */

void SysTick_Handler(void) {
    Task_Update();
}

/** ***************************************************************************
 * @brief  Blinker processing routine
 *
 * @note   Called every 1 second
 */

void Blinker1(void) {

        LED_Toggle(LED1);
}

void Blinker2(void) {

        LED_Toggle(LED2);
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
    Task_Add(Blinker1,2000,0);
    Task_Add(Blinker2,1700,0);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/DIVIDER);

    /* Blink loop */
    while (1) {
        Task_Dispatch();
    }

}
