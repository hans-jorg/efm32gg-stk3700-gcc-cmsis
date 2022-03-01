#ifndef GPIO_H
#define GPIO_H
/**
 * @file    gpio.h
 * @brief   GPIO HAL for EFM32GG
 * @version 1.0
 */

#include <stdint.h>
#include "em_device.h"

/**
 * @brief   When set, use DOUTCLR and DOUTSET for output
 *          When not, only DOUT is used in a Read-Modify-Write cyclus
 */

#define GPIO_ALTERNATE_OPERATIONS


/**
 * @brief   Pointer to GPIO registers
 */
typedef GPIO_P_TypeDef *GPIO_t;

/**
 * @brief   Pointers for GPIO ports
 */
//@{
static const GPIO_t GPIOA = &(GPIO->P[0]);  // GPIOA
static const GPIO_t GPIOB = &(GPIO->P[1]);  // GPIOB
static const GPIO_t GPIOC = &(GPIO->P[2]);  // GPIOC
static const GPIO_t GPIOD = &(GPIO->P[3]);  // GPIOD
static const GPIO_t GPIOE = &(GPIO->P[4]);  // GPIOE
static const GPIO_t GPIOF = &(GPIO->P[5]);  // GPIOF
//@}


/**
 * @brief   mode options
 */
//@{
static const uint32_t GPIO_MODE_DISABLE                           =   0x0;
static const uint32_t GPIO_MODE_INPUT                             =   0x1;
static const uint32_t GPIO_MODE_INPUTPULL                         =   0x2;
static const uint32_t GPIO_MODE_INPUTPULLFILTER                   =   0x3;
static const uint32_t GPIO_MODE_PUSHPULL                          =   0x4;
static const uint32_t GPIO_MODE_PUSHPULLDRIVE                     =   0x5;
static const uint32_t GPIO_MODE_WIREDOR                           =   0x6;
static const uint32_t GPIO_MODE_WIREDORPULLDOWN                   =   0x7;
static const uint32_t GPIO_MODE_WIREDAND                          =   0x8;
static const uint32_t GPIO_MODE_WIREDANDFILTER                    =   0x9;
static const uint32_t GPIO_MODE_WIREDANDPULLUP                    =   0xA;
static const uint32_t GPIO_MODE_WIREDANDPULLUPFILTER              =   0xB;
static const uint32_t GPIO_MODE_WIREDANDDRIVE                     =   0xC;
static const uint32_t GPIO_MODE_WIREDANDDRIVEFILTER               =   0xD;
static const uint32_t GPIO_MODE_WIREDANDDRIVEPULLUP               =   0xE;
static const uint32_t GPIO_MODE_WIREDANDDRIVEPULLUPFILTER         =   0xF;
//@}

void GPIO_ConfigPins(GPIO_t gpio, uint32_t pins, uint32_t mode);
void GPIO_Init(GPIO_t gpio, uint32_t inputs, uint32_t outputs);

/**
 * @brief   Set specified pins to zero, then others (or same) to 1
 * @param   gpio:   pointer to GPIO registers
 * @param   zeroes: bit mask specifing pins to be cleared
 * @param   ones:   bit mask specifing pins to be set
 *
 * @note    when a pin is specified in both (zeroes and ones) it is
 *          cleared and then set
 */

inline static void
GPIO_WritePins(GPIO_t gpio, uint32_t zeroes, uint32_t ones) {
#ifdef GPIO_ALTERNATE_OPERATIONS
    gpio->DOUTCLR = zeroes;
    gpio->DOUTSET = ones;
#else
    gpio->DOUT = (gpio->DOUT&~zeroes)|ones;
#endif
}


/**
 * @brief  Toggle specified pins
 * @param  delay: bit mask specifing pins to be toggled
 */

inline static void
GPIO_TogglePins(GPIO_t gpio, uint32_t pins) {
#ifdef GPIO_ALTERNATE_OPERATIONS
    gpio->DOUTTGL = pins;
#else
    gpio->DOUT ^= pins;
#endif
}

inline static uint32_t
GPIO_ReadPins(GPIO_t gpio) {
    return gpio->DIN;
}

#endif // GPIO_H
