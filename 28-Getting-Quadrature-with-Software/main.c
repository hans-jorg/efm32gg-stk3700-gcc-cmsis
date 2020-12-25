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
#include <stdlib.h>
#include <string.h>

#include "em_device.h"
#include "led.h"
#include "lcd.h"
#include "temperature.h"
#include "clock_efm32gg2.h"
#include "button.h"
#include "conv.h"
#include "quadrature.h"

#define QUADRATURE_POLLING 1


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
static int qinterval = 0;
    ticks++;
    
    if( counter != 0 ) {
        counter--;
    } else {
        counter = SOFTDIVIDER-1;
        LED_Toggle(LED1);
    }
#if 1
    if( qinterval <= 0 ) {
        Quadrature_Process();
        qinterval = QUADRATURE_POLLING;
     } else {
        qinterval--;
     }
#endif
}

//}


/*****************************************************************************
 *  Delay function based on SysTick
 *****************************************************************************/

/**
 * @brief   Delay using SysTick
 *
 * @note    ticks overun after 49 days
 */
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
int c = 0;


    // Message
    printf("Starting......");
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    /* Turn on LEDs */
    LED_Write(LED2,LED1);
    
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
    //int32_t VREF = 1250;
    int32_t adcthv_num = 192;
    int32_t adcthv_den = 100;
    printf("cal = %d\n",(int) caltemp);
    printf("adc cal = %d\n",(int) caltempread);
    printf("adc thu = %d/%d\n",(int) adcthu_num,(int)adcthu_den);
    printf("adc thv = %d/%d\n",(int) adcthv_num,(int)adcthv_den);

    Button_Init(BUTTON1|BUTTON2);

    Quadrature_Init();

    LCD_ClearAll();

    // Enable IRQs
    __enable_irq();

    static const char ch[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                       "abcdefghijklmnopqrstuvwxyz";
 //   int pos = 2;
    int ich = 0;
    int cant = 0;
    while (1) {
        c = Quadrature_GetPosition();
        if( c > cant ) {
            ich++;
            if( ich >= strlen(ch) ) ich=0;
        } else if ( c < cant ) {
            ich--;
            if( ich < 0 ) ich = strlen(ch)-1;
        }
        if( c != cant ) {
            char s[10];
            if( c < 0 ) {
                LCD_WriteSpecial(LCD_MINUS,0);
                c = -c;
            } else {
                LCD_WriteSpecial(LCD_MINUS,1);
            }
            itoa(c,s);
            LCD_WriteString(s);
            cant = c;
        }
        printf("%d %d '%c' \n",c,ich,ch[ich]);
        Delay(300);
        if( Quadrature_GetButtonStatus() ) {
           Quadrature_Reset();
           LED_Toggle(LED2);
           }
    }
}
