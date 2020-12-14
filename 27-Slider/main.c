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
#include <stdint.h>
#include <stdio.h>

#include "em_device.h"
#include "led.h"
#include "lcd.h"
#include "touch.h"
#include "clock_efm32gg2.h"

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

//{
#define SYSTICKDIVIDER 1000
#define SOFTDIVIDER 1000
#define TOUCH_PERIOD 10

static uint32_t ticks = 0;

void SysTick_Handler(void) {
static int counter = 0;             // must be static
static int touchcounter = 0;        // must be static

    ticks++;                        // overflow in 49 days

    /* Blink */
    if( counter != 0 ) {
        counter--;
    } else {
        counter = SOFTDIVIDER-1;
        LED_Toggle(LED1);
    }

    /* Touch processing */
    if( touchcounter != 0 ) {
        touchcounter--;
    } else {
        touchcounter = TOUCH_PERIOD;
        Touch_PeriodicProcess();
    }
}

//}


/*****************************************************************************
 * @brief  Delay function based on SysTick
 *****************************************************************************/


void
Delay(uint32_t v) {
uint32_t lim = ticks+v;       // Missing processing of overflow here

    while ( ticks < lim ) {}

}


/**
 * @brief printbin
 */
void printbin(unsigned v, int n) {
unsigned m = 1<<(n-1);

    while(m) {
        if( (v&m) != 0 )
            putchar('1');
        else
            putchar('0');
        m>>=1;
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

#include "uart2.h"

#define DELAYVAL 2

int main(void) {
unsigned v;
char s[10];
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    
    // Message
    printf("\nHello!");

    // Message
    printf("\nStarting......\n");

    // Turn on LEDs
    LED_Write(0,LED1|LED2);

    // Configure SysTick
    SysTick_Config(SystemCoreClock/SYSTICKDIVIDER);

    // Configure LCD
    LCD_Init();

    LCD_SetAll();
    Delay(DELAYVAL);

    LCD_ClearAll();
    Delay(DELAYVAL);

    s[0] = 0;
    LCD_WriteString(s);

    LCD_WriteString("hello");


    // Configure touch sensor 
    Touch_Init();
   
    // Enable IRQs
    __enable_irq();

    int cnt = 0;
    while (1) {
        LED_Toggle(LED2);
        v = Touch_Read();
        printf("%d: ",cnt);
        printbin(v,TOUCH_N);
        printf("\n");
        cnt++;
        Delay(1000);
    }

}





