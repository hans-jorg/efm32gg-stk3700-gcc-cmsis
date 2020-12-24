/** ***************************************************************************
 * @file    main.c
 * @brief   Simple UART Demo for EFM32GG_STK3700
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
#include "em_chip.h"
#include "em_system.h"

#include "clock_efm32gg.h"

#include "FreeRTOS.h"
#include "task.h"

#include "led.h"
#include "button.h"

#define TASKDELAYUNTIL

/***************************************************************************
 *                                                                         *
 *              Global variables and definitions                           *
 *                                                                         *
 ***************************************************************************/



/// Blinking state (ative|inactive)
uint32_t blinking = 1;                  // blinking active

/**
 * @brief Control blinking period
 */

//@{
uint32_t semiperiodmax  = 5000;
uint32_t semiperiodmin  = 100;
uint32_t semiperiodstep = 100;

uint32_t semiperiod0    = 1250;         // 1 sec
uint32_t semiperiod1    = 1000;        // 1.5 sec


/***************************************************************************
 *                                                                         *
 *              Tasks                                                      *
 *                                                                         *
 ***************************************************************************/


/**
 * @brief  Blink LED1 Task
 *
 */

void Task_BlinkLED1(void *pvParameters){
const portTickType xFrequency = 1000;
portTickType xLastWakeTime=xTaskGetTickCount();

    while(1) {
        if( blinking ) {
            LED_Toggle(LED1);
#ifdef TASKDELAYUNTIL
            vTaskDelayUntil(&xLastWakeTime,xFrequency);
#else
            vTaskDelay(semiperiod0);
#endif
        } else {
            LED_Write(0,LED1);
        }
    }
}

/**
 * @brief  Blink LED2 Task
 *
 */
void Task_BlinkLED2(void *pvParameters){
const portTickType xFrequency = 500;
portTickType xLastWakeTime=xTaskGetTickCount();

    while(1) {
        if( blinking ) {
            LED_Toggle(LED2);
#ifdef TASKDELAYUNTIL
            vTaskDelayUntil(&xLastWakeTime,xFrequency);
#else
            vTaskDelay(semiperiod1);
#endif
        } else {
            LED_Write(0,LED2);
        }
    }
}
//@}


/**
 * @brief  Switch Control Task
 *
 */
//@{
#define DEBOUNCE_TIME 40

void Task_Button(void *pvParameters) {

    while(1) {
        if( (Button_Read()&BUTTON1) != 0 ) {
            blinking = ! blinking;
            vTaskDelay(DEBOUNCE_TIME);
            while( (Button_Read()&BUTTON1) != 0 ) {
                vTaskDelay(DEBOUNCE_TIME);
            }
        }
    }

}
//@}


static void STOP(void) { while(1) {} }

/**************************************************************************//**
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO
 *         HFCORECLK = HFCLK
 *         HFPERCLK  = HFCLK
 */

int main(void) {
BaseType_t rc;

    // Apply erratas
    CHIP_Init();
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);



    Button_Init(BUTTON0|BUTTON1);
    LED_Write(0,LED1|LED2);

 //   SysTick_Config(SystemCoreClock/1000);   /* 1 ms */

    rc = xTaskCreate(Task_BlinkLED1,"0", 1000,0,2,0);
    if( rc != pdPASS )
        STOP();

    rc = xTaskCreate(Task_BlinkLED2,"1", 1000,0,1,0);
    if( rc != pdPASS )
        STOP();

    rc = xTaskCreate(Task_Button,"Button", 1000,0,3,0);
    if( rc != pdPASS )
        STOP();

    vTaskStartScheduler();

    while(1) {}
}
