/** ***************************************************************************
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.0
******************************************************************************/

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

/** **************************************************************************
 * @brief  Context for Protothreads Blinker1 and Blinker2
 *
 */
struct pt pt1;
uint32_t threshold1,period1=2000;

struct pt pt2;
uint32_t threshold2,period2=1700;

/** **************************************************************************
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

PT_THREAD(Blinker1(struct pt *pt)) {

    PT_BEGIN(pt);

    LED_Write(0,LED1);

    while(1) {
        // Processing
        LED_Toggle(LED1);
        threshold1 = timer_counter+period1;
        PT_WAIT_UNTIL(pt,timer_counter>=threshold1);
    }

    (void) PT_YIELD_FLAG; // to silence compiler warning

    PT_END(pt);

}

PT_THREAD(Blinker2(struct pt *pt)) {

    PT_BEGIN(pt);

    LED_Write(0,LED2);

    while(1) {
        // Processing
        LED_Toggle(LED2);
        threshold2 = timer_counter+period2;
        PT_WAIT_UNTIL(pt,timer_counter>=threshold2);
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
    PT_INIT(&pt1);
    PT_INIT(&pt2);

    /* Blink loop */
    while (1) {
        Blinker1(&pt1);
        Blinker2(&pt2);
    }

}
