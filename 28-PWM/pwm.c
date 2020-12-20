
/**
 * @file    pwm.c
 * @brief   PWM Generator for EFM32GG
 * @version 1.0
 *
 * @note    Can use any timer
 *
 * @note    The HFPERCLOCK is used as clock source
 *
 * @note    Pinout and Ports
 *
 *  Timer  | Channel |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
 *   ------|---------|----|----|----|----|----|----|----|----|
 * TIMER0  | CC0     |PA0 |PA0 |PF6 |PD1 |PA0 |PF0 |    |    |
 * TIMER0  | CC1     |PA1 |PA1 |PF7 |PD2 |PC0 |PF1 |    |    |
 * TIMER0  | CC2     |PA2 |PA2 |PF8 |PD3 |PC1 |PF2 |    |    |
 * TIMER1  | CC0     |    |PE10|PB0 |PB7 |PD6 |    |    |    |
 * TIMER1  | CC1     |    |PE11|PB1 |PB8 |PD7 |    |    |    |
 * TIMER1  | CC2     |    |PE12|PB2 |PB11|    |    |    |    |
 * TIMER2  | CC0     |PA8 |PA12|PC8 |    |    |    |    |    |
 * TIMER2  | CC1     |PA9 |PA13|PC9 |    |    |    |    |    |
 * TIMER2  | CC2     |PA10|PA14|PC10|    |    |    |    |    |
 * TIMER3  | CC0     |PE14|PE0 |    |    |    |    |    |    |
 * TIMER3  | CC1     |PE15|PE1 |    |    |    |    |    |    |
 * TIMER3  | CC2     |PA15|PE2 |    |    |    |    |    |    |
 *
 *
 */


#include <stdint.h>
#include "em_device.h"
#include "gpio.h"
#include "pwm.h"

#define CONCAT(A,B)  A##B
#define STRINGFY(A)  #A

#define GPIOPIN(GPIONUMBER,NUMBER)  ((GPIONUMBER<<8)|(NUMBER))
#define PA(NUMBER)                  GPIOPIN(0,NUMBER)
#define PB(NUMBER)                  GPIOPIN(1,NUMBER)
#define PC(NUMBER)                  GPIOPIN(2,NUMBER)
#define PD(NUMBER)                  GPIOPIN(3,NUMBER)
#define PE(NUMBER)                  GPIOPIN(4,NUMBER)
#define PF(NUMBER)                  GPIOPIN(5,NUMBER)
#define PG(NUMBER)                  GPIOPIN(6,NUMBER)
#define PH(NUMBER)                  GPIOPIN(7,NUMBER)
#define PI(NUMBER)                  GPIOPIN(8,NUMBER)
#define PJ(NUMBER)                  GPIOPIN(9,NUMBER)

#define TC(T,N)                     ((T<<5)|(N))
/* Structure to store info about pin configuration  */
#define LGP(L,GP)                   (((L)<<8)|(GP))

#if 0
typedef struct {
    int16_t     timer_channel;
    uint16_t    loc_gpio_pin;
} TimerPin;

static const TimerPin timerpins[] = {
    {   TC(0,0),  LGP(0,PA(0))  },
    {   TC(0,0),  LGP(1,PA(0))  },
    {   TC(0,0),  LGP(2,PF(6))  },
    {   TC(0,0),  LGP(3,PD(1))  },
    {   TC(0,0),  LGP(4,PA(0))  },
    {   TC(0,0),  LGP(5,PF(0))  },

    {   TC(0,1),  LGP(0,PA(1))  },
    {   TC(0,1),  LGP(1,PA(1))  },
    {   TC(0,1),  LGP(2,PF(7))  },
    {   TC(0,1),  LGP(3,PD(2))  },
    {   TC(0,1),  LGP(4,PC(0))  },
    {   TC(0,1),  LGP(5,PF(1))  },

    {   TC(0,2),  LGP(0,PA(2))  },
    {   TC(0,2),  LGP(1,PA(2))  },
    {   TC(0,2),  LGP(2,PF(8))  },
    {   TC(0,2),  LGP(3,PD(3))  },
    {   TC(0,2),  LGP(4,PC(1))  },
    {   TC(0,2),  LGP(5,PF(2))  },

    {   TC(1,0),  LGP(1,PE(10)) },
    {   TC(1,0),  LGP(2,PB(0))  },
    {   TC(1,0),  LGP(3,PB(7))  },
    {   TC(1,0),  LGP(4,PD(6))  },

    {   TC(1,1),  LGP(1,PE(11)) },
    {   TC(1,1),  LGP(2,PB(1))  },
    {   TC(1,1),  LGP(3,PB(8))  },
    {   TC(1,1),  LGP(4,PD(7)) },

    {   TC(1,2),  LGP(1,PE(12)) },
    {   TC(1,2),  LGP(2,PB(2))  },
    {   TC(1,2),  LGP(3,PB(11)) },

    {   TC(2,0),  LGP(0,PA(8))  },
    {   TC(2,0),  LGP(1,PA(12)) },
    {   TC(2,0),  LGP(2,PC(8))  },

    {   TC(2,1),  LGP(0,PA(9))  },
    {   TC(2,1),  LGP(1,PA(13)) },
    {   TC(2,1),  LGP(2,PC(9))  },

    {   TC(2,2),  LGP(0,PA(10)) },
    {   TC(2,2),  LGP(1,PA(14)) },
    {   TC(2,2),  LGP(2,PC(10)) },

    {   TC(3,0),  LGP(0,PE(14)) },
    {   TC(3,0),  LGP(1,PE(0))  },

    {   TC(3,1),  LGP(0,PE(15)) },
    {   TC(3,1),  LGP(1,PE(1))  },

    {   TC(3,2),  LGP(0,PA(15)) },
    {   TC(3,2),  LGP(1,PE(2))  },
    {        -1,             0  }
};

