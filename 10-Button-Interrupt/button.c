/************************************************************************//**
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

// Constant to access GPIO Port B where buttons are connected
static GPIO_P_TypeDef * const GPIOB = &(GPIO->P[1]);    // GPIOB

/**
 * @brief   global variables for button states and configuration
 */
//{
static uint32_t lastread    = 0;
static uint32_t newestread  = 0;
static uint32_t inputpins   = 0;
static void   (*callback)(uint32_t) = 0;
//}

/**
 * @brief GPIO IRQ Handler (only even pins)
 */
void GPIO_EVEN_IRQHandler(void) {
uint32_t newread;
const uint32_t mask = BIT(10);

    if( GPIO->IF&mask ) {
        lastread   = (lastread&~mask)|(newestread&mask);
        newread = GPIOB->DIN&mask;
        newestread = (newestread&~mask)|newread;
    }
    GPIO->IFC = 0x5555;         // Clear all interrupts from even pins

    if( callback ) callback(mask);
}

/**
 * @brief GPIO IRQ Handler (only odd pins)
 */
void GPIO_ODD_IRQHandler(void) {
uint32_t newread;
const uint32_t mask = BIT(9);

    if( GPIO->IF&mask ) {
        lastread   = (lastread&~mask)|(newestread&mask);
        newread = GPIOB->DIN&mask;
        newestread = (newestread&~mask)|newread;
    }
    GPIO->IFC = 0xAAAA;         // Clear all interrupts from odd pins

    if( callback ) callback(mask);
}

/**
 * @brief Button initialization routine
 */
void Button_Init(uint32_t buttons) {

    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    if ( buttons&BUTTON1 ) {
        GPIOB->MODEH &= ~(_GPIO_P_MODEH_MODE9_MASK);    // Clear bits
        GPIOB->MODEH |= GPIO_P_MODEH_MODE9_INPUT;       // Set bits
        inputpins |= BUTTON1;
        /* Interrupt */

        GPIO->EXTIPSELH = (GPIO->EXTIPSELH&~(_GPIO_EXTIPSELH_EXTIPSEL9_MASK))
                            |GPIO_EXTIPSELH_EXTIPSEL9_PORTB;
        GPIO->EXTIRISE  |= BIT(9);
        GPIO->EXTIFALL  |= BIT(9);
        GPIO->IEN       |= BIT(9);

    }

    if ( buttons&BUTTON2 ) {
        GPIOB->MODEH &= ~(_GPIO_P_MODEH_MODE10_MASK);    // Clear bits
        GPIOB->MODEH |= GPIO_P_MODEH_MODE10_INPUT;       // Set bits
        inputpins |= BUTTON2;
        /* Interrupt */
        GPIO->EXTIPSELH = (GPIO->EXTIPSELH&~(_GPIO_EXTIPSELH_EXTIPSEL10_MASK))
                            |GPIO_EXTIPSELH_EXTIPSEL10_PORTB;
        GPIO->EXTIRISE  |= BIT(10);
        GPIO->EXTIFALL  |= BIT(10);
        GPIO->IEN       |= BIT(10);
    }
    // First read
    lastread = GPIOB->DIN&inputpins;

    // Clear all interrupts from GPIO
    GPIO->IFC = 0xFFFF;

    /* Enable interrupts */
    NVIC_SetPriority(GPIO_EVEN_IRQn,BUTTON_INT_LEVEL);
    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);

    NVIC_SetPriority(GPIO_ODD_IRQn,BUTTON_INT_LEVEL);
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**
 * @brief Returns the last status of buttons
 *
 * @note  Only pins configured as buttons are returned. The others are masked.
 */
uint32_t Button_Read(void) {

    return newestread&inputpins;
}


/**
 * @brief Returns the buttons whose status are changed
 *
 * @note  Only pins configured as buttons are returned. The others are masked.
 */
uint32_t Button_ReadChanges(void) {
uint32_t changes;

    changes = newestread^lastread;
    lastread = newestread;

    return changes&inputpins;
}


/**
 * @brief Returns the buttons, which were released
 *
 * @note  Only pins configured as buttons are returned. The others are masked.
 */
uint32_t Button_ReadReleased(void) {
uint32_t changes;

    changes = newestread&~lastread;
    lastread = newestread;

    return changes&inputpins;
}

/**
 * @brief Returns the buttons, which were pressed
 *
 * @note  Only pins configured as buttons are returned. The others are masked.
 */
uint32_t Button_ReadPressed(void) {
uint32_t changes;

    changes = ~newestread&lastread;
    lastread = newestread;

    return changes&inputpins;
}


/**
 * @brief Set Callback routine
 */
void Button_SetCallback( void (*proc)(uint32_t parm) ) {

    callback = proc;

}
