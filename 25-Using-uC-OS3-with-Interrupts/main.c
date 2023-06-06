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
#include "clock_efm32gg_ext.h"


#include "app_cfg.h"
#include "os.h"

#include "led.h"
#include "uart.h"


/**
 *  Stop function. The parameter can indicate where it was called
 */
//{
static int sn;

void Stop(int n) {
    sn = n;
    while(1) {}
}
//}

/**
 * @brief Semiperiod values for LED1 and LED2
 */
//{
#define DELAY0 1500
#define DELAY1 3500
#define DELAY2 2000

//}


/**
 * Parameters for Task TaskStart
 */
//{
#define TASKSTART_STACKSIZE 100
static CPU_STK TaskStart_Stack[TASKSTART_STACKSIZE];
static OS_TCB  TaskStart_TCB;
//}

/**
 * Parameters for Task Task 0: Blink LED1
 */
//{
#define TASK0_STACKSIZE 100
static CPU_STK Task0_Stack[TASK0_STACKSIZE];
static OS_TCB  Task0_TCB;
//}

/**
 * Parameters for Task Task 1: Blink LED2
 */
//{
#define TASK1_STACKSIZE 100
static CPU_STK Task1_Stack[TASK1_STACKSIZE];
static OS_TCB  Task1_TCB;
//}

/**
 * Parameters for Task Task 2: Send a char
 */
//{
#define TASK2_STACKSIZE 100
static CPU_STK Task2_Stack[TASK2_STACKSIZE];
static OS_TCB  Task2_TCB;

/**
 * @brief  Task for blinking LED1
 *
 * @note   LED_Init must be called before
 */

void Task0(void *param) {
OS_ERR err;

    while(1) {
        LED_Toggle(LED1);
        OSTimeDly(DELAY0,OS_OPT_TIME_DLY,&err);
        if( err != OS_ERR_NONE )
            Stop(4);
    }
}

/**
 * @brief  Task for blinking LED2
 *
 * @note   LED_Init must be called before
 */

void Task1(void *param) {
OS_ERR err;

    while(1) {
        LED_Toggle(LED2);
        OSTimeDly(DELAY1,OS_OPT_TIME_DLY,&err);
        if( err != OS_ERR_NONE )
            Stop(5);
    }
}

/**
 * @brief  Task for sending a char
 *
 * @note   UART_Init must be called before
 */

void Task2(void *param) {
OS_ERR err;

    while(1) {
        UART_SendChar('*');
        OSTimeDly(DELAY1,OS_OPT_TIME_DLY,&err);
        if( err != OS_ERR_NONE )
            Stop(5);
    }
}

/**
 * @brief  Task for starting other tasks
 *
 * @note   It is recommended to create task when uc/os is already running
 */


void TaskStart(void *param) {
OS_ERR err;

    param = param;                                      // To avoid warning

    CPU_Init();

    // Set clock source to external crystal: 48 MHz
    (void) SystemCoreClockSet(CLOCK_HFXO,1,1);

    OSTaskCreate(   (OS_TCB *)      &Task0_TCB,
                    (CPU_CHAR *)    "Task 0",
                    (OS_TASK_PTR )  Task0,
                    (void *)        0,
                    (OS_PRIO )      TASK0_PRIO,
                    (CPU_STK *)     &Task0_Stack[0],
                    (CPU_STK_SIZE)  0,
                    (CPU_STK_SIZE)  TASK0_STACKSIZE,
                    (OS_MSG_QTY )   0,
                    (OS_TICK )      0,
                    (void *)        0,
                    (OS_OPT)        (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR *)      &err
                );

    OSTaskCreate(   (OS_TCB *)      &Task1_TCB,
                    (CPU_CHAR *)    "Task 1",
                    (OS_TASK_PTR )  Task1,
                    (void *)        0,
                    (OS_PRIO )      TASK1_PRIO,
                    (CPU_STK *)     &Task1_Stack[0],
                    (CPU_STK_SIZE)  0,
                    (CPU_STK_SIZE)  TASK1_STACKSIZE,
                    (OS_MSG_QTY )   0,
                    (OS_TICK )      0,
                    (void *)        0,
                    (OS_OPT)        (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR *)      &err
                );


    OSTaskCreate(   (OS_TCB *)      &Task2_TCB,
                    (CPU_CHAR *)    "Task 2",
                    (OS_TASK_PTR )  Task2,
                    (void *)        0,
                    (OS_PRIO )      TASK2_PRIO,
                    (CPU_STK *)     &Task2_Stack[0],
                    (CPU_STK_SIZE)  0,
                    (CPU_STK_SIZE)  TASK2_STACKSIZE,
                    (OS_MSG_QTY )   0,
                    (OS_TICK )      0,
                    (void *)        0,
                    (OS_OPT)        (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR *)      &err
                );


    // Configure SysTick
    SysTick_Config(SystemCoreClock/1000);

    // Turn on LEDs
    LED_Write(0,LED1|LED2);         // Turn them on
    
    while (1) {
        OSTimeDlyHMSM(  (CPU_INT16U)    0,
                        (CPU_INT16U)    0,
                        (CPU_INT16U)    0,
                        (CPU_INT32U)    100,
                        (OS_OPT )       OS_OPT_TIME_HMSM_STRICT,
                        (OS_ERR *)      &err
                    );
        if( err != OS_ERR_NONE )
            Stop(1);
//        LED_Toggle(LED1|LED2);
    }
}



/**
 * @brief  Main function
 *
 * @note   Using external crystal oscillator
 *         HFCLK = HFXO, HFCORECLK = HFCLK, HFPERCLK  = HFCLK
 */
int main(void) {
OS_ERR err;

    
    // Initialize uC/OS III
    OSInit(&err);
    if( err != OS_ERR_NONE )
        Stop(1);
    
    /* Configure LEDs */
    LED_Init(LED1|LED2);

    /* Configure UART */
    UART_Init();

    // Create a task to start the other tasks
    OSTaskCreate(   (OS_TCB   *)        &TaskStart_TCB,             // TCB
                    (CPU_CHAR *)        "App Task Start",           // Name
                    (OS_TASK_PTR )      TaskStart,                  // Function
                    (void*)             0,                          // Parameter
                    (OS_PRIO)           TASKSTART_PRIO,             // Priority (in app_cfg.h)
                    (CPU_STK *)         &TaskStart_Stack[0],         // Stack
                    (CPU_STK_SIZE)      TASKSTART_STACKSIZE/10,     // Slack
                    (CPU_STK_SIZE)      TASKSTART_STACKSIZE,        // Size
                    (OS_MSG_QTY )       0,
                    (OS_TICK )          0,
                    (void *)            0,
                    (OS_OPT )           (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                    (OS_ERR *)          &err
                );
    if( err != OS_ERR_NONE )
        Stop(2);
        
    __enable_irq();

    // Enter uc/os and never returns
    OSStart(&err);
    if( err != OS_ERR_NONE )
        Stop(3);
    

}
