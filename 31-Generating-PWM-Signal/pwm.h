#ifndef PWM_H
#define PWM_H
/**
 * @file    pwm.h
 */

#define PWM_LOC0                             0
#define PWM_LOC1                             1
#define PWM_LOC2                             2
#define PWM_LOC3                             3
#define PWM_LOC4                             4
#define PWM_LOC5                             5
#define PWM_LOC_UNUSED                      -1

/**
 * @brief   configuration parameters
 */
///{
#define PWM_PARAMS_ENABLECHANNEL0            1
#define PWM_PARAMS_ENABLECHANNEL1            2
#define PWM_PARAMS_ENABLECHANNEL2            4
#define PWM_PARAMS_ENABLEINTERRUPT0          8
#define PWM_PARAMS_ENABLEINTERRUPT1         16
#define PWM_PARAMS_ENABLEINTERRUPT2         32
#define PWM_PARAMS_ENABLEPIN0               64
#define PWM_PARAMS_ENABLEPIN1              128
#define PWM_PARAMS_ENABLEPIN2              256
#define PWM_PARAMS_ENABLEINVERTPOL0        512
#define PWM_PARAMS_ENABLEINVERTPOL1       1024
#define PWM_PARAMS_ENABLEINVERTPOL2       2048
///}



int         PWM_Init(TIMER_TypeDef* timer, int location, unsigned params);
int         PWM_Config(TIMER_TypeDef *timer, unsigned div, unsigned top);

int         PWM_Write(TIMER_TypeDef *timer, unsigned channel, unsigned value);
unsigned    PWM_Read(TIMER_TypeDef *timer, unsigned channel);
void        PWM_Start(TIMER_TypeDef *timer);
void        PWM_Stop(TIMER_TypeDef *timer);
#endif  // PWM_H

