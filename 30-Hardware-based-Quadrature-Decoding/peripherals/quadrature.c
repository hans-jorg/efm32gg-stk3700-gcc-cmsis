/**
 * @file    quadrature.c
 * @brief   HAL for a position encoder with quadrature outputs
 *
 * @version 1.0
 *
 * @note   There are two implementation. One, more precise, reading the
 *         A and B inputs and decoding it with a state machine. The other
 *         uses one input as a clock and the other as a direction signal.
 *
 * @note   Both inputs must be on the same GPIO port
 *
 * @note   It uses a debouncing algorithm. At start, it needs N cycles to stabilize.
 *
 */


#include  <stdint.h>


#include "em_device.h"

#include "gpio.h"
#include "quadrature.h"



//#define USING_PULSE_DIR_SIGNALS_TIMER
//#define USING_A_B_SIGNALS
#define USING_PULSE_DIR_SIGNALS_PCNT

// Default
#if !defined(USING_A_B_SIGNALS) && !defined(USING_PULSE_DIR_SIGNALS_TIMER)&& \
    !defined(USING_PULSE_DIR_SIGNALS_PCNT)
#define USING_A_B_SIGNALS
#endif

/*****************************************************************************
 * @brief  Quadrature decoder
 *****************************************************************************/

///@{
#define QUADRATURE_GPIO             GPIOD
#define QUADRATURE_PIN0             7
#define QUADRATURE_PIN1             6
#define QUADRATURE_PINBUTTON        5

#if defined(USING_PULSE_DIR_SIGNALS_TIMER) || defined(USING_A_B_SIGNALS)
#define QUADRATURE_TIMER            TIMER1
#define QUADRATURE_LOC              4

// TBD: Make it configurable only on QUADRATURE_TIMER
//#if QUADRATURE_TIMER == TIMER1
#define QUADRATURE_CLOCKEN          CMU_HFPERCLKEN0_TIMER1
//#elif QUADRATURE_TIMER == TIMER2
//#define QUADRATURE_CLOCKEN          CMU_HFPERCLKEN0_TIMER2
//#elif QUADRATURE_TIMER == TIMER3
//#define QUADRATURE_CLOCKEN          CMU_HFPERCLKEN0_TIMER3
//#endif
///@}
#endif

#if defined(USING_PULSE_DIR_SIGNALS_PCNT)
#define QUADRATURE_PCNT             PCNT0
#define QUADRATURE_LOC              3


// PCNT->CNT can not be reset. Remember the value and subtract it
// when returned
static int16_t pcnt_cnt_ref = 0;

// TBD: Make it configurable only on QUADRATURE_PCNT
//#if QUADRATURE_PCNT == PCNT0
#define QUADRATURE_CLOCKEN          CMU_PCNTCTRL_PCNT0CLKEN
#define QUADRATURE_CLOCKSEL         CMU_PCNTCTRL_PCNT0CLKSEL_PCNT0S0
#define QUADRATURE_CLOCKEN_MASK     _CMU_PCNTCTRL_PCNT0CLKEN_MASK
#define QUADRATURE_CLOCKSEL_MASK    _CMU_PCNTCTRL_PCNT0CLKSEL_MASK
//
//#define QUADRATURE_CLOCKEN          CMU_PCNTCTRL_PCNT1CLKEN
//#define QUADRATURE_CLOCKSEL         CMU_PCNTCTRL_PCNT1CLKSEL
#endif



#define QUADRATURE_M                ((1<<QUADRATURE_PIN0)|(1<<QUADRATURE_PIN1))
#define QUADRATURE_BUTTON           (1<<QUADRATURE_PINBUTTON)

// These routines are identical for each deboding method


int Quadrature_GetPosition(void) {
#if defined(USING_PULSE_DIR_SIGNALS_TIMER) || defined(USING_A_B_SIGNALS)
    return (int16_t) QUADRATURE_TIMER->CNT;
#endif
#if defined(USING_PULSE_DIR_SIGNALS_PCNT)
    return (int16_t) QUADRATURE_PCNT->CNT - (int16_t) pcnt_cnt_ref;
#endif
}

void Quadrature_Reset(void) {
#if defined(USING_PULSE_DIR_SIGNALS_TIMER) || defined(USING_A_B_SIGNALS)
    QUADRATURE_TIMER->CNT = 0;
#endif
#if defined(USING_PULSE_DIR_SIGNALS_PCNT)
    pcnt_cnt_ref = QUADRATURE_PCNT->CNT;
#endif
}


int Quadrature_GetButtonStatus(void) {

    return ~(QUADRATURE_GPIO->DIN)&QUADRATURE_BUTTON;
}

void QuadratureProcess(void) {}



#ifdef USING_A_B_SIGNALS

/*
 *  Configuration for A and B decoding
 */

