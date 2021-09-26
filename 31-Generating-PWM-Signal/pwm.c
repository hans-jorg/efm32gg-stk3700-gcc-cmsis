
/**
 * @file    pwm.c
 * @brief   PWM Generator for EFM32GG
 * @version 1.0
 *
 * @note    Can use any timer
 *
 * @note    The HFPERCLOCK is used as clock source
 *
 * @note    Pinout and Ports for EFM32GG990
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


/**
 * @brief Output mode for timer output
 *
 * @note  Alternatives are Push-Pull (4), Push-Pull with strength (5)
 *
 * @note  Maybe the Wired* modes could be used!
 */
#define PINMODE    4


/**
 * @brief   Standard C Macro Utilities
 */
#define CONCAT(A,B)  A##B
#define STRINGFY(A)  #A

/**
 * @brief Configuration table
 *
 * @note Each timer has a LOCATION field, that speficies where the output signal is routed to.
 *
 * @note The pin must be configured as output using the GPIO interface
 *
 * @note The compound value (timer_channel_loc) is the key. The gpio_pin compound is the
 *       content. So it is possible to find out which pin is beeing used by a timer
 */

typedef struct {
    int16_t     timer_channel_loc;
    uint16_t    gpio_pin;
} TimerPin;

/* Macros to build the compounds for timer_channel_loc and gpio_pin */
#define TCL(T,C,L)                  (((T)<<8)|((C)<<3)|((L)<<0))
#define GPIOPIN(GPIONUMBER,NUMBER)  ((GPIONUMBER<<8)|(NUMBER))

/*
 * @brief Macro to easy the initialization
 */

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


/**
 * @brief Pin usage by timers at the EFM32GG990F1024 microcontroller
 *
 */
static const TimerPin timerpins[] = {
    {   TCL(0,0,0),     PA(0)  },
    {   TCL(0,0,1),     PA(0)  },
    {   TCL(0,0,2),     PF(6)  },
    {   TCL(0,0,3),     PD(1)  },
    {   TCL(0,0,4),     PA(0)  },
    {   TCL(0,0,5),     PF(0)  },

    {   TCL(0,1,0),     PA(1)  },
    {   TCL(0,1,1),     PA(1)  },
    {   TCL(0,1,2),     PF(7)  },
    {   TCL(0,1,3),     PD(2)  },
    {   TCL(0,1,4),     PC(0)  },
    {   TCL(0,1,5),     PF(1)  },

    {   TCL(0,2,0),     PA(2)  },
    {   TCL(0,2,1),     PA(2)  },
    {   TCL(0,2,2),     PF(8)  },
    {   TCL(0,2,3),     PD(3)  },
    {   TCL(0,2,4),     PC(1)  },
    {   TCL(0,2,5),     PF(2)  },

    {   TCL(1,0,1),     PE(10) },
    {   TCL(1,0,2),     PB(0)  },
    {   TCL(1,0,3),     PB(7)  },
    {   TCL(1,0,4),     PD(6)  },

    {   TCL(1,1,1),     PE(11) },
    {   TCL(1,1,2),     PB(1)  },
    {   TCL(1,1,3),     PB(8)  },
    {   TCL(1,1,4),     PD(7) },

    {   TCL(1,2,1),     PE(12) },
    {   TCL(1,2,2),     PB(2)  },
    {   TCL(1,2,3),     PB(11) },

    {   TCL(2,0,0),     PA(8)  },
    {   TCL(2,0,1),     PA(12) },
    {   TCL(2,0,2),     PC(8)  },

    {   TCL(2,1,0),     PA(9)  },
    {   TCL(2,1,1),     PA(13) },
    {   TCL(2,1,2),     PC(9)  },

    {   TCL(2,2,0),     PA(10) },
    {   TCL(2,2,1),     PA(14) },
    {   TCL(2,2,2),     PC(10) },

    {   TCL(3,0,0),     PE(14) },
    {   TCL(3,0,1),     PE(0)  },

    {   TCL(3,1,0),     PE(15) },
    {   TCL(3,1,1),     PE(1)  },

    {   TCL(3,2,0),     PA(15) },
    {   TCL(3,2,1),     PE(2)  },
    {         -1,      0      }
};

