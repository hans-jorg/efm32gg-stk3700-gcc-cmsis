/** ***************************************************************************
 * @file    main.c
 * @brief   Simple Test for I2C
 * @version 1.0
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "led.h"
#include "lcd.h"
#include "i2cmaster.h"

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

/**
 * I2C address for testing
 */
#define VALIDADDR                 0x11
#define NOTVALIDADDR              0x29


/*****************************************************************************
 * @brief  Main function
 *
 * @note   Using default clock configuration
 *         HFCLK = HFRCO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */
int main(void) {
int rc;
uint8_t data[10];

    /* Configure Pins in GPIOE */
    LED_Init(LED1|LED2);
    LED_Off(LED1|LED2);

    /* Configure LCD */
    LCD_Init();
    LCD_ClearAll();
    Delay(DELAYVAL);


    rc = I2CMaster_Init(I2C1,100,0);
    if( rc < 0 ) {
        fprintf(stderr,"Initialization failed(%d)\n",rc);
    }

    while (1) {

        rc = I2CMaster_Send(I2C1,VALIDADDR,data,1);
        if( rc < 0 ) {
            fprintf(stderr,"Detecting valid I2C device with address %d failed (%d)\n",VALIDADDR,rc);
        } else {
            fprintf(stderr,"Detected I2C device with address %d successful\n",VALIDADDR);
        }

        rc = I2CMaster_Send(I2C1,NOTVALIDADDR,data,1);
        if( rc < 0 ) {
            fprintf(stderr,"Detected an I2c device with address %d \n",NOTVALIDADDR);
        } else {
            fprintf(stderr,"Detected an I2c device with address"
                           " %d but the address should be invalid\n",NOTVALIDADDR);
        }
        Delay(1000000);
    }
    /* NEVER */
}
