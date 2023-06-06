#ifndef TT_TASKS_H
#define TT_TASKS_H

#define TASKS_INT int
/**
 * @file        tt_tasks.h
 * @brief       implements a cooperative multitasking kernel following
 *              Pont, Patterns for time-triggered embedded systems. 2014.
 *              Specially, Chapter 14.
 *
 * @see         https://www.safetty.net/products/publications/pttes
 *
 * @author      Hans Schneebeli
 * @date        20/10/2017
 */


/**
 * @fn      Task_Init
 * @brief   Initializes the tasks table.
 *          Can be omitted when BSS section is initialized to zeroes
 *
 * @param   void
 * @return  void
 */
void Task_Init(void);

/**
 * @fn      Task_Delete
 * @brief   Removes a task from tasks table
 *
 * @param   index of task to be deleted
 * @return  void
 */
void Task_Delete(TASKS_INT index);

/**
 * @fn      Task_Add
 * @brief   Add a task to tasks table
 *
 * @param   f          pointer to task function
 * @param   p          period (in ticks)
 * @param   d          initial delay (in ticks)
 * @return  index of task in tasks table. -1 if there is no more space for this task
 */
int Task_Add(void (*f)(void), TASKS_INT p, TASKS_INT d);

/**
 * @fn      Task_Update
 * @brief   Update tasks table enabling execution of tasks in Task_Dispatch
 *
 *          Must be run inside the timer interrupt routine
 *
 * @param   void
 * @return  void
 */
void Task_Update( void );

/**
 * @fn      Task_Dispatch
 * @brief   Run the tasks enabled by Task_Update
 *
 *
 * @param   void
 * @return  void
 */
void Task_Dispatch(void);

#endif // TT_TASKS_H
