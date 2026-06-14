/** ***************************************************************************
 * @file    main.c
 * @brief   Simple Flash Demo for EFM32GG_STK3700
 * @version 1.0
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "lfs.h"
#include "lfs_interface.h"


/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"
#include "clock_efm32gg_ext.h"

#include "led.h"
#include "nand-flash.h"


//void UART0_TX_IRQHandler(void);
#define ENTER_ATOMIC() __disable_irq()
#define EXIT_ATOMIC()  __enable_irq()

/*************************************************************************//**
 * @brief  Sys Tick Handler
 */
const int TickDivisor = 1000; // milliseconds

volatile uint64_t tick = 0;

void SysTick_Handler (void) {
static int counter = 0;

    tick++;

    if( counter == 0 ) {
        counter = TickDivisor;
        // Process every second
        LED_Toggle(LED1);
    }
    counter--;
}


void Delay(int delay) {
uint64_t l = tick+delay;

    while(tick<l) {}

}

uint8_t spare[NAND_SPARESIZE];
uint8_t signature[4];
uint8_t page[NAND_PAGESIZE];
/**************************************************************************//**
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {
//char line[100];
uint32_t addr;
int i;
int32_t rc;
uint32_t s;

    /* Configure LEDs */
    LED_Init(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    //(void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    /* Turn on LEDs */
    LED_Write(0,LED1|LED2);

    /* Configure SysTick */
    SysTick_Config(SystemCoreClock/TickDivisor);

    __enable_irq();

    printf("Initializing NAND Flash\n");
    rc = NAND_Init();
    if( rc != 0 ) {
        printf("Error at initialization\n");
        //goto error;
    }
    s = NAND_Status();
    printf("Status = %02lX\n",s);

    NAND_DisableWriteProtect();
    s = NAND_Status();
    printf("Status = %02lX\n",s);

    NAND_EnableWriteProtect();
    s = NAND_Status();
    printf("Status = %02lX\n",s);


    rc = NAND_ReadSignature(signature);
    if( rc != 0 ) {
        printf("Error when reading signature\n");
       // goto error;
    }
    printf("Signature = %02X%02X\n",signature[0],signature[1]);

    int errors = 0;
    putchar('\n');
    for(addr=0;addr<NAND_MAXADDRESS;addr+=NAND_PAGESIZE) {
        printf("\rReading spare at address %08lX",addr);
        memset(spare,0xAA,NAND_SPARESIZE);
        rc = NAND_ReadSpare(addr, spare);
        if( rc == 0 ) {
            printf("\nError when reading spare area\n");
            //goto error;
        }
        int err = 0;
        for(i=0;i<NAND_SPARESIZE;i++) {
            if( spare[i] != 0xFF ) {
                err++;
                printf(" [%d]=%02X",i,spare[i]);
            }
        }
        if( err ) {
            putchar('\n');
            errors++;
        }
        //Delay(2000);
    }
    printf("\nPages with errors: %d\n",errors);
//error:
    while(1) {} // Stop here !!!!!!
}
