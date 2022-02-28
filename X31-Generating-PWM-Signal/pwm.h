#ifndef PWM_H
#define PWM_H
/**
 * @file    pwm.h
 */


/**
 * @brief   location options
 * 
 * @note    one for each timer
 */

#define PWM_LOC0                              0
#define PWM_LOC1                              1
#define PWM_LOC2                              2
#define PWM_LOC3                              3
#define PWM_LOC4                              4
#define PWM_LOC5                              5

/**
 *' @brief  ' configuration parameters
 */
///{
#define PWM_PARAMS_ENABLE                     0x001
#define PWM_PARAMS_ENABLEPIN                  0x002
#define PWM_PARAMS_ENABLEINTERRUPT            0x004
#define PWM_PARAMS_ENABLEINVERTPOL            0x008

#define PWM_PARAMS_CH0_ENABLE                 ((PWM_PARAMS_ENABLE         )<<0)
#define PWM_PARAMS_CH0_ENABLEPIN              ((PWM_PARAMS_ENABLEPIN      )<<0)
#define PWM_PARAMS_CH0_ENABLEINTERRUPT        ((PWM_PARAMS_ENABLEINTERRUPT)<<0)
#define PWM_PARAMS_CH0_ENABLEINVERTPOL        ((PWM_PARAMS_ENABLEINVERTPOL)<<0)

#define PWM_PARAMS_CH1_ENABLE                 ((PWM_PARAMS_ENABLE         )<<4)
#define PWM_PARAMS_CH1_ENABLEPIN              ((PWM_PARAMS_ENABLEPIN      )<<4)
#define PWM_PARAMS_CH1_ENABLEINTERRUPT        ((PWM_PARAMS_ENABLEINTERRUPT)<<4)
#define PWM_PARAMS_CH1_ENABLEINVERTPOL        ((PWM_PARAMS_ENABLEINVERTPOL)<<4)

#define PWM_PARAMS_CH2_ENABLE                 ((PWM_PARAMS_ENABLE         )<<8)
#define PWM_PARAMS_CH2_ENABLEPIN              ((PWM_PARAMS_ENABLEPIN      )<<8)
#define PWM_PARAMS_CH2_ENABLEINTERRUPT        ((PWM_PARAMS_ENABLEINTERRUPT)<<8)
#define PWM_PARAMS_CH2_ENABLEINVERTPOL        ((PWM_PARAMS_ENABLEINVERTPOL)<<8)

///}



int  PWM_Init(TIMER_TypeDef* timer, int loc, unsigned params);
int  PWM_ConfigChannel(TIMER_TypeDef* timer, int channel, unsigned params );
int  PWM_ConfigTimer(TIMER_TypeDef *timer, unsigned div, unsigned top);

void PWM_Start(TIMER_TypeDef* timer);
void PWM_Stop(TIMER_TypeDef* timer);
int  PWM_Read(TIMER_TypeDef* timer, unsigned channel);
int  PWM_Write(TIMER_TypeDef *timer, unsigned channel, unsigned value);

int  PWM_ReconfigureChannel(TIMER_TypeDef* timer, int channel, unsigned top);

#endif  // PWM_H