void Quadrature_Init(void) {

    /* Enable Clock for GPIO and TIMER */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCLK for GPIO
    CMU->HFPERCLKEN0 |= QUADRATURE_CLOCKEN;             // Enable HFPERCLK for TIMERx

    // Configure A, B and BUTTON as input
    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M|QUADRATURE_BUTTON,0);

    // Pullup for button
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_BUTTON,GPIO_MODE_INPUTPULLFILTER);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_BUTTON);

    // Pullup for quadrature inputs
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_M,GPIO_MODE_INPUTPULL);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_M);

    // Configure timer
    QUADRATURE_TIMER->CTRL  = (QUADRATURE_TIMER->CTRL
                                    &~(_TIMER_CTRL_QDM_MASK
                                      |_TIMER_CTRL_MODE_MASK
                                      |_TIMER_CTRL_CLKSEL_MASK
                                      |_TIMER_CTRL_PRESC_MASK))
                              |TIMER_CTRL_QDM_X4    // mode x4
                              |TIMER_CTRL_MODE_QDEC
                              |TIMER_CTRL_CLKSEL_PRESCHFPERCLK
                              |TIMER_CTRL_PRESC_DIV1;

    // Enable filter 5x
    QUADRATURE_TIMER->CC[0].CTRL |= TIMER_CC_CTRL_FILT;
    QUADRATURE_TIMER->CC[1].CTRL |= TIMER_CC_CTRL_FILT;

    // Configure pins
    QUADRATURE_TIMER->ROUTE = (QUADRATURE_TIMER->ROUTE
                                &~(_TIMER_ROUTE_LOCATION_MASK
                                    |_TIMER_ROUTE_CC0PEN_MASK
                                    |_TIMER_ROUTE_CC0PEN_MASK))
                             |(QUADRATURE_LOC<<_TIMER_ROUTE_LOCATION_SHIFT)
                             |TIMER_ROUTE_CC0PEN
                             |TIMER_ROUTE_CC1PEN;

    // Start timer
    QUADRATURE_TIMER->CMD = TIMER_CMD_START;

}

#endif

#ifdef USING_PULSE_DIR_SIGNALS_TIMER
/*
 *  Configuration for lock and dir inputs using TIMER
 */
void Quadrature_Init(void) {

    /* Enable Clock for GPIO and TIMER */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCLK for GPIO
    CMU->HFPERCLKEN0 |= QUADRATURE_CLOCKEN;             // Enable HFPERCLK for TIMERx

    // Configure A, B and BUTTON as input
    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M|QUADRATURE_BUTTON,0);

    // Pullup for button
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_BUTTON,GPIO_MODE_INPUTPULLFILTER);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_BUTTON);

    // Pullup for quadrature inputs
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_M,GPIO_MODE_INPUTPULL);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_M);


    // Configure timer
    QUADRATURE_TIMER->CTRL  = (QUADRATURE_TIMER->CTRL
                                    &~(_TIMER_CTRL_QDM_MASK
                                      |_TIMER_CTRL_MODE_MASK
                                      |_TIMER_CTRL_CLKSEL_MASK
                                      |_TIMER_CTRL_PRESC_MASK))
                              |TIMER_CTRL_QDM_X4    // mode x4
                              |TIMER_CTRL_MODE_QDEC
                              |TIMER_CTRL_CLKSEL_PRESCHFPERCLK
                              |TIMER_CTRL_PRESC_DIV16;

    // Enable filter 5x
    QUADRATURE_TIMER->CC[0].CTRL |= TIMER_CC_CTRL_FILT;
    QUADRATURE_TIMER->CC[1].CTRL |= TIMER_CC_CTRL_FILT;

    // Configure pins
    QUADRATURE_TIMER->ROUTE = (QUADRATURE_TIMER->ROUTE&~_TIMER_ROUTE_LOCATION_MASK)
                             |(QUADRATURE_LOC<<_TIMER_ROUTE_LOCATION_SHIFT)
                             |TIMER_ROUTE_CC0PEN
                             |TIMER_ROUTE_CC1PEN;

    // Start timer
    QUADRATURE_TIMER->CMD = TIMER_CMD_START;
}


#endif


#ifdef USING_PULSE_DIR_SIGNALS_PCNT
/*
 *  Configuration for lock and dir inputs using PCNT
 */
void Quadrature_Init(void) {

    /* Enable Clock for GPIO and TIMER */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCLK for GPIO

    CMU->PCNTCTRL     = (CMU->PCNTCTRL
                         &~(QUADRATURE_CLOCKEN_MASK|QUADRATURE_CLOCKSEL_MASK))
                        |(QUADRATURE_CLOCKEN|QUADRATURE_CLOCKSEL);


    // Configure A, B and BUTTON as input
    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M|QUADRATURE_BUTTON,0);

    // Pullup for button
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_BUTTON,GPIO_MODE_INPUTPULLFILTER);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_BUTTON);

    // Pullup for quadrature inputs
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_M,GPIO_MODE_INPUTPULL);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_M);


    // Configure timer
    QUADRATURE_PCNT->CTRL  = (QUADRATURE_PCNT->CTRL
                                    &~(_PCNT_CTRL_MODE_MASK))
                              |PCNT_CTRL_MODE_EXTCLKQUAD;

    // Configure pins
    QUADRATURE_PCNT->ROUTE = (QUADRATURE_PCNT->ROUTE
                                &~(_PCNT_ROUTE_LOCATION_MASK))
                             |(QUADRATURE_LOC<<_PCNT_ROUTE_LOCATION_SHIFT);

    // WTF
    QUADRATURE_PCNT->TOPB = (int16_t) -1;
}


#endif

