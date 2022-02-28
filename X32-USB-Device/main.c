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
#include "clock_efm32gg2.h"
#include "led.h"
#include "lcd.h"

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

//{
#define SYSTICKDIVIDER 1000
#define SOFTDIVIDER 1000


static uint64_t ticks = 0;

void SysTick_Handler(void) {
static int counter = 0;             // must be static

    ticks++;
    
    if( counter != 0 ) {
        counter--;
    } else {
        counter = SOFTDIVIDER-1;
        LED_Toggle(LED1);
    }
}

//}


/*****************************************************************************
 * @brief  Delay function based on SysTick
 *****************************************************************************/


void
Delay(uint32_t v) {
uint64_t lim = ticks+v;       // Missing processing of overflow here

    while ( ticks < lim ) {}

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
char line[100];
int tryn = 0;

    /* Configure LEDs */
    LED_Init(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    /* Turn on LEDs */
    LED_Write(0,LED1|LED2);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/SYSTICKDIVIDER);

    // Message
    printf("Starting......");
    
    /* Configure LCD */
    LCD_Init();

    LCD_SetAll();
    Delay(DELAYVAL);

    LCD_ClearAll();
    Delay(DELAYVAL);

    LCD_WriteString("hello");


    // Enable IRQs
    __enable_irq();

    printf("Hello\n");
    while (1) {
        LED_Toggle(LED2);
        printf("Try %d\n",tryn++);
        printf("\nYour name: ");
        fgets(line,99,stdin);
        printf("Hello %s\n",line);
        Delay(100);
    }

}

 #if 0
int main(void) {
//uint32_t v;
const uint32_t DELAYVAL = 2;

    /* Configure Pins in GPIOE */
    LED_Init(LED1|LED2);

    LED_On(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/SYSTICKDIVIDER);

    // Message
    printf("Starting......");
    
    /* Configure LCD */
    LCD_Init();

    LCD_SetAll();
    Delay(DELAYVAL);

    LCD_Clear_All();
    Delay(DELAYVAL);

    LCD_WriteString("hello");

    // Configure ADC for temperature measurement 
    Temperature_Init(500000);
    
    /* Blink loop */
    __enable_irq();
    
    while (1) {

       // v = Temperature_GetRawValue();

        //printf("t=%u\n",(unsigned) v);
        
        Delay(DELAYVAL);
        LED_Toggle(LED1);                                // Toggle LED1 

    }
}
#endif
