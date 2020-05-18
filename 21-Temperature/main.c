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

#define DELAYVAL 100

/*****************************************************************************
 * @brief  Quick and dirty delay function
 *****************************************************************************/

void Delay(uint32_t delay) {
volatile uint32_t counter;
int i;

    for(i=0;i<delay;i++) {
        counter = 10000;
        while( counter ) counter--;
    }
}

void WriteMultiple(uint8_t ch) {
int pos;

    for(pos=1;pos<=11;pos++)
        LCD_WriteChar(ch,pos);
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
uint32_t v;

    /* Configure Pins in GPIOE */
    LED_Init(LED1|LED2);

    /* Configure LCD */
    LCD_Init();

    LCD_SetAll();
    Delay(DELAYVAL);

    LCD_Clear();
    Delay(DELAYVAL);

    Temperature_Init(500000);
    
    /* Blink loop */
    while (1) {

        v = Temperature_GetRawValue();

        printf("t=%u\n",(unsigned) v);
        
        Delay(DELAYVAL);
        LED_Toggle(LED1);                                // Toggle LED1 

    }
}
