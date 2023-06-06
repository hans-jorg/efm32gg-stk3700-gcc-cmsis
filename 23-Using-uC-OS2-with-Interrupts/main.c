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
#include "clock_efm32gg_ext.h"

#include "ucos_ii.h"
#include "led.h"

/**
 * @brief   Board Support Package (BSP)
 */
typedef unsigned int CPU_INT32U;

void  OS_CPU_TickInit (CPU_INT32U  tick_rate)
{
    CPU_INT32U  cnts;
    CPU_INT32U  cpu_freq;
    
    cpu_freq = SystemCoreClock;
    cnts     = (cpu_freq / tick_rate);                          /* Calculate the number of SysTick counts               */

    OS_CPU_SysTickInit(cnts);                                   /* Call the Generic OS Systick initialization           */
}

/**
 * @brief Semiperiod values for LED1 and LED2
 */
//{
#define DELAY0 750
#define DELAY1 1750
//}


/**
 * Stacks for tasks
 */
//{
static OS_STK TaskStartStack[APP_CFG_STARTUP_TASK_STK_SIZE];
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

    /* Configure LEDs */
    LED_Init(LED1|LED2);
    LED_Write(0,LED1|LED2);                                     // Turn them on

    // Initialize the Tick interrupt (CMSIS way)
    SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC);

    // Initialize the Tick interrupt (uCOS way)
    OS_CPU_TickInit(OS_TICKS_PER_SEC);

    
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                               // Determine CPU capacity
#endif

#if 1                  
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

#endif

    OSTaskDel(OS_PRIO_SELF);                                    // Kill itself. Task should never return
}




/**
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO, HFCORECLK = HFCLK, HFPERCLK  = HFCLK
 */
int main(void) { 

    __disable_irq();

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    // Initialize uc/os II
    OSInit();
     
    // Create a task to start the other tasks
    OSTaskCreate(   TaskStart,                                          // Pointer to function
                    (void *) 0,                                         // Parameter for task
                    (void *) &TaskStartStack[APP_CFG_STARTUP_TASK_STK_SIZE-1],    // Initial value of SP
                    APP_CFG_STARTUP_TASK_PRIO);                                    // Task Priority/ID

    // Effectively starting uC/OS
    __enable_irq();
    
    // Enter uc/os and never returns
    OSStart();

}