/**
 * @brief   PWM_Start
 *
 * @param   timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 */
void PWM_Start(TIMER_TypeDef* timer) {

    timer->CMD = TIMER_CMD_START;

}

/**
 * @brief   PWM_Stop
 *
 * @param   timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 */
void PWM_Stop(TIMER_TypeDef* timer) {

    timer->CMD = TIMER_CMD_STOP;

}

/**
 * @brief   Timer_ClockEnable
 *
 * @note    Local function to configure clock for a specific timer.
 *
 * @note    It ENABLES the HFPER Clock
 *
 * @param   timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 */
static int Timer_ClockEnable(TIMER_TypeDef* timer) {

    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
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

/**
 * @brief    PWM_ConfigureOutputPin
 *
 * @note     local function to configure the pin associdated to a channel to be used as output
 *
 * @param    timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 *
 * @param    channel  Channel number (0,1,2)
 *
 * @location location used for pins as defined in Table 4.2. Alternate functionality overview of
 *           datasheet
 */

static int PWM_ConfigureOutputPin(TIMER_TypeDef* timer, unsigned channel, unsigned location ) {
unsigned t;
int16_t key;
TimerPin const *ptimerpin;
GPIO_P_TypeDef *gpio;
unsigned gpion,pinn;

    /* Finds the index of a timer */
    if( timer == TIMER0 ) {
        t = 0;
    } else if ( timer == TIMER1 ) {
        t = 1;
    } else if ( timer == TIMER2 ) {
        t = 2;
    } else if ( timer == TIMER3 ) {
        t = 3;
    } else {
        return -1;
    }

    // Configure GPIO pin
    key = TCL(t,channel,location);

    // It uses a linear search!!!
    ptimerpin = timerpins;
    while( (ptimerpin->timer_channel_loc >= 0) && (ptimerpin->timer_channel_loc == key) ) {
        ptimerpin++;
    }
    if( ptimerpin->timer_channel_loc != key )
        return -2;

    gpion = ptimerpin->gpio_pin>>8;
    pinn = ptimerpin->gpio_pin&0xFF;

    gpio = &(GPIO->P[gpion]);

    int ppos;

    // Configure pin as output, push-pull, fast
    if( pinn < 8 ) {       // pins 7-0
        ppos = pinn*4;
        gpio->MODEL = (gpio->MODEL&~(0xF<<ppos))|(PINMODE<<ppos);
    } else {            // pins 15-8
        pinn -= 8;
        ppos = pinn*4;
        gpio->MODEH = (gpio->MODEH&~(0xF<<ppos))|(PINMODE<<ppos);
    }

    return 0;
}

/**
 * @brief    PWM_Init
 *
 * @note     Initialize timer

 * @param    timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 *
 * @location location used for pins as defined in Table 4.2. Alternate functionality overview of
 *           datasheet
 *
 * @param    params  OR of the following values
 *
 *           | Params                          |
 *           |---------------------------------|
 *           | PWM_PARAMS_ENABLECHANNEL0       |
 *           | PWM_PARAMS_ENABLECHANNEL1       |
 *           | PWM_PARAMS_ENABLECHANNEL2       |
 *           | PWM_PARAMS_ENABLEINTERRUPT0     |
 *           | PWM_PARAMS_ENABLEINTERRUPT1     |
 *           | PWM_PARAMS_ENABLEINTERRUPT2     |
 *           | PWM_PARAMS_ENABLEPIN0           |
 *           | PWM_PARAMS_ENABLEPIN1           |
 *           | PWM_PARAMS_ENABLEPIN2           |
 *           | PWM_PARAMS_ENABLEINVERTPOL0     |
 *           | PWM_PARAMS_ENABLEINVERTPOL1     |
 *           | PWM_PARAMS_ENABLEINVERTPOL2     |
 *           | PWM_PARAMS_ENABLEINVERTPOL2     |
 *
 * @note   If there is one value for a channel, it is assumed it is enabled
 */

int PWM_Init(TIMER_TypeDef* timer, int location, unsigned params) {
int rc;

    // Assert consistency (If any configuration for a channel exists, it must be enabled)
    if( params&(PWM_PARAMS_ENABLEINTERRUPT0|PWM_PARAMS_ENABLEPIN0|PWM_PARAMS_ENABLEINVERTPOL0) )
        params |= PWM_PARAMS_ENABLECHANNEL0;
    if( params&(PWM_PARAMS_ENABLEINTERRUPT1|PWM_PARAMS_ENABLEPIN1|PWM_PARAMS_ENABLEINVERTPOL1) )
        params |= PWM_PARAMS_ENABLECHANNEL1;
    if( params&(PWM_PARAMS_ENABLEINTERRUPT2|PWM_PARAMS_ENABLEPIN2|PWM_PARAMS_ENABLEINVERTPOL2) )
        params |= PWM_PARAMS_ENABLECHANNEL2;
    if( location == PWM_LOC_UNUSED )
        location = 0;

    /* Set clock for peripherals. If already set, it is a nop */

    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    /* Enable clock for timer */
    rc = Timer_ClockEnable(timer);
    if( rc < 0 ) return -1;

    /* Timer must be stopped */
    PWM_Stop(timer);

    /* Configure timer to default values: div=1, top = max*/
    rc = PWM_Config(timer,1,0xFFFF);
    if( rc < 0 ) return -2;

    /* Configure location, i.e., where pins coulde located */
    timer->ROUTE = (timer->ROUTE&~(_TIMER_ROUTE_LOCATION_MASK))|(location<<_TIMER_ROUTE_LOCATION_SHIFT);

    /* Configure output pins for output */
    if( params&PWM_PARAMS_ENABLEPIN0 ) {
        PWM_ConfigureOutputPin(timer, 0, location);
    }
    if( params&PWM_PARAMS_ENABLEPIN1 ) {
        PWM_ConfigureOutputPin(timer, 1, location);
    }
    if( params&PWM_PARAMS_ENABLEPIN2 ) {
        PWM_ConfigureOutputPin(timer, 2, location);
    }

    /* Reset channel configuration */
    timer->CC[0].CTRL = 0;
    timer->CC[1].CTRL = 0;
    timer->CC[2].CTRL = 0;
    timer->CC[0].CCVB = 0xFFFF;
    timer->CC[1].CCVB = 0xFFFF;
    timer->CC[2].CCVB = 0xFFFF;

    /* Configure polarity of channel output */
    if( params&PWM_PARAMS_ENABLEINVERTPOL0 ) {
        timer->CC[0].CTRL |= TIMER_CC_CTRL_OUTINV;
    }
    if( params&PWM_PARAMS_ENABLEINVERTPOL1 ) {
        timer->CC[1].CTRL |= TIMER_CC_CTRL_OUTINV;
    }
    if( params&PWM_PARAMS_ENABLEINVERTPOL2 ) {
        timer->CC[2].CTRL |= TIMER_CC_CTRL_OUTINV;
    }

    /* Configure channel interrupts */
    timer->IEN &= ~(TIMER_IEN_CC0|TIMER_IEN_CC1|TIMER_IEN_CC2);
    if( params&PWM_PARAMS_ENABLEINTERRUPT0 ) {
        timer->IEN = TIMER_IEN_CC0;
    }
    if( params&PWM_PARAMS_ENABLEINTERRUPT1 ) {
        timer->IEN = TIMER_IEN_CC1;
    }
    if( params&PWM_PARAMS_ENABLEINTERRUPT2 ) {
        timer->IEN = TIMER_IEN_CC2;
    }

    /* Configure all specified channels */
    if( params&PWM_PARAMS_ENABLECHANNEL0 ) {
        timer->CC[0].CTRL =  TIMER_CC_CTRL_ICEVCTRL_RISING
                            |TIMER_CC_CTRL_ICEDGE_RISING
                            |TIMER_CC_CTRL_CMOA_CLEAR
                            |TIMER_CC_CTRL_COIST
                            |TIMER_CC_CTRL_MODE_PWM;
        timer->CC[0].CCVB = 0xFFFF;
    }
    if( params&PWM_PARAMS_ENABLECHANNEL1 ) {
        timer->CC[1].CTRL =  TIMER_CC_CTRL_ICEVCTRL_RISING
                            |TIMER_CC_CTRL_ICEDGE_RISING
                            |TIMER_CC_CTRL_CMOA_CLEAR
                            |TIMER_CC_CTRL_COIST
                            |TIMER_CC_CTRL_MODE_PWM;
        timer->CC[1].CCVB = 0xFFFF;
    }
    if( params&PWM_PARAMS_ENABLECHANNEL2 ) {
        timer->CC[2].CTRL =  TIMER_CC_CTRL_ICEVCTRL_RISING
                            |TIMER_CC_CTRL_ICEDGE_RISING
                            |TIMER_CC_CTRL_CMOA_CLEAR
                            |TIMER_CC_CTRL_COIST
                            |TIMER_CC_CTRL_MODE_PWM;
        timer->CC[2].CCVB = 0xFFFF;
    }

    /* Finally, start timer */
    PWM_Start(timer);

    return 0;
}

/**
 * @brief   Returns status of all channels as a bit vector in a unsigned int
 *
 * @param    timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 *
 * @param    channel  Channel number (0,1,2)
 *
 * @return   The threshold value
 */

unsigned PWM_Read(TIMER_TypeDef* timer, unsigned channel) {
unsigned m;

    m = timer->CC[channel].CCV;

    return m;
}


/**
 * @brief   Find prescaler encoding for a divider value
 *
 * @note    Actually is simple the ln2(v)
 *
 * @param   v the divider
 *
 * @note
 *
 *      |   div    | coding  |
 *      |----------|- -------|
 *      |     1    |     0   |
 *      |     2    |     1   |
 *      |     4    |     2   |
 *      |     5    |     3   |
 *      | ....     | .....   |
 *      |  1024    |    10   |
 *
 * @note    It rounds to the nearest higher power of 2. Is 3 is treated as 4, 64 as
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
 * @brief    PWM_Config
 *
 * @note     Configure timer (top value and divider)
 *
 * @param    timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 *
 * @param    div   prescaler to be used. It is rounded to the nearest highest power of 2
 *
 * @param    top   Maximal value reached by timer. It resets to 0 when reaching it
 *
 */
int PWM_Config(TIMER_TypeDef *timer, unsigned div, unsigned top) {
unsigned d;

    PWM_Stop(timer);

    d = finddivencoding(div);

    timer->CTRL = (timer->CTRL
                      &~(_TIMER_CTRL_PRESC_MASK
                        |_TIMER_CTRL_MODE_MASK))
                  |(d<<_TIMER_CTRL_PRESC_SHIFT)
                  |(TIMER_CTRL_MODE_UP);

    timer->TOP = top;

    PWM_Start(timer);

    return 0;

}

/**
 * @brief   PWM_Write
 *
 * @note    Set threshold value for  a channel
 *
 * @param    timer Pointer to timer as defined by the efm32gg headers (em_device.h *)
 *
 * @param    channel  Channel number (0,1,2)
 *
 * @param    value    The threshold value to be set
 *
 * @note    Uses buffered write to CCV
 */

int PWM_Write(TIMER_TypeDef *timer, unsigned channel, unsigned value) {

    timer->CC[channel].CCVB = value;          // Write to buffer to avoid glitch

    return 0;

}

