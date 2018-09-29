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

#include "pt.h"

#define DIVIDER 1000

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Just calls Task_Update
 * @note   Called every 1 ms
 */

uint32_t timer_counter = 0;
void SysTick_Handler(void) {
    timer_counter++;
}

/*****************************************************************************
 * @brief  Blinker processing routine
 *
 */

struct pt pt;
uint32_t threshold;

#define PT_DELAY(T) threshold = timer_counter+(T); PT_WAIT_UNTIL(pt,timer_counter>=threshold);

PT_THREAD(Blinker(struct pt *pt)) {

    PT_BEGIN(pt);

    LED_Write(LED2,LED2);

    while(1) {
        // Processing
        LED_Toggle(LED1|LED2);
        PT_DELAY(1000);

        LED_Toggle(LED1|LED2);
        PT_DELAY(1000);

//        LED_Write(0,LED1|LED2);
//        PT_DELAY(1000);

    }

    (void) PT_YIELD_FLAG; // to silence compiler warning

    PT_END(pt);

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


    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/DIVIDER);

    /* Initialize Protothreads */
    PT_INIT(&pt);

    /* Enable Interrupts */
    __enable_irq();

    /* Blink loop */
    while (1) {
        Blinker(&pt);
    }

}
