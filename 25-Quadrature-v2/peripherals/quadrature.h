#ifndef QUADRATURE_H
/** ***************************************************************************
 * @file    quadrature.h
 * @brief   HAL for a quadrature encoder
 * @version 1.0
******************************************************************************/
#define QUADRATURE_H
#include <stdint.h>

void    Quadrature_Init(void);
int     Quadrature_GetPosition(void);
int     Quadrature_GetButtonStatus(void);

#endif // LCD_H
