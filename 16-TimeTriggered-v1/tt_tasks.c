/**
 * @file        tt_tasks.c
 * @brief       implements a cooperative multitasking kernel following
 *              Pont, Patterns for time-triggered embedded systems. 2014.
 *              Specially, Chapter 14.
 *
 * @brief       Time unit is ticks (periodic timer interrupt)
 *              Task_Update must be called by timer interrupt processing
 *              All tasks must complete in less than tick seconds
 *
 * @bug         There is a hazard between tasks[i].run++ in Task_Update (interrupt)
 *              and tasks[i].run-- in Task_Dispatch
 *
 * @see         https://www.safetty.net/products/publications/pttes
 * @see         http://www.riosscheduler.org/
 *              https://www.embedded.com/electronics-news/4434501/Writing-a-simple-cooperative-scheduler
 *
 * @author      Hans Schneebeli
 * @date        20/10/2017
 */


#include "tt_tasks.h"

/// Maximum number of tasks
#define TASK_N  (10)


/// To allow easy redefining
#define INT int

/**
 * @typedef Task_t
 * @brief   Information about a task
 */
typedef struct {
    void    (*function)(void);          ///< pointer to function with task code
    INT     counter;                    ///< counter (in ticks). when 0, task should be run
    INT     period;                     ///< period (in ticks). when 0, task in run once
    INT     run;                        ///< run counter. when above 1, task is delayed
} Task_t;

/**
 *  @var    tasks
 *  @brief  Task table
 */
static Task_t  tasks[TASK_N];

/**
 * @details Uses Task_Delete
 */

void Task_Init(void) {
int i;

    for(i=0; i<TASK_N; i++ ) {
        Task_Delete(i);
    }

}

/**
 * @details No check is done in index parameter (Must be less than TASK_N)
 */
void Task_Delete(TASKS_INT index) {

    tasks[index].function = 0;
    tasks[index].counter  = 0;
    tasks[index].period   = 0;
    tasks[index].run      = 0;

}

/**
 * @details No check is done on input parameters
 */

int Task_Add(  void (*f)(void), TASKS_INT p, TASKS_INT d) {
int i = 0;

    while( (i<TASK_N) && (tasks[i].function) ) i++;

    if( i == TASK_N )
        // No more space for this task
        return -1;

    tasks[i].function = f;
    tasks[i].period   = p;
    tasks[i].counter  = d;
    tasks[i].run      = 0;

    return i;
}

/**
 * @details none
 */

void Task_Update(void) {
int i;

    for( i=0; i<TASK_N; i++ ) {

        if ( tasks[i].function ) {
            if( tasks[i].counter == 0 ) {
                tasks[i].run++;             // DANGER: hazard
                tasks[i].counter = tasks[i].period;
            } else {
                tasks[i].counter--;
            }
        }
    }

}

/**
 * @details updates counters e set function variable
 */

void Task_Dispatch(void) {
int i;

    for( i=0; i<TASK_N; i++ ) {
        if( tasks[i].run ) {
            tasks[i].run--;         // Read/Modify/Write  = Hazard run++ in Task_Update
            tasks[i].function();
            if( tasks[i].period == 0 ) {// Run once tasks
                Task_Delete(i);
            }
        }
    }

}
