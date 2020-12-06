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


#ifndef LCD_EMULATION
#include "em_device.h"
#endif

#include "gpio.h"
#include "quadrature.h"



#define USING_PULSE_DIR_SIGNALS
//#define USING_A_B_SIGNALS


// Default
#if !defined(USING_A_B_SIGNALS) && !defined(USING_PULSE_DIR_SIGNALS)
#define USING_A_B_SIGNALS
#endif

/*****************************************************************************
 * @brief  Quadrature decoder
 *****************************************************************************/

#define TIMER_1   (0)
///@{
#define QUADRATURE_GPIO             GPIOD
#define QUADRATURE_PIN0             7
#define QUADRATURE_PIN1             6
#define QUADRATURE_PINBUTTON        5
#define QUADRATURE_TIMER            TIMER1
#define QUADRATURE_LOC              4
///@}

//#if QUADRATURE_TIMER == TIMER1
#define QUADRATURE_CLOCKEN          CMU_HFPERCLKEN0_TIMER1
//#elif QUADRATURE_TIMER == TIMER2
//#define QUADRATURE_CLOCKEN          CMU_HFPERCLKEN0_TIMER2
//#elif QUADRATURE_TIMER == TIMER3
//#define QUADRATURE_CLOCKEN          CMU_HFPERCLKEN0_TIMER3
//#endif


#define QUADRATURE_M                ((1<<QUADRATURE_PIN1)|(1<<QUADRATURE_PIN1))
#define QUADRATURE_BUTTON           (1<<QUADRATURE_PINBUTTON)

// These routines are identical for each deboding method



int Quadrature_GetPosition(void) {

    return QUADRATURE_TIMER->CNT;
}


int Quadrature_GetButtonStatus(void) {

    return QUADRATURE_GPIO->DIN&QUADRATURE_BUTTON;
}





#ifdef USING_A_B_SIGNALS

/*
 *  Configuration for A and B decoding
 */

void Quadrature_Init(void) {

    /* Enable Clock for GPIO and TIMER */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCLK for GPIO
    CMU->HFPERCLKEN0 |= QUADRATURE_CLOCKEN;             // Enable HFPERCLK for TIMERx

    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M|QUADRATURE_BUTTON,0);

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

#ifdef USING_PULSE_DIR_SIGNALS
/*
 *  Configuration for lock and dir inputs
 */
void Quadrature_Init(void) {

    /* Enable Clock for GPIO and TIMER */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCLK for GPIO
    CMU->HFPERCLKEN0 |= QUADRATURE_CLOCKEN;             // Enable HFPERCLK for TIMERx

    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M|QUADRATURE_BUTTON,0);

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

