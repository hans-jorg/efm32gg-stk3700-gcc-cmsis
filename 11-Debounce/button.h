/** ***************************************************************************
 * @file    button.h
 * @brief   Button HAL for EFM32GG STK3200
 * @version 1.0
******************************************************************************/
#ifndef BUTTON_H
#define BUTTON_H
#include <stdint.h>

//#ifndef BIT
#define BIT(N) (1U<<(N))
//#endif

/**
 * @brief  Buttons are on PB
 */
///@{
#define BUTTON1 BIT(9)
#define BUTTON2 BIT(10)
///@}

void        Button_Init(uint32_t buttons);
uint32_t    Button_Read(void);
uint32_t    Button_ReadChanges(void);
uint32_t    Button_ReadPressed(void);
uint32_t    Button_ReadReleased(void);
void        Button_SetCallback( void (*proc)(uint32_t parm) );
void        Button_Processing(void); // Must be called every X ms
#endif // BUTTON_H
