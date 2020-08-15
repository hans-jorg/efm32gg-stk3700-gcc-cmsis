/** **************************************************************************
 * @file    button.c
 * @brief   Button HAL for EFM32GG STK
 * @version 1.0
 *****************************************************************************/

#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */
#include "em_device.h"

#include "button.h"

#ifndef BUTTON_INT_LEVEL
#define BUTTON_INT_LEVEL 3
#endif

static GPIO_P_TypeDef * const GPIOB = &(GPIO->P[1]);    // GPIOB

typedef struct {
    uint32_t    mask;
    uint32_t    counter;
    uint32_t    status;
    } Button_t;

static Button_t buttontable[] = {
    {   BUTTON1,    0,  0   },
    {   BUTTON2,    0,  0   },
    {   0,          0,  0   }
};

static uint32_t pressed    = 0;
static uint32_t released   = 0;
static uint32_t changed    = 0;


static void   (*callback)(uint32_t) = 0;

void Button_Processing(void) {

    uint32_t b = GPIOB->DIN;

    Button_t *p = buttontable;
    while(p->mask ) {
        switch(p->status) {
        case 0:         // Released
            if( b&p->mask ) {
                if( p->counter == 10 ) {
                    p->status = 1;
                    changed |= p->mask;
                } else {
                    p->counter++;
                }
            } else { // Not Pressed
                p->counter = 0;
            }
        case 1:         // Pressed
            if( b&p->mask ) {
                p->counter = 0;
            } else { // Not Pressed
                if( p->counter == 10 ) {
                    p->status = 0;
                    released |= p->mask;
                } else {
                    p->counter++;
                }
            }
        }
        p++;
    }
    changed = pressed | released;
    if( changed )
        callback(changed);
}

uint32_t Button_Status(void) {
uint32_t b = 0;

    Button_t *p = buttontable;
    while(p->mask ) {
        if (p->status) b |= p->mask;
        p++;
    }
    return b;
}




void Button_Init(uint32_t buttons) {

    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    if ( buttons&BUTTON1 ) {
        GPIOB->MODEH &= ~(_GPIO_P_MODEH_MODE9_MASK);    // Clear bits
        GPIOB->MODEH |= GPIO_P_MODEH_MODE9_INPUT;       // Set bits
    }

    if ( buttons&BUTTON2 ) {
        GPIOB->MODEH &= ~(_GPIO_P_MODEH_MODE10_MASK);    // Clear bits
        GPIOB->MODEH |= GPIO_P_MODEH_MODE10_INPUT;       // Set bits
    }

}

uint32_t Button_Read(void) {

    return Button_Status();
}


uint32_t Button_ReadChanges(void) {
uint32_t c = changed;

    changed = 0;
    return c;
}

uint32_t Button_ReadReleased(void) {
uint32_t r = released;

    released = 0;
    return r;
}

uint32_t Button_ReadPressed(void) {
uint32_t p = pressed;

    pressed = 0;
    return p;
}

void Button_SetCallback( void (*proc)(uint32_t parm) ) {

    callback = proc;

}