#endif

/**
 * @brief   PWM_Start
 *
 */
void PWM_Start(TIMER_TypeDef* timer) {

    timer->CMD = TIMER_CMD_START;

}

/**
 * @brief   PWM_Stop
 *
 */
void PWM_Stop(TIMER_TypeDef* timer) {

    timer->CMD = TIMER_CMD_STOP;

}

static int Timer_ClockEnable(TIMER_TypeDef* timer) {

    if( timer == TIMER0 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0;
    } else if ( timer == TIMER1 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER1;
    } else if ( timer == TIMER2 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER2;
    } else if ( timer == TIMER3 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER3;
    } else {
        return -1;
    }

    return 0;
}


static int PWM_ConfigureOutputPin(TIMER_TypeDef* timer, unsigned channel, int loc) {

    if( loc == PWM_LOC_UNUSED )
        return 0;


    timer->ROUTE = (timer->ROUTE
                        &~(_TIMER_ROUTE_LOCATION_MASK)
                          )
                    | (loc<<_TIMER_ROUTE_LOCATION_SHIFT)
                    | (1<<(_TIMER_ROUTE_CC0PEN_SHIFT+channel));

    return 0;
}

/**
 * @brief   Initializes timer using default parameters
 *
 * @note    It configure by default only channel 0
 */


int PWM_Init(TIMER_TypeDef* timer, int loc0, int loc1, int loc2) {
int rc;

    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    /* Enable clock for timer */
    rc = Timer_ClockEnable(timer);
    if( rc < 0 ) return -1;

    /* Configure output pins */
    PWM_ConfigureOutputPin(timer, 0, loc0);
    PWM_ConfigureOutputPin(timer, 1, loc1);
    PWM_ConfigureOutputPin(timer, 2, loc2);

    /* Configure timer */
    rc = PWM_Config(timer,1,0xFFFF, 0);
    if( rc < 0 ) return -2;

    if( loc0 != PWM_LOC_UNUSED ) {
        timer->CC[0].CTRL =  TIMER_CC_CTRL_ICEVCTRL_RISING
                            |TIMER_CC_CTRL_ICEDGE_RISING
                            |TIMER_CC_CTRL_CMOA_CLEAR
                            |TIMER_CC_CTRL_COIST
                            |TIMER_CC_CTRL_MODE_PWM;
    }
    if( loc1 != PWM_LOC_UNUSED ) {
        timer->CC[1].CTRL =  TIMER_CC_CTRL_ICEVCTRL_RISING
                            |TIMER_CC_CTRL_ICEDGE_RISING
                            |TIMER_CC_CTRL_CMOA_CLEAR
                            |TIMER_CC_CTRL_COIST
                            |TIMER_CC_CTRL_MODE_PWM;
    }
    if( loc2 != PWM_LOC_UNUSED ) {
        timer->CC[2].CTRL =  TIMER_CC_CTRL_ICEVCTRL_RISING
                            |TIMER_CC_CTRL_ICEDGE_RISING
                            |TIMER_CC_CTRL_CMOA_CLEAR
                            |TIMER_CC_CTRL_COIST
                            |TIMER_CC_CTRL_MODE_PWM;
    }

    timer->IEN = 0;

    PWM_Start(timer);

    return 0;
}

/**
 * @brief   Returns status of all channels as a bit vector in a unsigned int
 *
 */

unsigned PWM_Read(TIMER_TypeDef* timer, unsigned channel) {
unsigned m;

    m = timer->CC[channel].CCV;

    return m;
}


/**
 * @brief   Find prescaler encoding
 *
 *      |   div    | coding  |
 *      |----------|- -------|
 *      |     1    |     0   |
 *      |     2    |     1   |
 *      |     4    |     2   |
 *      |     5    |     3   |
 *      | ....     | .....   |
 *      |  1024    |  10     |
 *
 * @note    It is ln2(div)
 *
 */

static unsigned finddivencoding(unsigned v) {
unsigned m = 1;
unsigned c = 0;

    while( m < v ) {
        m <<= 1;
        c++;
    }

    return c;
}
/**
 * @brief   Returns status of all channels as a bit vector in a unsigned int
 */

int PWM_Config(TIMER_TypeDef *timer, unsigned div, unsigned top, int pol) {
unsigned d;

    PWM_Stop(timer);

    d = finddivencoding(div);

    timer->CTRL = (timer->CTRL
                      &~(_TIMER_CTRL_PRESC_MASK
                        |_TIMER_CTRL_MODE_MASK))
                  |(d<<_TIMER_CTRL_PRESC_SHIFT)
                  |(TIMER_CTRL_MODE_UP);

    timer->TOP = top;

    if( pol ) {
        timer->CC[0].CTRL |= TIMER_CC_CTRL_OUTINV;
        timer->CC[1].CTRL |= TIMER_CC_CTRL_OUTINV;
        timer->CC[2].CTRL |= TIMER_CC_CTRL_OUTINV;
    } else {
        timer->CC[0].CTRL &= ~TIMER_CC_CTRL_OUTINV;
        timer->CC[1].CTRL &= ~TIMER_CC_CTRL_OUTINV;
        timer->CC[2].CTRL &= ~TIMER_CC_CTRL_OUTINV;
    }

    PWM_Start(timer);

    return 0;

}

/**
 * @brief   PWM_Write
 *
 * @note    Uses buffered write to CCV
 */

int PWM_Write(TIMER_TypeDef *timer, unsigned channel, unsigned value) {

    timer->CC[channel].CCVB = value;          // Write to buffer to avoid glitch

    return 0;

}

