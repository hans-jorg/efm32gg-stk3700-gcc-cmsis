#ifndef TOUCH_H
#define TOUCH_H
/**
 * @file    touch.h
 */

int         Touch_Init(void);
unsigned    Touch_Read(void);
unsigned    Touch_ReadChannel(int ch);
int         Touch_GetCenterOfTouch(unsigned v);
unsigned    Touch_Interpolate(unsigned v);

/* Must be called periodically */
void        Touch_PeriodicProcess(void);

#define TOUCH_N 4

#endif  // TOUCH_H

