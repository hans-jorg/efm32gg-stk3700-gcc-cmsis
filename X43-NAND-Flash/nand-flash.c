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
 *
 * @note https://catonmat.net/low-level-bit-hacks
 *       https://graphics.stanford.edu/~seander/bithacks.html
 */


#include <em_device.h>
#include <stdint.h>
#include "gpio.h"

#ifndef BIT
#define BIT(N)  (1U<<(N))
#endif

/**
 * @brief Pin Configuration for GPIO
 */
///@{
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
///@}


/// The pins below will be controlled by EBI
///@{
#define AD_GPIO                             GPIOE
#define AD_PINS                             ( BIT(15)|BIT(14)|BIT(13)|BIT(12) \
                                             |BIT(11)|BIT(10)|BIT(9)|BIT(8)   \
                                            )

#define ALE_GPIO                            GPIOC
#define ALE_PIN                             BIT(1)

#define CLE_GPIO                            GPIOC
#define CLE_PIN                             BIT(2)

#define WE_GPIO                             GPIOF
#define WE_PIN                              BIT(8)

#define RE_GPIO                             GPIOF
#define RE_PIN                              BIT(9)
///@}


/// Setting timing parameters (unit is HFPERCLK period)
///@{
#define ADDR_SETUPTIME                  (0)
#define ADDR_HOLDTIME                   (0)
#define RD_SETUPTIME                    (0)
#define RD_HOLDTIME                     (1)
#define RD_STROBETIME                   (2)
#define WR_SETUPTIME                    (0)
#define WR_HOLDTIME                     (1)
#define WR_STROBETIME                   (2)
///@}


/// NAND Flash commands
///@{
#define CMD_READ_A                      (0x00)
#define CMD_READ_B                      (0x01)
#define CMD_READ_C                      (0x50)
#define CMD_READ_SIGNATURE              (0x90)
#define CMD_READ_STATUS                 (0x70)
#define CMD_PROGRAM_1                   (0x80)
#define CMD_PROGRAM_2                   (0x10)
#define CMD_COPY_BACK_1                 (0x00)
#define CMD_COPY_BACK_2                 (0x8A)
#define CMD_COPY_BACK_3                 (0x10)
#define CMD_BLOCK_ERASE_1               (0x60)
#define CMD_BLOCK_ERASE_2               (0xD0)
#define CMD_RESET                       (0xFF)
///@}

/**
 * @brief  Status bits of Status Data
 */
///@{
#define STATUS_WP                       BIT(7)
#define STATUS_READY                    BIT(6)
#define STATUS_ERROR                    BIT(0)
///@}


/**
 * @brief  Adresses used to access NAND Flash
 */
///@{
static       uint8_t * const pntData     = (uint8_t *) 0x80000000;
static       uint8_t * const pntAddress  = (uint8_t *) 0x81000000;
static       uint8_t * const pntCommand  = (uint8_t *) 0x82000000;
// To make write to or read from NAND Flash easier
#define NAND_DATA       *pntData
#define NAND_ADDRESS    *pntData
#define NAND_COMMAND    *pntCommand
///@}

/**
 *  @brief  Spare Area (Area C)
 */
///@{
#define SPARESIZE      (16)
#define ERRORBYTE      (5)
#define OKVALUE        '\xFF'

static uint8_t spare[SPARESIZE];
///@}
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
    GPIO_WritePins(CE_GPIO, 0, CE_PINMASK);            // Set to High
}

static inline void DisableNANDDevice(void) {
    GPIO_WritePins(CE_GPIO, 0, CE_PINMASK);            // Set to High
}

static inline void EnableWriteProtect(void) {
    GPIO_WritePins(WP_GPIO, WP_PINMASK, 0);         // Set to Low due to the negative logic
}
static inline void DisableWriteProtect(void) {
    GPIO_WritePins(WP_GPIO,  0, WP_PINMASK);        // Set to High due to the negative logic
}
///@}

/**
 *  @brief  Enable EBI clock
 */
