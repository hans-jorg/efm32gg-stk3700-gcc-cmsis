
/**
 * @file    touch.c
 * @brief   Capacitive Touch Sensor Reading for EFM32GG
 * @version 1.0
 *
 * @note    Uses ACMP1 Channel 0,1,2,3 as in STK-3700 board
 *
 * @note    Pinout and Ports
 *
 *    Signal     |   Port/Pin    | Analog comparator channel
 *   ------------|---------------|----------------------------
 *   UIF_TOUCH0  |  PC8          |    ACMP1/CH0
 *   UIF_TOUCH1  |  PC9          |    ACMP1/CH1
 *   UIF_TOUCH2  |  PC10         |    ACMP1/CH2
 *   UIF_TOUCH3  |  PC11         |    ACMP1/CH3
 */


#include <stdint.h>
#include "em_device.h"
#include "gpio.h"
#include "touch.h"

/*
 * Hardware configuration
 */
#define TOUCH_ACMP          ACMP1
#define TOUCH_GPIO          GPIOC
#define TOUCH_TIMER         TIMER1
#define TOUCH_CH0           0
#define TOUCH_CH1           1
#define TOUCH_CH2           2
#define TOUCH_CH3           3
#define TOUCH_P0            8
#define TOUCH_P1            9
#define TOUCH_P2           10
#define TOUCH_P3           11
#define TOUCH_INVERTSEQUENCE 1
/**
 * Configuration in GeckoSDK for EFM32GG-STK3700
 *  fullBias = false
 *  halfBias = false
 *  biasProg = 0x7
 *  warmuptime = 512
 *  hysteresis = 5
 *  resistor = 3
 *  lowpower = false
 *  vddlevel = 0x3D = 61
 *
 *   CSRESSEL  |   Resistor (Ohm)
 *   ----------|--------------------
 *      00     |     39 K
 *      01     |     71 K
 *      10     |    104 K
 *      11     |    136 K
 */

// Hysteresis 0-7 (7=Higher)
#define HYST                5
// VDD diviser (Pull up voltage)
#define VDIV                40
// Res value: 0-3
#define RES                 3
// location
#define LOC                 0
// Peripheral Reflex System channel
#define PRSSCH              11
// Biasprog
#define BIASPROG            7
// Fullbias
#define FULLBIAS            0
// Halfbias
#define HALFBIAS            0

// Threshold to detect touch (NUM/DEN)
#define THRESHOLD_NUM       8
#define THRESHOLD_DEN       10




#define CONCAT(A,B)  A##B
#define STRINGFY(A)  #A

#define TIMER_CC_CTRL_PRSSEL_PRSCH_PRSSCH(N) CONCAT(TIMER_CC_CTRL_PRSSEL_PRSCH,N)

/**
 * @brief   Interpolation table
 */
unsigned interpolated[] = {
/*0000  0000000*/   0x00,
/*0001  0000001*/   0x01,
/*0010  0000100*/   0x04,
/*0011  0000010*/   0x02,
/*0100  0010000*/   0x10,
/*0101  0000100*/   0x04,
/*0110  0001000*/   0x08,
/*0111  0000100*/   0x01,
/*1000  1000000*/   0x80,
/*1001  0001000*/   0x10,
/*1010  0100000*/   0x40,
/*1011  0000100*/   0x04,
/*1100  0010000*/   0x10,
/*1101  0100000*/   0x20,
/*1110  0010000*/   0x10,
/*1111  0001000*/   0x08,
};

/**
 * @brief Prototypes
 */

void Touch_Restart(void);


/**
 *  @brief  Global data
 */

static uint16_t measurements[TOUCH_N];
static uint16_t measurements_max[TOUCH_N];

static int channelindex = 0;
static int pollcompleted = 0;

/**
 * @brief   Returns status of all channels as a bit vector in a unsigned int
 */

void
Touch_PeriodicProcess(void) {
uint16_t v;

    v = TOUCH_TIMER->CNT;

    if( v > measurements_max[channelindex] )
        measurements_max[channelindex] = v;

    measurements[channelindex++] = v;

    if( channelindex >= TOUCH_N ) {
        channelindex = 0;
        pollcompleted = 1;
    }

    Touch_Restart();

    return;
}


