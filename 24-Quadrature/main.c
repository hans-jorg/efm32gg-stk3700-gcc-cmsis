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

#include "em_device.h"
#include "led.h"
#include "lcd.h"
#include "temperature.h"
#include "clock_efm32gg2.h"
#include "button.h"
#include "conv.h"
#include "gpio.h"

//#define USING_PULSE_DIR_GPIO_SIGNALS
#define USING_A_B_SIGNALS

/*****************************************************************************
 * @brief  Quadrature decoder
 *****************************************************************************/

/**
 * @brief   Quadrature decoder
 *
 * @note    Pins must be in same GPIO port
 */

#define QUADRATURE_GPIO     GPIOD
static int qcounter = 0;     /* counter */
#define QUADRATURE_DEBOUNCE_N   3

static uint32_t qnow = 0;

static uint32_t qlast[QUADRATURE_DEBOUNCE_N];
static int qi = 0;

#ifdef USING_A_B_SIGNALS
#define QUADRATURE_APIN         6
#define QUADRATURE_BPIN         7
#define QUADRATURE_BUTTONPIN    5
#define QUADRATURE_A            (1U<<QUADRATURE_APIN)
#define QUADRATURE_B            (1U<<QUADRATURE_BPIN)
#define QUADRATURE_BUTTON       (1U<<QUADRATURE_BUTTONPIN)

#define QUADRATURE_M            (QUADRATURE_A|QUADRATURE_B)
#define AB_00                   0
#define AB_01                   QUADRATURE_B
#define AB_10                   QUADRATURE_A
#define AB_11                   (QUADRATURE_A|QUADRATURE_B)





void Quadrature_Process(void) {
typedef enum { Q0, Q1, Q2, Q3 } state_t;
static state_t state = Q0;  /* initial state */
uint32_t qand, qor;
uint32_t ab;
uint32_t qread;

    qread = GPIO_ReadPins(QUADRATURE_GPIO)&QUADRATURE_M;

    qlast[qi++] = qread;
    if( qi >= QUADRATURE_DEBOUNCE_N ) qi = 0;

    qand = qor = qlast[0];
    for(int i=1;i<QUADRATURE_DEBOUNCE_N;i++) {
        qand &= qlast[i];
        qor  |= qlast[i];
    }
    qnow |= qand;           // set if there are all 1s
    qnow &= qor;            // reset if there are all 0s

    ab = qnow&QUADRATURE_M;

    switch(state) {
    case Q0:
        if( ab == AB_01 ) {
            state = Q1;
            qcounter++;
        } else if ( ab == AB_10 ) {
            state = Q3;
            qcounter--;
        }
        break;
    case Q1:
        if( ab == AB_11 ) {
            state = Q2;
            qcounter++;
        } else if ( ab == AB_00 ) {
            state = Q0;
            qcounter--;
        }
        break;
    case Q2:
        if( ab == AB_10 ) {
            state = Q3;
            qcounter++;
        } else if ( ab == AB_01 ) {
            state = Q1;
            qcounter--;
        }
        break;
    case Q3:
        if( ab == AB_10 ) {
            state = Q0;
            qcounter++;
        } else if ( ab == AB_11 ) {
            state = Q2;
            qcounter--;
        }
        break;
    }
}

#endif

/*
 * Using one signal as pulse and other as direction
 */
#ifdef USING_PULSE_DIR_GPIO_SIGNALS

#define QUADRATURE_PULSEPIN     7
#define QUADRATURE_DIRPIN       6
#define QUADRATURE_BUTTONPIN    5

#define QUADRATURE_PULSE        (1U<<QUADRATURE_PULSEPIN)
#define QUADRATURE_DIR          (1U<<QUADRATURE_DIRPIN)
#define QUADRATURE_BUTTON       (1U<<QUADRATURE_BUTTONPIN)

#define QUADRATURE_M            (QUADRATURE_PULSE|QUADRATURE_DIR)


void Quadrature_Init(void) {

    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M,0);

}

void Quadrature_Process(void) {
static uint32_t qant = 0;
uint32_t qand, qor;
uint32_t qread;

    qread = GPIO_ReadPins(QUADRATURE_GPIO)&QUADRATURE_M;

    qlast[qi++] = qread;
    if( qi >= QUADRATURE_DEBOUNCE_N ) qi = 0;

    qand = qor = qlast[0];
    for(int i=1;i<QUADRATURE_DEBOUNCE_N;i++) {
        qand &= qlast[i];
        qor  |= qlast[i];
    }
    qnow |= qand;           // set if there are all 1s
    qnow &= qor;            // reset if there are all 0s
    
        if( (qnow^qant)&QUADRATURE_PULSE ) {
            if( qnow&QUADRATURE_PULSE ) {
            #if 1
                if( qnow&QUADRATURE_DIR ) {
                    qcounter++;
                } else {
                    qcounter--;
                }
            #else
                qcounter++;
            #endif
            }
        }
        qant = qnow;
}


#endif   

// These routines are identical for each deboding method

void Quadrature_Init(void) {

    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M|QUADRATURE_BUTTON,0);

}

static inline int Quadrature_GetPosition(void) {

    return qcounter;
}


static inline int Quadrature_GetButtonStatus(void) {

    return qnow&QUADRATURE_BUTTON;
}


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
        qinterval = 10;
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
char s[10];
int counter = 0;

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

    LCD_Clear();
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

    // Enable IRQs
    __enable_irq();
    int cnt=0;
    while (1) {
        //Quadrature_Process();
        //Delay(100);
        if( Quadrature_GetButtonStatus() )
            qcounter = 0;
        if( cnt == 0 ) {
            printf("%lX\n",qnow);
            counter = Quadrature_GetPosition();
            itoa(counter,s);
            LCD_WriteString(s);
            cnt = 100;
        } else {
            cnt--;
        }
    }
}
