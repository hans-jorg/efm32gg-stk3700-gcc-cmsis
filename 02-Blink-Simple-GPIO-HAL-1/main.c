/**
 * @file    main.c
 * @brief   Simple LED Blink Demo for EFM32GG_STK3700
 * @version 1.1
 *
 * @note    Just blinks the LEDs of the STK3700
 *
 * @note    LEDs are on pins 2 and 3 of GPIO Port E
 *
 * @note    It uses a primitive delay mechanism. Do not use it.
 *
 * @note    It uses a GPIO HAL.
 *
 * @author  Hans
 * @date    01/09/2018
 */


#include <stdint.h>
#include "em_device.h"


#define BIT(N) (1U<<(N))

// LEDs are on PE
#define LED1 BIT(2)
#define LED2 BIT(3)

#define DELAYVAL 1



/**
 * @brief  Quick and dirty delay function
 * @param  delay: specified delay (not calibrated)
 * @note   Do not use this in a production code
 */

void Delay(uint32_t delay) {
volatile uint32_t counter;
int i;

    for(i=0;i<delay;i++) {
        counter = 1000000;
        while( counter ) counter--;
    }
}

/**
 * @brief   GPIO HAL for EFM32GG
 *
 *
 *
 * @note   When set, use DOUTCLR and DOUTSET for output
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


void
GPIO_ConfigPins(GPIO_t gpio, uint32_t pins, uint32_t mode) {
const uint32_t mask = 0xF;  // Fields are 4 bits wide
int i;
uint32_t mbit;


    mbit = 0x0001; // Bit 0 set
    /* Configure low order bits/pins  0-7 */
    for(i=0;i<32;i+=4) {
        if( (pins&mbit) != 0 ) {
            gpio->MODEL &= ~(mask<<i);
            gpio->MODEL |= mode<<i;
        }
        mbit <<= 1;
    }
    /* Configure High order bits/pins 8-15 */
    for(i=0;i<32;i+=4) {
        if( (pins&mbit) != 0 ) {
            gpio->MODEH &= ~(mask<<i);
            gpio->MODEH |= mode<<i;
        }
        mbit <<= 1;
    }

}

/**
 * @brief   Configure pins for input or output
 * @param   gpio:    pointer to gpio registers
 * @param   inputs:  bit mask containing 1 in the position to be configured as input
 * @param   outputs: bit mask containing 1 in the position to be configured as output
 */

void
GPIO_Init(GPIO_t gpio, uint32_t inputs, uint32_t outputs) {


    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO

    GPIO_ConfigPins(gpio,outputs,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(gpio,inputs,GPIO_MODE_INPUT);       // Safer when both specified

}

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



/**
 * @brief  Main function
 *
 * @note   Using default clock configuration
 * @note   HFCLK = HFRCO
 * @note   HFCORECLK = HFCLK
 * @note   HFPERCLK  = HFCLK
 */

int main(void) {

    /* Configure Pins in GPIOE */
    GPIO_Init(GPIOE,0,LED1|LED2);       // LED pins configured as output

    GPIO_WritePins(GPIOE,LED1|LED2,0);  // Turn Off LEDs

    /* Blink loop */
    while (1) {

        Delay(DELAYVAL);
        GPIO_TogglePins(GPIOE,LED1);            // Toggle LED2

        Delay(DELAYVAL);
        GPIO_TogglePins(GPIOE,LED2);            // Toggle LED2

        Delay(DELAYVAL);
        GPIO_WritePins(GPIOE,0,LED1|LED2);      // Turn On All LEDs

    }
}