/**
 * @brief   Touch_Start
 *
 */
void Touch_Start(void) {

    channelindex = 0;

    Touch_Restart();

}


/**
 * @brief   Touch_Start
 *
 */
void
Touch_Restart(void) {
int ch = channelindex;

    TOUCH_TIMER->CNT = 0;

    TOUCH_ACMP->INPUTSEL = (TOUCH_ACMP->INPUTSEL
                            &~(_ACMP_INPUTSEL_POSSEL_MASK))
                          |(ch)<<_ACMP_INPUTSEL_POSSEL_SHIFT;

    TOUCH_TIMER->CMD = TIMER_CMD_START;
//    NVIC_EnableIRQ(TOUCH_IRQ);

}

/**
 * @brief   Initializes ADC and get calibration values
 */


int
Touch_Init(void) {



    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    /* Enable clock for ACMP */
    // Use CONCAT later CMU->HFPERCLKEN0 |= CONCAT(CMU_HFPERCLKEN0_,TOUCH_ACMP
    if( TOUCH_ACMP == ACMP0 )
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_ACMP0;
    else if ( TOUCH_ACMP == ACMP1 )
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_ACMP1;
    else
        return -1;

    /* Enable clock for Peripheral Reflex Systems (PRS) */
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_PRS;

    /* Enable clock for timer */
    // Use CONCAT later:  CMU->HFPERCLKEN0 |= CONCAT(CMU_HFPERCLKEN0_,TOUCH_TIMER)
    if( TOUCH_TIMER == TIMER0 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0;
    } else if ( TOUCH_TIMER == TIMER1 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER1;
    } else if ( TOUCH_TIMER == TIMER2 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER2;
    } else if ( TOUCH_TIMER == TIMER3 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER3;
    } else {
        return -1;
    }

    /* Configure timer for counting pulses on CC1 */
    TOUCH_TIMER->CTRL = (TOUCH_TIMER->CTRL
                            &~_TIMER_CTRL_CLKSEL_MASK)
                        |TIMER_CTRL_CLKSEL_CC1;

    TOUCH_TIMER->TOP = 0xFFFF;

    /* Configure channel 1 of timer to use PRS channel PRSSCH */
    //TOUCH_TIMER->CC[1].CTRL = (TOUCH_TIMER->CC[1].CTRL
    //                            &~( _TIMER_CC_CTRL_PRSSEL_MASK
    //                               |_TIMER_CC_CTRL_INSEL_MASK
    //                               |_TIMER_CC_CTRL_MODE_MASK))
    TOUCH_TIMER->CC[1].CTRL = TIMER_CC_CTRL_INSEL_PRS
                             |TIMER_CC_CTRL_MODE_OFF //INPUTCAPTURE
                             |TIMER_CC_CTRL_PRSSEL_PRSCH_PRSSCH(PRSSCH)
                             |TIMER_CC_CTRL_ICEVCTRL_RISING
                             |TIMER_CC_CTRL_ICEDGE_BOTH;

    TOUCH_TIMER->IEN = 0;

    // Configure PRS to route ACMP output to channel PRSSCH
    if( TOUCH_ACMP == ACMP0 ) {
        PRS->CH[PRSSCH].CTRL =  PRS_CH_CTRL_EDSEL_POSEDGE
                                |PRS_CH_CTRL_SIGSEL_ACMP0OUT
                                |PRS_CH_CTRL_SOURCESEL_ACMP0;
    } else if ( TOUCH_ACMP == ACMP1 ) {
        PRS->CH[PRSSCH].CTRL =   PRS_CH_CTRL_EDSEL_POSEDGE
                                |PRS_CH_CTRL_SIGSEL_ACMP1OUT
                                |PRS_CH_CTRL_SOURCESEL_ACMP1;
    } else {
        return -3;
    }


    // Set bias for fast response (higher power consumption))
    TOUCH_ACMP->CTRL = (TOUCH_ACMP->CTRL
                            &~( _ACMP_CTRL_BIASPROG_MASK
                               |_ACMP_CTRL_HYSTSEL_MASK
                               |_ACMP_CTRL_FULLBIAS_MASK
                               |_ACMP_CTRL_HALFBIAS_MASK
                               |_ACMP_CTRL_WARMTIME_MASK)
                              )
                        |(BIASPROG<<_ACMP_CTRL_BIASPROG_SHIFT)
                        |(HYST<<_ACMP_CTRL_HYSTSEL_SHIFT)
                        |(FULLBIAS<<_ACMP_CTRL_FULLBIAS_SHIFT)
                        |(HALFBIAS<<_ACMP_CTRL_HALFBIAS_SHIFT)
                        |ACMP_CTRL_WARMTIME_512CYCLES
                        |ACMP_CTRL_MUXEN;

    // Configure pins
    TOUCH_ACMP->ROUTE = (TOUCH_ACMP->ROUTE&~(_ACMP_ROUTE_LOCATION_MASK))
                        |LOC<<_ACMP_ROUTE_LOCATION_SHIFT;

    // Configure
    TOUCH_ACMP->INPUTSEL = (TOUCH_ACMP->INPUTSEL
                            &~(_ACMP_INPUTSEL_NEGSEL_MASK
                              |_ACMP_INPUTSEL_VDDLEVEL_MASK
                              |_ACMP_INPUTSEL_CSRESSEL_MASK
                              |_ACMP_INPUTSEL_POSSEL_MASK))
                          |ACMP_INPUTSEL_NEGSEL_CAPSENSE
                          |ACMP_INPUTSEL_CSRESEN
                          |(VDIV)<<_ACMP_INPUTSEL_VDDLEVEL_SHIFT
                          |(RES)<<_ACMP_INPUTSEL_CSRESSEL_SHIFT;


    // Enable
    TOUCH_ACMP->CTRL |= ACMP_CTRL_EN;

    //while( (TOUCH_ACMP->STATUS&ACMP_STATUS_ACMPACT) == 0 ) {} // Timeout??
    Touch_Start();

    return 0;
}

