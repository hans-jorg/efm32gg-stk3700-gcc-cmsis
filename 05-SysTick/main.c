/**
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.0
 */

#include <stdint.h>
#include "em_device.h"
#include "led.h"

/**
 * @brief  SysTick is called every 1 ms
 */
#define DIVIDER 1000

/**
 * @brief standard delay is 1 sec
 */
#define DELAYVAL 1000

/**
 * @brief Tick counter
 * @note  Incremented every 1 ms
 */
uint32_t volatile TickCounter = 0;



/**
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

void SysTick_Handler(void) {
    TickCounter++;
}

/**
 * @brief  Better delay function
 *
 * @param  delay is in ms
 */

void Delay(uint32_t delay) {
volatile uint32_t initialvalue = TickCounter;

    while( (TickCounter-initialvalue) < delay ) {}
}

/**
 * @brief  Better delay function
 *
 * @param  delay is in clock pulses
 */

void DelayPulses(uint32_t delay) {

    SysTick->LOAD = delay;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

    while( (SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk) == 0 ) {}
    SysTick->CTRL = 0;
}


/**
 * Delay using Data Watchpoint and Trace (DWT)
 */

#define DWT_CONTROL *((volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT  *((volatile uint32_t *)0xE0001004)
#define SCB_DEMCR   *((volatile uint32_t *)0xE000EDFC)

static inline uint32_t getDwtCyccnt(void)
{
    return DWT_CYCCNT;
}

static inline void resetDwtCyccnt(void)
{
    DWT_CYCCNT = 0; // reset the counter
}

static inline void enableDwtCyccnt(void)
{
    SCB_DEMCR   = SCB_DEMCR |BIT(24);       // TRCENA = 1
    DWT_CONTROL = DWT_CONTROL | BIT(0) ;    // enable the counter (CYCCNTENA = 1)
    DWT_CYCCNT  = 0;                        // reset the counter
}

/**
 * Delay using Data Watchpoint and Trace (DWT)
 */


static inline uint32_t getDwtCyccnt2(void)
{
    return DWT->CYCCNT;
}

static inline void resetDwtCyccnt2(void)
{
    DWT->CYCCNT = 0; // reset the counter
}

static inline void enableDwtCyccnt2(void)
{
    CoreDebug->DEMCR = CoreDebug->DEMCR | BIT(24);       // TRCENA = 1
    DWT->CTRL  = DWT->CTRL  | BIT(0) ;    // enable the counter (CYCCNTENA = 1)
    DWT->CYCCNT= 0;                        // reset the counter
}

/**
 * @brief  Main function
 *
 * @note   Using default clock configuration
 * @note         HFCLK = HFRCO
 * @note         HFCORECLK = HFCLK
 * @note         HFPERCLK  = HFCLK
 */

int main(void) {

    /* Configure Pins in GPIOE */
    LED_Init(LED0|LED1);

   /* Configure SysTick */
    SysTick_Config(SystemCoreClock/DIVIDER);

    /* Enable interrupts */
    __enable_irq();

    /* Blink loop */
    while (1) {

        Delay(DELAYVAL);
        LED_Toggle(LED0);                                // Toggle LED0

        Delay(DELAYVAL);
        LED_Toggle(LED1);                                // Toggle LED1

        Delay(DELAYVAL);
        LED_Write(0,LED0|LED1);                          // Turn On All LEDs

    }
}
