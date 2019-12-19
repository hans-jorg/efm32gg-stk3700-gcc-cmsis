/** ***************************************************************************
 * @file    main.c
 * @brief   Simple UART Demo for EFM32GG_STK3700
 * @version 1.1
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
#include "clock_efm32gg.h"

#include "ucos_ii.h"
#include "led.h"


/**
 * @brief Semiperiod values for LED1 and LED2
 */
//{
#define DELAY0 1500
#define DELAY1 3500
//}


/**
 * Stacks for tasks
 */
//{
#define TASKSTART_STACKSIZE 100
#define TASK0_STACKSIZE 100
#define TASK1_STACKSIZE 100

static OS_STK TaskStartStack[TASKSTART_STACKSIZE];
static OS_STK Task0Stack[TASK0_STACKSIZE];
static OS_STK Task1Stack[TASK1_STACKSIZE];

//}

/**
 * @brief  Task for blinking LED1
 *
 * @note   LED_Init must be called before
 */

void Task0(void *param) {

    while(1) {
        LED_Toggle(LED1);
        OSTimeDly(DELAY0);
    }
}

/**
 * @brief  Task for blinking LED2
 *
 * @note   LED_Init must be called before
 */

void Task1(void *param) {

    while(1) {
        LED_Toggle(LED2);
        OSTimeDly(DELAY1);
    }
}

/**
 * @brief  Task for starting other tasks
 *
 * @note   It is recommended to create tasks when uc/os is already running.
 *         This enable the calibration of Stats module.
 */


void TaskStart(void *param) {

//
//    OS_CPU_TickInit(OS_TICKS_PER_SEC);        // Initialize the Tick interrupt

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                               // Determine CPU capacity
#endif
                  
    // Create a task to blink LED 0
    OSTaskCreate(   Task0,                                      // Pointer to task
                    (void *) 0,                                 // Parameter
                    (void *) &Task0Stack[TASK0_STACKSIZE-1],    // Initial value of SP
                    TASK0_PRIO);                                // Task Priority/ID
                            
    // Create a task to blink LED 1
    OSTaskCreate(   Task1,                                      // Pointer to task
                    (void *) 0,                                 // Parameter
                    (void *) &Task1Stack[TASK1_STACKSIZE-1],    // Initial value of SP
                    TASK1_PRIO);                                // Task Priority/ID

    // Effectively starting uC/OS
    __enable_irq();
    
    OSTaskDel(OS_PRIO_SELF);                    // Kill itself. Task should never return
}




/**
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO, HFCORECLK = HFCLK, HFPERCLK  = HFCLK
 */
int main(void) {

    
    // Initialize uc/os II
    OSInit();
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);
    LED_Write(0,LED1|LED2);         // Turn them on
    
    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    // Configure SysTick (Maybe this should be in TaskStart)
    SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC);
    
    // Create a task to start the other tasks
    OSTaskCreate(   TaskStart,                                          // Pointer to function
                    (void *) 0,                                         // Parameter for task
                    (void *) &TaskStartStack[TASKSTART_STACKSIZE-1],    // Initial value of SP
                    TASKSTART_PRIO);                                    // Task Priority/ID

    // Enter uc/os and never returns
    OSStart();

}
