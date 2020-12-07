#ifndef TTE_H
#define TTE_H
/**
 * @file     tte.h
 * @brief    Time Triggered Kernel
 * @version  V1.0
 * @date     23/01/2016
 *
 * @note     Direct access to registers
 * @note     No library except CMSIS is used
 *
 *
 ******************************************************************************/

/**
 * @brief TTE API
 *
 */
//@{
uint32_t Task_Init(void);
uint32_t Task_Add(void (*task)(void), uint32_t period, uint32_t delay);
uint32_t Task_Delete(uint32_t i); // Do not use
uint32_t Task_Dispatch(void);
void     Task_Update(void);
//@}
#endif
