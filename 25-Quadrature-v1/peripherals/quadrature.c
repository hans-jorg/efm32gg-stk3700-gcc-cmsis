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



#define USING_PULSE_DIR_SIGNALS
//#define USING_A_B_SIGNALS


// Default
#if !defined(USING_A_B_SIGNALS) && !defined(USING_PULSE_DIR_SIGNALS)
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
///@}

/**
 * @brief   Debouncing parameter.
 *
 * @note    A signal is considered stable after N consecutive identical values
*/
#define QUADRATURE_DEBOUNCE_N   3

/**
 * @brief   Mask for decoder signals and button
 */

///@{
#define QUADRATURE_M            ((1U<<QUADRATURE_PIN0)|(1U<<QUADRATURE_PIN1))
#define QUADRATURE_BUTTON       (1U<<QUADRATURE_PINBUTTON)
///@}

/**
 * @brief   static variables
 */
///@{
static int      qcounter = 0;                       /**< position counter   */
static uint32_t qnow = 0;                           /**@< debounced input   */
static uint32_t qlast[QUADRATURE_DEBOUNCE_N];       /**@< circular buffer   */
static int      qi = 0;                             /**@< buffer pointer    */
///@}



// These routines are identical for each deboding method

void Quadrature_Init(void) {

    GPIO_Init(QUADRATURE_GPIO,QUADRATURE_M|QUADRATURE_BUTTON,0);
// Pullup for button
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_BUTTON,GPIO_MODE_INPUTPULLFILTER);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_BUTTON);

// Pullup for quadrature inputs
    GPIO_ConfigPins(QUADRATURE_GPIO,QUADRATURE_M,GPIO_MODE_INPUTPULLFILTER);
    GPIO_WritePins(QUADRATURE_GPIO,0,QUADRATURE_M);

}

int Quadrature_GetPosition(void) {

    return qcounter;
}

void Quadrature_Load(int v) {

    qcounter = v;

}

void Quadrature_Reset(void) {

    qcounter = 0;
}


int Quadrature_GetButtonStatus(void) {

    return (~qnow)&QUADRATURE_BUTTON;
}





#ifdef USING_A_B_SIGNALS
/*
 *  Configuration for A and B decoding
 */
#define QUADRATURE_PIN_A         QUADRATURE_PIN0
#define QUADRATURE_PIN_B         QUADRATURE_PIN1

#define QUADRATURE_A            (1U<<QUADRATURE_PIN_A)
#define QUADRATURE_B            (1U<<QUADRATURE_PIN_B)

#define AB_00                   0
#define AB_01                   QUADRATURE_B
#define AB_10                   QUADRATURE_A
#define AB_11                   (QUADRATURE_A|QUADRATURE_B)

/**
 * @brief   AB decoding process
 *
 * @note    Must be called at constant intervals, that must be much less than the
 *          input period
 */
void Quadrature_Process(void) {
typedef enum { Q0, Q1, Q2, Q3 } state_t;
static state_t state = Q0;  /* initial state */
uint32_t qand, qor;
uint32_t ab;
uint32_t qread;

    qread = GPIO_ReadPins(QUADRATURE_GPIO)&QUADRATURE_M;

    qlast[qi++] = qread;
    if( qi >= QUADRATURE_DEBOUNCE_N ) qi = 0;

    qand = qor = qlast[0];
    for(int i=1;i<QUADRATURE_DEBOUNCE_N;i++) {
        qand &= qlast[i];
        qor  |= qlast[i];
    }
    qnow |= qand;           // set bit if they are are all 1 in this position
    qnow &= qor;            // clear bit if there are all 0 in this position

    ab = qnow&QUADRATURE_M;

    switch(state) {
    case Q0:
        if( ab == AB_01 ) {
            state = Q1;
            qcounter++;
        } else if ( ab == AB_10 ) {
            state = Q3;
            qcounter--;
        }
        break;
    case Q1:
        if( ab == AB_11 ) {
            state = Q2;
            qcounter++;
        } else if ( ab == AB_00 ) {
            state = Q0;
            qcounter--;
        }
        break;
    case Q2:
        if( ab == AB_10 ) {
            state = Q3;
            qcounter++;
        } else if ( ab == AB_01 ) {
            state = Q1;
            qcounter--;
        }
        break;
    case Q3:
        if( ab == AB_00 ) {
            state = Q0;
            qcounter++;
        } else if ( ab == AB_11 ) {
            state = Q2;
            qcounter--;
        }
        break;
    }
}

#endif

#ifdef USING_PULSE_DIR_SIGNALS
/*
 *  Configuration for lock and dir inputs
 */
#define QUADRATURE_PINPULSE     QUADRATURE_PIN0
#define QUADRATURE_PINDIR       QUADRATURE_PIN1

#define QUADRATURE_PULSE        (1U<<QUADRATURE_PINPULSE)
#define QUADRATURE_DIR          (1U<<QUADRATURE_PINDIR)

/**
 * @brief   Clock and dir processisng
 *
 * @note    Must be called at constant intervals, that must be much less than the
 *          input period
 */

void Quadrature_Process(void) {
static uint32_t qant = 0;
uint32_t qand, qor;
uint32_t qread;

    qread = GPIO_ReadPins(QUADRATURE_GPIO);

    qlast[qi++] = qread;
    if( qi >= QUADRATURE_DEBOUNCE_N ) qi = 0;

    qand = qor = qlast[0];
    for(int i=1;i<QUADRATURE_DEBOUNCE_N;i++) {
        qand &= qlast[i];
        qor  |= qlast[i];
    }
    qnow |= qand;           // set bit if they are are all 1 in this position
    qnow &= qor;            // clear bit if there are all 0 in this position

    if( (qnow^qant)&QUADRATURE_PULSE ) {
        if( qnow&QUADRATURE_PULSE ) {
            if( qnow&QUADRATURE_DIR ) {
                qcounter++;
            } else {
                qcounter--;
            }
        }
    }
    qant = qnow;
}
#endif