/**
 * @brief   Returns status of all channels as a bit vector in a unsigned int
 */

unsigned
Touch_Read(void) {
int i;
unsigned m;

#ifdef TOUCH_INVERTSEQUENCE
    m = 0;
    for(i=0;i<TOUCH_N;i++) {
        m <<= 1;
        if( measurements[i] < measurements_max[i]*THRESHOLD_NUM/THRESHOLD_DEN )
            m |= 1;
    }
#else
    m = 0;
    for(i=TOUCH_N-1;i>=0;i--) {
        m <<= 1;
        if( measurements[i] < measurements_max[i]*THRESHOLD_NUM/THRESHOLD_DEN )
            m |= 1;
    }
#endif


    pollcompleted = 1;

    return m;
}



/**
 * @brief   Returns status of a channel as a LSB bit of a unsigned int
 */

unsigned
Touch_ReadChannel(int ch){
uint32_t v = 0;

    v = measurements[ch] < measurements_max[ch]*THRESHOLD_NUM/THRESHOLD_DEN;
    return v;
}

/**
 * @brief   Return center of touch
 *
 * @note    Returns -1 when there is no touch
 */
int
Touch_GetCenterOfTouch(unsigned v) {
int i;
unsigned p,m,sum;
int n1;

    sum = 0;
    p = 1;
    m = 1;
    n1 = 0;
    for(i=0;i<TOUCH_N;i++) {
        if( v&m ) {
            sum += p;
            n1++;
        }
        p = p+2;
        m <<= 1;
    }
    if( n1 )
        return (sum+n1-1)/n1;
    else
        return -1;
}

/**
 * @brief   Return interpolated touch
 *
 * @note    Returns 0 when there is no touch and 1 bit of 2*TOUCH_N
 *          indicating center of touch
 */
unsigned
Touch_Interpolate(unsigned v) {
int k;

    k = Touch_GetCenterOfTouch(v);

    if( k < 0 ) return 0;

    return 1<<(k-1);
}

