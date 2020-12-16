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
#include "temperature.h"
#include "clock_efm32gg2.h"

/*****************************************************************************
 * @brief  SysTick interrupt handler
 *
 * @note   Called every 1/DIVIDER seconds (1 ms)
 */

//{
#define SYSTICKDIVIDER 1000
#define SOFTDIVIDER 1000


static uint32_t ticks = 0;

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
uint32_t lim = ticks+v;       // Missing processing of overflow here

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
uint32_t v;
char s[10];
int t;

    // Message
    printf("Starting......");
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    /* Turn on LEDs */
    LED_Write(0,LED1|LED2);
    
    // Message
    printf("\nHello!");
    
    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/SYSTICKDIVIDER);

    /* Configure LCD */
    LCD_Init();

    LCD_SetAll();
    Delay(DELAYVAL);

    LCD_ClearAll();
    Delay(DELAYVAL);

    LCD_WriteString("hello");


    // Configure ADC for temperature measurement 
    Temperature_Init(500000);

    uint32_t caltemp = (DEVINFO->CAL>>16)&0xFF;              // 0x0FE0_81B2
    uint32_t caltempread = (DEVINFO->ADC0CAL2>>20)&0xFFF;   // 0x0FE0_81BE
    int32_t adcthu_num = 63;
    int32_t adcthu_den = 10;
    int32_t VREF = 1250;
    int32_t adcthv_num = 192;
    int32_t adcthv_den = 100;
    printf("cal = %d\n",(int) caltemp);
    printf("adc cal = %d\n",(int) caltempread);
    printf("adc thu = %d/%d\n",(int) adcthu_num,(int)adcthu_den);
    printf("adc thv = %d/%d\n",(int) adcthv_num,(int)adcthv_den);
    
    // Enable IRQs
    __enable_irq();

    while (1) {
        LED_Toggle(LED2);
        v = Temperature_GetRawReading();
        printf("T=%u  ",(unsigned) v);
        t = Temperature_GetUncalibratedValue();
        printf("%d  ", t);
        t = Temperature_GetCalibratedValue();
        printf("%d  ", t);
        sprintf(s,"%5d",t);
        LCD_WriteString(s);
        int v1 = caltemp + (caltempread-v)*adcthu_den/adcthu_num;
        printf("%d  ", v1);
        int v2 = caltemp*10 + (caltempread-v)*adcthu_den*10/adcthu_num;
        printf("%d  ", v2);
        int v3 = caltemp + ((caltempread-v)*VREF*adcthv_den)/(4096*adcthv_num);
        printf("%d\n",v3);
        Delay(1000);
    }

}