static inline void EnableEBIClock(void) {
    CMU->HFPERCLKDIV  |= CMU_HFPERCLKDIV_HFPERCLKEN;// Enable HFPERCLK
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_EBI;      // Enable EBI Clock
}

/**
 * @brief  Configure GPIO pins used for the NAND interface
 *
 * @note   Only the pins directly controlled thru GPIO are configured here. The pins directly
 *         controlled by EBI are configured elsewhere
 *
 */
static inline void ConfigGPIOPins(void) {

    GPIO_ConfigPins(WP_GPIO, WP_PINMASK, WP_PINMODE);
    GPIO_ConfigPins(CE_GPIO,  CE_PINMASK,  CE_PINMODE);
    GPIO_ConfigPins(PWR_GPIO, PWR_PINMASK, PWR_PINMODE);
    GPIO_ConfigPins(RB_GPIO, RB_PINMASK, RB_PINMODE);

    // The default values for output pins
    EnableWriteProtect();
    EnableNANDPower();
    EnableNANDDevice();
}



/**
 *  @brief  Configure pins directly controlled by EBI
 *
 *  @note   Pins are configured as Push-Pull before enabling EBI.
 *          This is done in the code example from SiLabs
 */

static inline void ConfigEBIPins(void) {

    // Configure pins before commit them to EBI
    GPIO_ConfigPins(AD_GPIO,AD_PINS,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(ALE_GPIO,ALE_PIN,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(CLE_GPIO,CLE_PIN,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(WE_GPIO,WE_PIN,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(RE_GPIO,RE_PIN,GPIO_MODE_PUSHPULL);

    // Enable clock for the EBI module (just in case)
    EnableEBIClock();

    // Enable independent timing. Just in case. Only needed when more than on bank is used */
    EBI->CTRL |= EBI_CTRL_ITS;

    // Configure mode
    EBI->CTRL = (EBI->CTRL&~(_EBI_CTRL_MODE_MASK|_EBI_CTRL_NOIDLE_MASK))
                |EBI_CTRL_MODE_D8A8
                |EBI_CTRL_NOIDLE
                |EBI_CTRL_BANK0EN;

    // Configure pins use directly by EBI
    EBI->ROUTE =  (EBI->ROUTE&~(_EBI_ROUTE_LOCATION_MASK|_EBI_ROUTE_APEN_MASK|_EBI_ROUTE_ALB_MASK))
                 |(EBI_ROUTE_LOCATION_LOC0  // Example uses LOC1 !?
                   |EBI_ROUTE_APEN_A26
                   |EBI_ROUTE_ALB_A24
                   |EBI_ROUTE_NANDPEN
                   |EBI_ROUTE_EBIPEN //?????
                  );


    // Configure polarities of ALE, WE and RE signals
    EBI->POLARITY &= ~(EBI_POLARITY_WEPOL
                      |EBI_POLARITY_REPOL
                      |EBI_POLARITY_ALEPOL);
}



void ConfigEBI(void) {

    EBI->ADDRTIMING =  (ADDR_HOLDTIME<<_EBI_ADDRTIMING_ADDRHOLD_SHIFT)
                      |(ADDR_SETUPTIME<<_EBI_ADDRTIMING_ADDRSETUP_MASK);
    EBI->RDTIMING   =  (RD_HOLDTIME<<_EBI_RDTIMING_RDHOLD_SHIFT)
                      |(RD_SETUPTIME<<_EBI_RDTIMING_RDSETUP_SHIFT)
                      |(RD_STROBETIME<<_EBI_RDTIMING_RDSTRB_SHIFT);
    EBI->WRTIMING   =  (WR_HOLDTIME<<_EBI_WRTIMING_WRHOLD_SHIFT)
                      |(WR_SETUPTIME<<_EBI_WRTIMING_WRSETUP_SHIFT)
                      |(WR_STROBETIME<<_EBI_WRTIMING_WRSTRB_SHIFT);

    // Pagemode and prefetch should be enabled? NO!!!! See example
    // Write buffer should be enabled?

    // Start EBI NAND controller
    EBI->NANDCTRL = (EBI->NANDCTRL&~(_EBI_NANDCTRL_BANKSEL_MASK|EBI_NANDCTRL_EN))
                     |EBI_NANDCTRL_BANKSEL_BANK0
                     |EBI_NANDCTRL_EN;

}

/**
 *  @brief  Get ECC Error Information
 *
 *  @param  w: ECC info
 *
 *  @returns the index of the bit in error
 *
 *  @note   The input word consists of 16 pairs of complementary bit.
 *
 *    | P32768 | P32768' | P16384 |  P16384' | ...| P4 | P4' | P2 | P2' | P1 | P1' |
 *
 *  @note
 *          When the pair is not complementary, it indicates that an error or multiples errors
 *          occurred.
 *
 *  @note
 *         |  Data size  |    ECC bits    |
 *         |-------------|----------------|

 *         |     64      |      18        |
 *         |    128      |      20        |
 *         |    256      |      22        |
 *         |    512      |      24        |
 *         |   1024      |      26        |
 *         |   2048      |      28        |
 *         |   4096      |      30        |
 *         |   8192      |      32        |
 *
 * @note To correct use the index. The 3 lowest order bit gives the bit index (0 to 7)
 *       The 19 high order bit give the byte index.
 *       Data[index>>3] ^= (1<<(index&0x7));
 *
 */
#define ECCMASK 0x00FFFFFFU

#define ISPOWER2(W)     (((W)&(W-1))==0)


typedef enum {
    NAND_NOERROR,
    NAND_ECCERROR,
    NAND_ONEERROR,
    NAND_MULTIPLEERROR
} NAND_ErrorType;


NAND_ErrorType getECCerror(uint32_t w, uint32_t *pindex) {
const uint32_t pmask = 0xAAAAAAAAU&ECCMASK;
const uint32_t ptickmask = pmask>>1;
uint32_t wp;
uint32_t index;
uint32_t m;

    // Mask out irrelevant bits. For 512 bytes, only 24 bits are needed
    w &= ECCMASK;

    if( w == 0 )
        return NAND_NOERROR;

    // If only one bit is set, error in the ECC
    if( ISPOWER2(w) )
        return NAND_ECCERROR;

    // Only Pn. All Pntick are zeroed
    wp=w&pmask;
    // find wether or not the pairs are complementar
    if(  ((wp>>1)  ^ (w&ptickmask)) != ptickmask )
        return NAND_MULTIPLEERROR;

/* In wp,
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
   0  0  0  0  0  0  0  0  P  0  P  0  P  0  P  0  P  0  P  0  P  0  P  0  P  0  P  0  P  0  P  0
                           2     1     5     2     1     6     3     1     8     4     2     1
                           0     0     1     5     2     4     2     6
                           4     2     2     6     8
                           8     4
   That is, every event bit is 0
   */

    m = 1;
    index = 0;
    while( wp ) {
        if( (wp&2)!=0 )
            index |= m;
        m <<= 1;
        wp >>= 2;
    }
    *pindex = index;

    return NAND_ONEERROR;
}

/**
 * @brief  Initialize NAND device including EBI
 *
 * @return 0 if OK, negative in case of error
 */

int NAND_Init(void) {

    EnableEBIClock();

    ConfigGPIOPins();
    ConfigEBIPins();

    ConfigEBI();

    EnableNANDPower();
    EnableNANDDevice();


    return 0;

}


/**
 * @/**
  * @brief   NAND_ReadPage
  *
  * @note    Write a page (512 bytes) into NAND device starting at given page address
  */
int NAND_WritePage(uint32_t pageaddr, uint8_t *data) {

    DisableWriteProtect();

    EnableWriteProtect();

    return 0;
}


/**
 * @/**
  * @brief   NAND_ReadPage
  *
  * @note    Read a page (512 bytes from NAND device
  */
int NAND_ReadPage(uint32_t pageaddr, uint8_t *data) {


    return 0;
}
