/** ***************************************************************************
 * @file    main.c
 * @brief   Simple Flash Demo for EFM32GG_STK3700
 * @version 1.0
******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdnoreturn.h>

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
#define ENTER_ATOMIC()              __disable_irq()
#define EXIT_ATOMIC()               __enable_irq()

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

/**************************************************************************//**
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */
uint8_t spare[NAND_SPARESIZE];
uint8_t signature[4];
uint8_t page[NAND_PAGESIZE];
uint8_t fullpage[NAND_FULLPAGESIZE];

int main(void) {
char line[100];
int i;
int32_t rc;
uint32_t s;
uint32_t addr;
uint32_t pagenumber;
uint32_t pageaddr;
int errors = 0;
int errorsinpage = 0;

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
    if( (rc&NAND_STATUS_ERROR) != 0 ) {
        printf("Error at initialization\n");
        //goto error;
    }
    s = NAND_Status();
    printf("Status = %02X\n",(unsigned) s);

    NAND_DisableWriteProtect();
    s = NAND_Status();
    printf("Status = %02X\n",(unsigned) s);

    NAND_EnableWriteProtect();
    s = NAND_Status();
    printf("Status = %02X\n",(unsigned) s);


    rc = NAND_ReadSignature(signature);
    if( (rc&NAND_STATUS_ERROR) != 0 ) {
        printf("Error when reading signature\n");
       // goto error;
    }
    printf("Signature = %02X%02X\n",signature[0],signature[1]);

    while(1) {
        printf("\nEnter \n");
        printf("1 - Verifying if flash is empty\n");
        printf("2 - Test read operation\n");
        printf("3 - Test write operation\n");
        printf("4 - Test erase operation\n");
        printf("->");
        fgets(line,10,stdin);
        int opt = atoi(line);
        putchar('\n');
        switch(opt) {
        case 1:
        /*
         *  Reading all pages and testing if empty
         */
            printf("\nReading full pages\n");
            for(addr=0;addr<NAND_MAXADDRESS;addr+=NAND_PAGESIZE) {
                printf("%08X\r",(unsigned) addr);
                memset(fullpage,0xAA,NAND_FULLPAGESIZE);
                rc = NAND_ReadFullPage(addr, fullpage);
                if( rc == 0 ) {
                    printf("\nError when reading full page at address %08X\n",
                        (unsigned) addr);
                    //goto error;
                }
                errorsinpage = 0;
                for(i=0;i<NAND_SPARESIZE;i++) {
                    if( fullpage[i] != 0xFF ) {
                        errorsinpage++;
                        printf(" [%d]=%02X",i,fullpage[i]);
                    }
                }
                if( errorsinpage ) {
                    putchar('\n');
                    errors++;
                }
            }
            printf("\nPages with errors: %d\n",errors);
            break;
        case 2:
            /*
            *  Testing read operations
            */
            errors = 0;
            printf("Testing reading operations\n");
            pagenumber = 14353;
            printf("Enter the page number: ");
            fgets(line,10,stdin);
            pagenumber = atoi(line);
            pageaddr = pagenumber<<NAND_PAGEADDRESS_POS;
            printf("\nReading spare\n");
            rc = NAND_ReadSpare(pageaddr, spare);
            if( rc == 0 ) {
                printf("\nError when reading spare area\n");
                //goto error;
            }
            errorsinpage = 0;
            for(i=0;i<NAND_SPARESIZE;i++) {
                if( spare[i] != 0xFF ) {
                    errorsinpage++;
                    printf(" [%d]=%02X",i,spare[i]);
                }
            }
            if( errorsinpage ) {
                putchar('\n');
                errors++;
            }
            printf("\n#spares with errors: %d\n",errors);
            errors = 0;
            printf("\nReading page area\n");
            memset(page,0xAA,NAND_PAGESIZE);
            rc = NAND_ReadPage(pageaddr, page);
            if( rc == 0 ) {
                printf("\nError when reading page area\n");
                //goto error;
            }
            for(i=0;i<NAND_PAGESIZE;i++) {
                if( page[i] != 0xFF ) {
                    errorsinpage++;
                    printf("\n[%d]=%02X",i,page[i]);
                }
            }
            if( errorsinpage ) {
                putchar('\n');
                errors++;
            }
            printf("\n# pages with errors: %d\n",errors);
            break;
        case 3:
            /*
            * Testing Write operations
            */
            printf("Testing write operations\n");
            pagenumber = 14353;
            printf("Enter the number of page to be written: ");
            fgets(line,10,stdin);
            pagenumber = atoi(line);
            pageaddr = pagenumber<<NAND_PAGEADDRESS_POS;
            printf("Writing a page into page %d at address %08X\n",
                (int) pagenumber,(unsigned) pageaddr);
            // Fill data to be written
            for(i=0;i<NAND_PAGESIZE;i++) {
                page[i] = (uint8_t) i;
            }
            rc = NAND_WritePage(pageaddr,page);
            if( rc&NAND_STATUS_ERROR ) {
                printf("Error when writing the page %d at address %08X\n",
                    (int) pagenumber,(unsigned) pageaddr);
            } else {
                printf("OK\n");
            }
            printf("Reading the same page\n");
            memset(page,0x55,NAND_PAGESIZE);
            rc = NAND_ReadPage(pageaddr,page);
            if( rc&NAND_STATUS_ERROR ) {
                printf("Error when reading the page %d at address %08X\n",
                    (int) pagenumber,(unsigned) pageaddr);
            } else {
                printf("OK\n");
            }
            errors = 0;
            errorsinpage = 0;
            for(i=0;i<NAND_PAGESIZE;i++) {
                if ( page[i] != (uint8_t) i ) {
                    errorsinpage++;
                }
            }
            if( errorsinpage == 0 ) {
                printf("OK\n");
            } else {
                printf("%d error(s) when writing the page %d at address %08X\n",
                    (int) errorsinpage,
                    (unsigned) pagenumber,
                    (unsigned) pageaddr);
            }
            break;
        case 4:
            printf("Testing erase operation\n");
            pagenumber = 14356;
            printf("Enter the number of a page inside the block: ");
            fgets(line,10,stdin);
            pagenumber = atoi(line);
            uint32_t pageaddr = pagenumber<<NAND_PAGEADDRESS_POS;
            uint32_t blockaddr = NAND_ALIGNBLOCKADDRESS(pageaddr);
            printf("Erasing block at %08X containing page #%08X\n",
                    (unsigned) blockaddr,
                    (unsigned) pagenumber);
            rc = NAND_EraseBlock(blockaddr);
            if( rc&NAND_STATUS_ERROR ) {
                printf("Error when erasing block at %08X\n",
                    (unsigned) blockaddr);
            }
            for(addr=blockaddr;
                NAND_ALIGNBLOCKADDRESS(addr)!=blockaddr;
                addr+=NAND_PAGESIZE) {
                rc = NAND_ReadPage(addr,page);
                if( rc&NAND_STATUS_ERROR ) {
                    printf("Error when erasing block at %08X\n",
                        (unsigned) blockaddr);
                }
                errorsinpage = 0;
                for(i=0;i<NAND_PAGESIZE;i++) {
                    if( page[i] != 0xFF ) {
                        printf("Error when reading page at %08X\n",
                            (unsigned) addr);
                        errorsinpage++;
                    }
                }
                if( errorsinpage != 0 ) {
                    printf("Erasing not completed\n");
                } else {
                    printf("OK\n");
                }
            }
            break;
        }
    }
//error:
    while(1) {} // Stop here !!!!!!
}
