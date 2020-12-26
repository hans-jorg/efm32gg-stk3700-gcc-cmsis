
/**
 *
 * @brief Time Triggered Executive
 *
 *
 * @note Based on the one found in Engineering Reliable Embedded System.
 *
 */

#include <stdint.h>
#include "tt_tasks.h"

//#include <stm32l476xx.h>
/**
 * @brief To avoid inclusion of CPU specific files
 *
 * From CMSIS Core_cmFunc.h
 */
///@{
__attribute__((always_inline))
static inline void __enable_irq(void)
{
  __asm volatile ("cpsie i" : : : "memory");
}
__attribute__((always_inline))
static inline void __disable_irq(void)
{
  __asm volatile ("cpsid i" : : : "memory");
}
///@}

/// task tick counter
static volatile uint32_t task_tickcounter = 0;

/**
 * @brief Task Info
 * @note  Time unit is ticks
 */

typedef struct {
    void    (*task)(void);  // pointer to function
    uint32_t    period;     // period, i.e. time between activations
    uint32_t    delay;      // time to next activation
} TaskInfo;


/// Default value for Task Info Table
#ifndef TASK_MAXCNT
#define TASK_MAXCNT 10
#endif

// Forward definition of Task_Delete
uint32_t Task_Delete(uint32_t );

/// Task info table
static TaskInfo taskinfo[TASK_MAXCNT];

/**
 * @brief Task Init
 * @note  Initialization of TTE Kernel
 */
uint32_t
Task_Init(void) {
int i;
    for(i=0;i<TASK_MAXCNT;i++) {
        Task_Delete(i);
    }
    return 0;
}

/**
 * @brief Task Add
 *
 * @note Add task to Kernel
 * @note delay can be used to serialize task activation and avoid clustering
 *       in a certain time
 */
uint32_t
Task_Add( void (*task)(void), uint32_t period, uint32_t delay ) {
int taskno = 0;

    while( (taskno<TASK_MAXCNT) && taskinfo[taskno].task ) taskno++;
    if( taskno == TASK_MAXCNT ) return 1;

    taskinfo[taskno].task   = task;
    taskinfo[taskno].period = period;

    return taskno;
}

/**
 * @brief Task Delete
 *
 * @note Remove task from Kernel
 * @note Use with caution because it modifies the scheduling calculation
 */
uint32_t
Task_Delete(uint32_t taskno) {

    taskinfo[taskno].task   = 0;
    taskinfo[taskno].period = 0;
    return 0;
}

/**
 * @brief Task Dispatch
 *
 * @note Run tasks if they are ready
 * @note Must be called in main loop
 */
uint32_t
Task_Dispatch(void) {
int i;
TaskInfo *p;
uint32_t dispatch;

    __disable_irq();
    if( task_tickcounter > 0 ) {
        task_tickcounter--;
        dispatch = 1;
    }
    __enable_irq();


    while( dispatch ) {
        for(i=0;i<TASK_MAXCNT;i++) {
            p = &taskinfo[i];
            if( p->task ) {
                if( p->delay == 0 ) {
                    p->task();      // call task function
                    if( p->period == 0 ) { // one time tasks are dangerous
                        Task_Delete(i);
                    } else {
                        p->delay = p->period;
                    }
                } else {
                    p->delay--;
                }
            }

        }
        __disable_irq();
        if( task_tickcounter > 0 ) {
            task_tickcounter--;
            dispatch = 1;
        } else {
            dispatch = 0;
        }
        __enable_irq();

    }
    return 0;
}

/**
 * @brief Task Update
 *
 * @note Must be called only in Timer Interrupt Routine
 * @note Almost nothing happens here
 *
 */

void
Task_Update(void) {

    task_tickcounter++;

    return;
}


/**
 * @brief Task Modify Period
 *
 * @note CAUTION!!! It can modify all timing calculation
 *
 */
uint32_t
Task_ModifyPeriod(uint32_t taskno, uint32_t newperiod) {
uint32_t t = taskinfo[taskno].period;
    taskinfo[taskno].period = newperiod;
    return t;
}
