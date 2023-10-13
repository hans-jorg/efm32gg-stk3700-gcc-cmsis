/**
 * @file nand-flash.c
 * @brief Interface routine for the NAND256 Flash device in the EFM32GG-STK3700 board
 *
 * @note
 *      It will be used as the device interface for the FatFS middleware
 *
 *      It does not have Garbage Collection nor Wear-Leveling mechanims
 *
 * @version 1.0.0
 * Date:    7 October 2023
 *
 */
/*
 * @note  ìnout
 *
 * | MCU Pin | PCB Signal  |  NAND Signal | MCU Signal  | Description                   |
 * |---------|-------------|--------------|-------------|-------------------------------|
 * | PD13    | NAND_WP#    |    WP#       | GPIO_PD13   | Write Protect                 |
 * | PD14    | NAND_CE#    |    E#        | GPIO_PD14   | Chip Enable                   |
 * | PD15    | NAND_R/B#   |    R/B#      | GPIO_PD15   | Ready/Busy indicator          |
 * | PC1     | NAND_ALE    |    AL        | EBI_A24     | Address Latch Enable/A24      |
 * | PC2     | NAND_CLE    |    CL        | EBI_A25     | Command Latch Enable/A25      |
 * | PF8     | NAND_WE#    |    W#        | EBI_WEn     | Write Enable                  |
 * | PF9     | NAND_RE#    |    R#        | EBI_REn     | Read Enable                   |
 * | PE15    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #7                    |
 * | PE14    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #6                    |
 * | PE13    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #5                    |
 * | PE12    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #4                    |
 * | PE11    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #3                    |
 * | PE10    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #2                    |
 * | PE9     | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #1                    |
 * | PE8     | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #0                    |
 * | PB15    | NAND_PWR_EN |      -       | GPIO_PB15   | Power enable (TS5A3166 switch)|
 *  */
 *


#include <stdint.h>
#include <gpio.h>

#ifndef BIT
#define BIT(N)  (1U<<(N))
#endif

/**
 * @brief Pin Configuration for GPIO
 */
#define WP_GPIO                             GPIOD
#define WP_PINMASK                          BIT(13)
#define WP_PINMODE                          GPIO_MODE_PUSHPULL

#define CE_GPIO                             GPIOD
#define CE_PINMASK                          BIT(14)
#define CE_PINMODE                          GPIO_MODE_PUSHPULL

#define RB_GPIO                             GPIOD
#define RB_PINMASK                          BIT(15)
#define RB_PINMODE                          GPIO_MODE_INPUT

#define PWR_GPIO                            GPIOB
#define PWR_PINMASK                         BIT(15)
#define PWR_PINMODE                         GPIO_MODE_PUSHPULL


/**
 * @brief  Auxiliary routines.
 *
 * @note   They hid the signal polarity
 */
///@{

static inline void EnableNANDPower(void) {
    GPIO_WritePins(PWR_GPIO, 0, PWR_PINMASK);        // Set to High (positive logic)
}

static inline void DisableNANDPower(void) {
    GPIO_WritePins(PWR_GPIO, PWR_PINMASK, 0);        // Set to  Low (positive logic)
}

static inline void EnableNANDDevice(void) {
    GPIO_WritePins(E_GPIO, 0, E_PINMASK);            // Set to High
}

static inline void DisableNANDDevice(void) {
    GPIO_WritePins(E_GPIO, 0, E_PINMASK);            // Set to High
}

static inline void EnableWriteProtect(void) {
    GPIO_WritePins(WP_GPIO, WP_PINMASK, 0);         // Set to Low due to the negative logic
}
static inline void DisableWriteProtect(void) {
    GPIO_WritePins(WP_GPIO,  0, WP_PINMASK);        // Set to High due to the negative logic
}


static inline void EnableEBIClock(void) {
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_EBI;
}


static inline void ConfigGPIOPins(void) {

    GPIO_ConfigPins(WP_GPIO, WP_PINMASK, WP_PINMODE);
    GPIO_ConfigPins(E_GPIO,  E_PINMASK,  E_PINMODE);
    GPIO_ConfigPins(PWR_GPIO, PWR_PINMASK, PWR_PINMODE);
    GPIO_ConfigPins(RB_GPIO, RB_PINMASK, RB_PINMODE);

    // Is this meaningfull?
    EnableWriteProtect();
    EnableNANDPower();
    EnableNANDDevice();


}




static inline void ConfigEBIPins(void) {

    EnableEBIClock(); // Just in case

    EBI->POLARITY |= EBI_POLARITY_WEPOL|EBI_POLARITY_REPOL;
    EBI->ROUTE = (EBI->ROUTE&~(EBI_ROUTE_LOCATION_Msk|EBI_ROUTE_APEN_Msk|EBI_ROUTE_ALB_Msk))
                 |(EBI_ROUTE_LOCATION_LOC0|EBI_ROUTE_APEN_A26EBI_ROUTE_ALB_A24)
                 |(EBI_ROUTE_NANDPEN);

    EBI->NANDCTRL = (EBI_NANDTRL_BANKSEL_Msk)
                    |EBI_NANDCTRL_BANKSEL_BANK0|EBI_NANDCTRL_EN);


}

///@}

/**
 * @brief  Initialize NAND device including EBI
 *
 * @return 0 if OK, negative in case of error
 */

int NAND_Init(void) {

    EnableEBIClock();

    ConfigGPIOPins();
    ConfigEBIPins();



    EnableNandPower();
    EnableNANDDevice();

}


int NAND_Write(uint32_t address, uint8_t *data, uint32_t size);
int NAND_Read(uint32_t address, uint8_t *data, uint32_t size);
