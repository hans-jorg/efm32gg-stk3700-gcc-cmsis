/**
 * @file  nand-flash.c
 * @brief Interface routine for the NAND256-A Flash device in the
 *        EFM32GG-STK3700 board
 *
 * @note  The Flash device depends on the board version
 *          * BRD2200A: NAND256W3A
 *          * BRD2200C: Windbond W29N01HV.
 *
 * @note  The most important characteristics are shown in the table below.
 *
 *  | Parameter              |  Value NAND256W3A  | Value W29N01HV   | Unit  |
 *  |------------------------|--------------------|------------------|-------|
 *  | Memory size            |       256          |        1024      | Mbit  |
 *  | Memory size (Mbytes)   |        32          |         128      | MBytes|
 *  | Memory size (bytes)    |  33554432          |                  | bytes |
 *  | Address bits           |        25          |                  | bits  |
 *  | Bus width              |         8          |           8      | bits  |
 *  | Block size             |        32          |          64      | pages |
 *  | # Blocks               |      2048          |        1024      | blocks|
 *  | #Program/Erase cycles  |   100.000          |     100.000      | cycles|
 *  | #Data Retention        |        10          |          10      | years |
 *  | Page size              |    512+16          |     2048+64      | bytes |
 *  | Block size             | 16384+512          |     128K+4K      | bytes |
 *  | Spare size             |        16          |          64      | bytes |
 *  | Block erase time       |         2          |        2-10      | ms    |
 *  | Random access          |        10          |                  | us    |
 *  | Sequential access      |        50          |                  | ns    |
 *  | Page program time      |       200          |     250-700      | us    |
 *  | Minimum # Valid blocks |      2008          |        1004       | blocks|
 *
 * @note
 *      The Flash device is powered thru a TS4A3166 or a SIP32431 switch,
 *      controlled by NAND_PWR_EN.
 *
 * @note
 *      It can be used as the device interface for the FatFS middleware
 *      It does not have Garbage Collection nor Wear-Leveling mechanims
 *
 * @version 1.0.0
 * Date:    28 April 2026
 *
 *
 *
 * @note  Pìnout
 *
 * | MCU Pin | PCB Signal  | Flash Signal | MCU Signal  | Description          |
 * |---------|-------------|--------------|-------------|----------------------|
 * | PD13    | NAND_WP#    |    WP#       | GPIO_PD13   | Write Protect        |
 * | PD14    | NAND_CE#    |    E#        | GPIO_PD14   | Chip Enable          |
 * | PD15    | NAND_R/B#   |    R/B#      | GPIO_PD15   | Ready/Busy indicator |
 * | PC1     | NAND_ALE    |    AL        | EBI_A24     | Address Latch Enable |
 * | PC2     | NAND_CLE    |    CL        | EBI_A25     | Command Latch Enable |
 * | PF8     | NAND_WE#    |    W#        | EBI_WEn     | Write Enable         |
 * | PF9     | NAND_RE#    |    R#        | EBI_REn     | Read Enable          |
 * | PE15    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #7           |
 * | PE14    | NAND_IO6    |    I/O6      | EBI_AD6     | I/O bit #6           |
 * | PE13    | NAND_IO5    |    I/O5      | EBI_AD5     | I/O bit #5           |
 * | PE12    | NAND_IO4    |    I/O4      | EBI_AD4     | I/O bit #4           |
 * | PE11    | NAND_IO3    |    I/O3      | EBI_AD3     | I/O bit #3           |
 * | PE10    | NAND_IO2    |    I/O2      | EBI_AD2     | I/O bit #2           |
 * | PE9     | NAND_IO1    |    I/O1      | EBI_AD1     | I/O bit #1           |
 * | PE8     | NAND_IO0    |    I/O0      | EBI_AD0     | I/O bit #0           |
 * | PB15    | NAND_PWR_EN |      -       | GPIO_PB15   | Power enable/switch  |
 *
 *
 * @note From RM 14.3.13 "For CEDC NAND Flash the shared EBI_REn and EBI_WEn
 * pins can be used instead of the dedicated EBI_NANDREn  and EBI_NANDWEn pins
 *
 * @note https://catonmat.net/low-level-bit-hacks
 *       https://graphics.stanford.edu/~seander/bithacks.html
 *
 */


#include <stdint.h>
#include "gpio3.h"
#include "nand-flash.h"
#include "em_device.h"

#ifndef BIT
#define BIT(N)  (1U<<(N))
#endif


/**
 *  @brief  Which parity table to use.
 *
 *  @note   There are two: one faster but demands a 256-byte table.
 *          The other is somewhat slower but only uses 32 bytes
 */
#define PARITYBYTETABLE

/**
 *  @brief  The EFM32GG hardware can calculate the hamming encoding. When this flag is
 *          defined, both the software implementation and hardware implementation are
 *          used and the results are compared
 */
#define VERIFYDEVICEHAMMING

/**
 *  @brief  NAND256 address parameters
 *
 *  @note
 *
 *  | Address bits |  Description     |
 *  |--------------|------------------|
 *  |     A0-A7    | Column Address   |
 *  |     A9-A26   | Page Address     |
 *  |     A9-A13   | Address in Block |
 *  |     A14-A26  | Block Address    |
 *  |     A8       | Address Half Page|
 *
 *  @note For x8 devices A8 is set using the command for pointing to
 *        A or B halfpage
 *  @note For x16 devices it is a Don’t Care
 *
 *
 *  AAA AAAA AAAA AAAA AAAA AAAA AAAA
 *  222 2222 1111 1111 1198 7654 3210
 *  654 3210 9876 5432 10.. .... ....
 *  ---------------------------------
 *  333 2222 2222 1111 1111 0000 0000   Byte #
 *  ---------------------------------   Address mask
 *  000 XXXX XXXX XXXX XXXY XXXX XXXX   16 MB (=128 Mb)
 *  00X XXXX XXXX XXXX XXXY XXXX XXXX   32 MB (=256 Mb)
 *  0XX XXXX XXXX XXXX XXXY XXXX XXXX   64 MB (=512 Mb) page address
 *  XXX XXXX XXXX XXXX XXXY XXXX XXXX   128 MB (=1024 Mb) page address
 *  ---------------------------------   Mask
 *  000 0000 0000 0000 000X XXXX XXXX   Column address mask
 *  *** XXXX XXXX XXXX XXX0 0000 0000   Row address
 *  *** XXXX XXXX XX00 0000 0000 0000   Block address
 *  000 0000 0000 00XX XXX0 0000 0000   Page displacement inside the block
 *
 *  Addressing cycles
 *  #1     A7-0
 *  #2     A16-9   (No A8!!!!!!!!)
 *  #3     A24-17
 *  #4     A26-25  (remaining as 0)
 *
 *  @note  A8 is defined by the command READ A (A8=0)
 *         and READ B (A8=1)
 *
 *  @note  They should be defined as function of the parameters in nand-flash.h
 */
///@{
#define HALFPAGEMASK                (0xFF)
#define PAGEMASK                    (0x1FF)
#define BLOCKMASK                   (0x3FFF)
#define COPYREGIONMASK              (0x1000000)
#define ADDRESSMASK                 NAND_MAXADDRESS
#define GETCOLUMNADDRESS(A)         ((A)&0xPAGEMASK)

#define IsPageAddress(A)            (((A)&PAGEMASK)==0)
#define IsBlockAddress(A)           (((A)&BLOCKMASK)==0)
///@}

/**
 * @brief Pin Configuration for GPIO
 *
 * @note
 *        WP:  Output. Write protect
 *        CE:  Output. Chip enable
 *        RB:  Ready=1/Busy=0
 *        PWR: Enable power to the nand device
 */
///@{
#define WP_GPIO                             GPIOD
#define WP_PINMASK                          BIT(13)
#define WP_PINMODE                          GPIO_MODE_PUSHPULL

#define CE_GPIO                             GPIOD
#define CE_PINMASK                          BIT(14)
#define CE_PINMODE                          GPIO_MODE_PUSHPULL

#define PWR_GPIO                            GPIOB
#define PWR_PINMASK                         BIT(15)
#define PWR_PINMODE                         GPIO_MODE_PUSHPULL

#define RB_GPIO                             GPIOD
#define RB_PINMASK                          BIT(15)
#define RB_PINMODE                          GPIO_MODE_INPUT
///@}


/**
 *  @brief  The pins below are be controlled by EBI
 *
 *  @note
 *     | Signal | EBI signal |  Port pin   |  Description                 |
 *     |--------|----------- |-------------|------------------------------|
 *     | IO0-7  |  AD00-07   | PE8-15      |  Address/Data pins           |
 *     | ALE    |  A24       |   PC1       |  Address Latch Enable        |
 *     | CLE    |  A25       |   PC2       |  Command Latch Enable        |
 *     | WE     |  WEn       |   PF8       |  Write enable                |
 *     | RE     |  REn       |   PF9       |  Read enable                 |
 *
 *  @note  NAND_WE and NAND_RE are not used. WEn and REn are used instead!!
 *  @note  PF8 is only used as WE when LOCATION is configured as LOC1
 */
///@{
#define AD_GPIO                             GPIOE
#define AD_PINMASK                          ( BIT(15)|BIT(14)|BIT(13)|BIT(12) \
                                             |BIT(11)|BIT(10)|BIT(9)|BIT(8)   \
                                            )

#define ALE_GPIO                            GPIOC
#define ALE_PINMASK                         BIT(1)

#define CLE_GPIO                            GPIOC
#define CLE_PINMASK                         BIT(2)

#define WE_GPIO                             GPIOF
#define WE_PINMASK                          BIT(8)

#define RE_GPIO                             GPIOF
#define RE_PINMASK                          BIT(9)
///@}


/// Setting timing parameters (unit is HFPERCLK period)
///@{
#define NANO_DELAY                         (20)
#define ADDR_SETUPTIME                      (0)
#define ADDR_HOLDTIME                       (0)
#define RD_SETUPTIME                        (0)
#define RD_HOLDTIME                         (1)
#define RD_STROBETIME                       (2)
#define WR_SETUPTIME                        (0)
#define WR_HOLDTIME                         (1)
#define WR_STROBETIME                       (2)
///@}

///@{
/*
 *  @note  These symbols are used as place holders.
 *         They must be different from the any code used by the DEVICE
 */
#define FULLADDRESS                          (0xFA)
#define SHORTADDRESS                         (0x5A)
///@}

typedef struct {
    uint8_t     n;
    uint8_t    *v;
} Command_TypeDef;

/**
 *  @brief  NAND Command Table
 *
 *  @note   It handles the address too.
 *  @note   There are two form of addresses to be sent.
 *          * 4 bytes (32 bit) address for read/program page
 *          * 3 bytes (24 bit) address for block erase
 *
 *  @note   There are two place holders in the table.
 *          * FULLADDRESS
 *          * SHORTADDRESS
 *          They shows how to send the address. The short address
 *          is used to address a block. Important note: The A8
 *          bit is not sent, because the command used already informs
 *          its value. *Read A* means A8=0, *Read B* means A8=1. For
 *          *Read C* it is a don't care.
 *
 *  @note   They must be set to values not used by the NAND Flash
 *          as command
 *
 *  @note   Command table used as parameter to send command
 *
 *  @note   These are the indexes for the table below
 *
 *  @note   Maybe a enum is a better idea
 */
#define CMD_READA                               0
#define CMD_READB                               1
#define CMD_READC                               2
#define CMD_READSIGNATURE                       3
#define CMD_READSTATUS                          4
#define CMD_PROGRAMA                            5
#define CMD_PROGRAMB                            6
#define CMD_PROGRAMC                            7
#define CMD_COPYBACK1                           8
#define CMD_COPYBACK2                           9
#define CMD_ERASEBLOCK                         10
#define CMD_RESET                              11
#define CMD_CONFIRM                            12

/// NAND Flash commands table
const Command_TypeDef CommandList[] = {
    {   2, (uint8_t []) {0x00, FULLADDRESS } },          //  0: Read A
    {   2, (uint8_t []) {0x01, FULLADDRESS } },          //  1: Read B
    {   2, (uint8_t []) {0x50, FULLADDRESS } },          //  2: Read C/Spare
    {   1, (uint8_t []) {0x90} },                        //  3: Read Signature
    {   1, (uint8_t []) {0x70} },                        //  4 Read Status
    {   3, (uint8_t []) {0x00, 0x80, FULLADDRESS} },     //  5: Program A
    {   3, (uint8_t []) {0x01, 0x80, FULLADDRESS} },     //  6: Program B
    {   3, (uint8_t []) {0x50, 0x80, FULLADDRESS} },     //  7: Program C
    {   3, (uint8_t []) {0x00, FULLADDRESS, 0x8A} },     //  8: Copy back 1
    {   2, (uint8_t []) {FULLADDRESS, 0x10} },           //  9: Copy back 2
    {   3, (uint8_t []) {0x60, SHORTADDRESS, 0xD0} },    // 10: Erase block
    {   1, (uint8_t []) {0xFF} },                        // 11: Reset
    {   1, (uint8_t []) {0x10} }                         // 12: Confirm
};

///@}


/**
 *  @brief  Signature
 */
///@{
#define MFG_CODE                            0x20
#define DEV_CODE                            0x75


///@}
/**
 * @brief  Addresses used to access NAND Flash
 */
///@{
static  uint8_t *const pntData     = (uint8_t *) 0x80000000;
static  uint8_t *const pntAddress  = (uint8_t *) 0x81000000;
static  uint8_t *const pntCommand  = (uint8_t *) 0x82000000;
// To make it easier to write to or read from NAND Flash
#define NAND_DATA       *pntData
#define NAND_ADDRESS    *pntAddress
#define NAND_COMMAND    *pntCommand
///@}

//static uint8_t spare[NAND_SPARESIZE];

/*
 *  @brief  Nano delay
 */
static void
nano_delay(uint32_t n) {
    while(n--) {
        __NOP();
    }
}

/*********************** Command routine **************************************/

/**
 *  @brief  Send Command
 *
 *  @param  cmd:index of command as in the CommandList
 */
static NAND_RC
SendCommand(int32_t cmd, uint32_t address) {
    for(int32_t i=0;i<CommandList[cmd].n;i++) {
        uint8_t command =  CommandList[cmd].v[i];
        if( command == FULLADDRESS ) {
            NAND_ADDRESS = address&0xFF;
            address >>= 9;   // No A8
            NAND_ADDRESS = address&0xFF;
            address >>= 8;
            NAND_ADDRESS = address&0xFF;
            // Only for 512 MB or larger devices
            // address >>= 8;
            // NAND_ADDRESS = address&0xFF;
        } else if ( command == SHORTADDRESS ) {
            address >>= 9;
            NAND_ADDRESS = address&0xFF;
            address >>= 8;
            NAND_ADDRESS = address&0xFF;
            // Only for 512 MB or larger devices
            // address >>= 8;
            // NAND_ADDRESS = address&0xFF;
        } else {
            NAND_COMMAND = command;
        }

    }

    NAND_RC rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/*********************** Pin control routines *********************************/

///@{
/**
 * @brief  Auxiliary routines (not exported) to control Power Enable, Chip
 *         Enable and Write Protection
 *
 * @note   They are controlled explicitly using the GPIO
 *
 * @note   They hid the signal polarity
 */

static inline void
EnablePWR(void) {
    // Set to High (positive logic)
    GPIO_SetPins(PWR_GPIO, PWR_PINMASK);
    NAND_WaitUntilReadyPin();
}

static inline void
DisablePWR(void) {
    // Set to  Low (positive logic)
    GPIO_ClearPins(PWR_GPIO, PWR_PINMASK);
}

static inline void
EnableCE(void) {
    // Set to Low (Active Low)
    GPIO_ClearPins(CE_GPIO, CE_PINMASK);
}

static inline void
DisableCE(void) {
    // Set to High (Active Low)
    GPIO_SetPins(CE_GPIO, CE_PINMASK);
}

static inline void
EnableWP(void) {
    // Set to Low due to the negative logic
    GPIO_ClearPins(WP_GPIO, WP_PINMASK);
}

static inline void
DisableWP(void) {
    // Set to High due to the negative logic
    GPIO_SetPins(WP_GPIO, WP_PINMASK);
}

// For debug mainly
void
NAND_EnableWriteProtect(void) {
    // Enable Write Protect using the WP pin
    EnableWP();
}

// For debug mainly
void
NAND_DisableWriteProtect(void) {
    // Disable Write Protect using the WP pin
    DisableWP();
}
///@}

/*********************** Initialization routines ******************************/

/**
 *  @brief  Enable EBI clock
 */
static inline void
EnableEBIClock(void) {
    CMU->HFPERCLKDIV  |= CMU_HFPERCLKDIV_HFPERCLKEN;// Enable HFPERCLK
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_EBI;      // Enable EBI Clock
}

/**
 * @brief  Configure GPIO pins used for the NAND interface
 *
 * @note   Only the pins directly controlled thru GPIO are configured here.
 *         The pins directly controlled by EBI are configured elsewhere
 *
 */
static inline void
ConfigGPIOPins(void) {

    // Write Protect, Chip Enable and Power Enable output pins
    GPIO_ConfigPins(WP_GPIO, WP_PINMASK, WP_PINMODE);
    GPIO_ConfigPins(CE_GPIO,  CE_PINMASK,  CE_PINMODE);
    GPIO_ConfigPins(PWR_GPIO, PWR_PINMASK, PWR_PINMODE);
    // Read/Busy input
    GPIO_ConfigPins(RB_GPIO, RB_PINMASK, RB_PINMODE);

    // The default values for output pins
    DisableCE();
    EnableWP();
    EnablePWR();
}

/**
 *  @brief  Configure pins directly controlled by EBI
 *
 *  @note   Pins are configured as Push-Pull before enabling EBI.
 *          This is done in the code example from SiLabs
 */
static inline void
ConfigEBIPins(void) {

    // Configure pins before commit them to EBI
    GPIO_ConfigPins(AD_GPIO,AD_PINMASK,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(ALE_GPIO,ALE_PINMASK,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(CLE_GPIO,CLE_PINMASK,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(WE_GPIO,WE_PINMASK,GPIO_MODE_PUSHPULL);
    GPIO_ConfigPins(RE_GPIO,RE_PINMASK,GPIO_MODE_PUSHPULL);

    // Initial values for data bus, write and read pins
    GPIO_ClearPins(AD_GPIO,AD_PINMASK);
    GPIO_SetPins(WE_GPIO,WE_PINMASK);
    GPIO_SetPins(RE_GPIO,RE_PINMASK);
    GPIO_ClearPins(CLE_GPIO,CE_PINMASK);
    GPIO_ClearPins(ALE_GPIO,ALE_PINMASK);

}

/**
 *  @brief  Configure the EBI
 *
 *  @note   Pins are configured before enabling the clock for EBI.
 *
 *  @note Initialization procedure for the STK3700 board as the example
 *        in SDK.
 *
 *  Configure pins including initial values
 *  Mode D8A8
 *  Active Low: WE, RE (ARDY, ALE, CS, BL are not used. Why configure them?)
 *  Disable BL(?)
 *  Enable NOIDLE
 *  Disable ARDY
 *  Disable ARDY timeout
 *  BANK0
 *  No Chip Select
 *  ADDR Setup and Strobe Cycles = 0
 *  Disable Half Cycle ALE
 *  Read Setup, Strobe, Hold = 0,2,1
 *  Disable Page Mode
 *  Disable Prefetch
 *  Disable Half Cycle REn
 *  Write Setup, Strobe, Hold = 0,2,1
 *  Enable Write Buffer
 *  Disable Half Cycle WEn
 *  A24
 *  A25
 *  Use Location 1
 *  Enable EBI
 *
 */

static inline void
ConfigEBI(void) {

    // Enable clock for the EBI module (just in case)
    EnableEBIClock();

    /* Do not enable independent timing.
     * It is only needed when more than one bank is used
     * When not set the values for Bank 0 is used for the other Banks.
     */
    //EBI->CTRL |= EBI_CTRL_ITS;

    // Configure polarity
    uint32_t polarity = EBI->POLARITY;
    polarity &= ~(EBI_POLARITY_WEPOL
                 |EBI_POLARITY_REPOL);
    // Why set polarity of pins not used?
    polarity &= ~(EBI_POLARITY_ALEPOL
                 |EBI_POLARITY_ARDYPOL
                 |EBI_POLARITY_CSPOL
                 |EBI_POLARITY_ARDYPOL);
    EBI->POLARITY = polarity;

    // Configure control register.
    // As now it can disturb configuration for other regions
    uint32_t ctrl = _EBI_CTRL_RESETVALUE;
    ctrl |=  EBI_CTRL_MODE_D8A8
            |EBI_CTRL_NOIDLE
            |EBI_CTRL_ARDYTODIS
            |EBI_CTRL_BANK0EN;     // Bank 0
    EBI->CTRL = ctrl;

    // Configure timing for address, read and write signals
    uint32_t addrtiming = _EBI_ADDRTIMING_RESETVALUE;
    addrtiming |=       (ADDR_HOLDTIME<<_EBI_ADDRTIMING_ADDRHOLD_SHIFT)
                       |(ADDR_SETUPTIME<<_EBI_ADDRTIMING_ADDRSETUP_MASK);
    EBI->ADDRTIMING = addrtiming;

    uint32_t rdtiming = _EBI_RDTIMING_RESETVALUE;
    rdtiming |=         (RD_HOLDTIME<<_EBI_RDTIMING_RDHOLD_SHIFT)
                      |(RD_SETUPTIME<<_EBI_RDTIMING_RDSETUP_SHIFT)
                      |(RD_STROBETIME<<_EBI_RDTIMING_RDSTRB_SHIFT);
    EBI->RDTIMING = rdtiming;

    uint32_t wrtiming = _EBI_WRTIMING_RESETVALUE;
    wrtiming =         (WR_HOLDTIME<<_EBI_WRTIMING_WRHOLD_SHIFT)
                      |(WR_SETUPTIME<<_EBI_WRTIMING_WRSETUP_SHIFT)
                      |(WR_STROBETIME<<_EBI_WRTIMING_WRSTRB_SHIFT);
    EBI->WRTIMING = wrtiming;

    // Configure pins use directly by EBI
    uint32_t route = EBI->ROUTE;
    route =  _EBI_ROUTE_RESETVALUE
                 |(EBI_ROUTE_LOCATION_LOC1  // Must be LOC1!!!
                  |EBI_ROUTE_APEN_A26
                  |EBI_ROUTE_ALB_A24
                  |EBI_ROUTE_EBIPEN        // Use WEn and REn
                  );
    EBI->ROUTE = route;

    // Start EBI NAND controller
    EBI->NANDCTRL = (EBI->NANDCTRL
                        &~(_EBI_NANDCTRL_BANKSEL_MASK|EBI_NANDCTRL_EN))
                         |(EBI_NANDCTRL_BANKSEL_BANK0|EBI_NANDCTRL_EN);
}

/**
 * @brief  Initialize NAND device including EBI
 *
 * @return 0 if OK, negative in case of error
 */

NAND_RC
NAND_Init(void) {

    ConfigGPIOPins();
    ConfigEBIPins();

    EnableEBIClock();
    ConfigEBI();

    EnablePWR();
    DisableCE();
    EnableWP();

    int32_t rc = NAND_Status();
    return rc; // NAND_Status
}

/*********************** ECC routines *************************************************/

/**
 *  @note  hamming 2D for 512-byte block
 *
 *  @note  It implements the block hamming encoding/decoding used in NAND flash
 *         devices.
 *  @note  It works on a array of 512 bytes called page and generates a three
 *         byte parity field.
 *  @note  It treats the page as an array of 512 rows (page size) and 8 columns
 *        (number of bits in a byte)
 *  @note  Hamming code can correct a 1-bit error and detect a 2-bit error.
 *         SECDED (Single Error Correction, Double Error Detection).
 *  @note  A description can be found in the Reference Manual for the EFM32
 *         Giant Gecko.
 *  @note  A linear approach needs only 13 bits (2^13 > 8*512+13+1)
 *         The condition is $$ 2^r >= m + r + 1 $$ where **m** is the block size
 *         and *r* the number of parity bits.
 *  @note  But to simplify hardware and processing, a 3-byte parity is used.
 *
 *  | Byte  |  ECC    | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
 *  |-------|---------|------|------|------|------|------|------|------|------|
 *  |Byte #2| ECC23-16| P2048| Q2048| P1024| Q1024| P512 | Q512 | P256 | Q256 |
 *  |Byte #1| ECC15-8 | P128 | Q128 |  P64 |  Q64 |  P32 |  Q32 |  P16 |  Q16 |
 *  |Byte #0| ECC7-0  |   P8 |   Q8 |   P4 |   Q4 |   P2 |   Q2 |   P1 |   Q1 |
 *
 *  Instead of using *Px'* as in the doc, the notation *Qx* is used
 *
 *  | Signal |  Expression                                                  |
 *  |--------|--------------------------------------------------------------|
 *  |  CP0   | d(0,0) XOR d(1,0) XOR d(2,0) XOR ..... XOR d(511,0)          |
 *  |   ...  | ...                                                          |
 *  |  CP7   | d(0,7) XOR d(1,7) XOR d(2,7) XOR ..... XOR d(511,7)          |
 *  |   ...  |
 */

/**
 *  @brief  Test if the parameter is a power of 2
 */
static inline int
ispower2(unsigned m ) {
    return (m&(m-1))==0;
}

#ifdef VERIFYDEVICEHAMMING
/**
 *  @brief  Parity table for a 8-bit byte
 *
 *  @note   There are two versions. One using a byte to store the parity bit
 *          i.e., only the 0 bit is used. It needs 256 bytes for the table.
 *          The other uses a 32-bit value to store 32 bits. It demands only 32 bytes.
 *          To get the parity bit for a byte n, first access the 32-bit value using
 *          n/32 as an index. Then get the corresponding bit by AND'ing with the
 *          mask 0x8000000>>(n%32). To avoid the division one can use n&0x1F to get
 *          the 32-bit value and 0x80000000>>(n&0x1F) to get the bit
 */

#ifdef PARITYBYTETABLE
/*
 *  @note   Only bit 0 is used for paritytable8
 */

/*********************** Status routines **************************************/

static uint8_t paritytable8[] = {
    0,   // 0b00000000 = 0x00
    1,   // 0b00000001 = 0x01
    1,   // 0b00000010 = 0x02
    0,   // 0b00000011 = 0x03
    1,   // 0b00000100 = 0x04
    0,   // 0b00000101 = 0x05
    0,   // 0b00000110 = 0x06
    1,   // 0b00000111 = 0x07
    1,   // 0b00001000 = 0x08
    0,   // 0b00001001 = 0x09
    0,   // 0b00001010 = 0x0A
    1,   // 0b00001011 = 0x0B
    0,   // 0b00001100 = 0x0C
    1,   // 0b00001101 = 0x0D
    1,   // 0b00001110 = 0x0E
    0,   // 0b00001111 = 0x0F
    1,   // 0b00010000 = 0x10
    0,   // 0b00010001 = 0x11
    0,   // 0b00010010 = 0x12
    1,   // 0b00010011 = 0x13
    0,   // 0b00010100 = 0x14
    1,   // 0b00010101 = 0x15
    1,   // 0b00010110 = 0x16
    0,   // 0b00010111 = 0x17
    0,   // 0b00011000 = 0x18
    1,   // 0b00011001 = 0x19
    1,   // 0b00011010 = 0x1A
    0,   // 0b00011011 = 0x1B
    1,   // 0b00011100 = 0x1C
    0,   // 0b00011101 = 0x1D
    0,   // 0b00011110 = 0x1E
    1,   // 0b00011111 = 0x1F
    1,   // 0b00100000 = 0x20
    0,   // 0b00100001 = 0x21
    0,   // 0b00100010 = 0x22
    1,   // 0b00100011 = 0x23
    0,   // 0b00100100 = 0x24
    1,   // 0b00100101 = 0x25
    1,   // 0b00100110 = 0x26
    0,   // 0b00100111 = 0x27
    0,   // 0b00101000 = 0x28
    1,   // 0b00101001 = 0x29
    1,   // 0b00101010 = 0x2A
    0,   // 0b00101011 = 0x2B
    1,   // 0b00101100 = 0x2C
    0,   // 0b00101101 = 0x2D
    0,   // 0b00101110 = 0x2E
    1,   // 0b00101111 = 0x2F
    0,   // 0b00110000 = 0x30
    1,   // 0b00110001 = 0x31
    1,   // 0b00110010 = 0x32
    0,   // 0b00110011 = 0x33
    1,   // 0b00110100 = 0x34
    0,   // 0b00110101 = 0x35
    0,   // 0b00110110 = 0x36
    1,   // 0b00110111 = 0x37
    1,   // 0b00111000 = 0x38
    0,   // 0b00111001 = 0x39
    0,   // 0b00111010 = 0x3A
    1,   // 0b00111011 = 0x3B
    0,   // 0b00111100 = 0x3C
    1,   // 0b00111101 = 0x3D
    1,   // 0b00111110 = 0x3E
    0,   // 0b00111111 = 0x3F
    1,   // 0b01000000 = 0x40
    0,   // 0b01000001 = 0x41
    0,   // 0b01000010 = 0x42
    1,   // 0b01000011 = 0x43
    0,   // 0b01000100 = 0x44
    1,   // 0b01000101 = 0x45
    1,   // 0b01000110 = 0x46
    0,   // 0b01000111 = 0x47
    0,   // 0b01001000 = 0x48
    1,   // 0b01001001 = 0x49
    1,   // 0b01001010 = 0x4A
    0,   // 0b01001011 = 0x4B
    1,   // 0b01001100 = 0x4C
    0,   // 0b01001101 = 0x4D
    0,   // 0b01001110 = 0x4E
    1,   // 0b01001111 = 0x4F
    0,   // 0b01010000 = 0x50
    1,   // 0b01010001 = 0x51
    1,   // 0b01010010 = 0x52
    0,   // 0b01010011 = 0x53
    1,   // 0b01010100 = 0x54
    0,   // 0b01010101 = 0x55
    0,   // 0b01010110 = 0x56
    1,   // 0b01010111 = 0x57
    1,   // 0b01011000 = 0x58
    0,   // 0b01011001 = 0x59
    0,   // 0b01011010 = 0x5A
    1,   // 0b01011011 = 0x5B
    0,   // 0b01011100 = 0x5C
    1,   // 0b01011101 = 0x5D
    1,   // 0b01011110 = 0x5E
    0,   // 0b01011111 = 0x5F
    0,   // 0b01100000 = 0x60
    1,   // 0b01100001 = 0x61
    1,   // 0b01100010 = 0x62
    0,   // 0b01100011 = 0x63
    1,   // 0b01100100 = 0x64
    0,   // 0b01100101 = 0x65
    0,   // 0b01100110 = 0x66
    1,   // 0b01100111 = 0x67
    1,   // 0b01101000 = 0x68
    0,   // 0b01101001 = 0x69
    0,   // 0b01101010 = 0x6A
    1,   // 0b01101011 = 0x6B
    0,   // 0b01101100 = 0x6C
    1,   // 0b01101101 = 0x6D
    1,   // 0b01101110 = 0x6E
    0,   // 0b01101111 = 0x6F
    1,   // 0b01110000 = 0x70
    0,   // 0b01110001 = 0x71
    0,   // 0b01110010 = 0x72
    1,   // 0b01110011 = 0x73
    0,   // 0b01110100 = 0x74
    1,   // 0b01110101 = 0x75
    1,   // 0b01110110 = 0x76
    0,   // 0b01110111 = 0x77
    0,   // 0b01111000 = 0x78
    1,   // 0b01111001 = 0x79
    1,   // 0b01111010 = 0x7A
    0,   // 0b01111011 = 0x7B
    1,   // 0b01111100 = 0x7C
    0,   // 0b01111101 = 0x7D
    0,   // 0b01111110 = 0x7E
    1,   // 0b01111111 = 0x7F
    1,   // 0b10000000 = 0x80
    0,   // 0b10000001 = 0x81
    0,   // 0b10000010 = 0x82
    1,   // 0b10000011 = 0x83
    0,   // 0b10000100 = 0x84
    1,   // 0b10000101 = 0x85
    1,   // 0b10000110 = 0x86
    0,   // 0b10000111 = 0x87
    0,   // 0b10001000 = 0x88
    1,   // 0b10001001 = 0x89
    1,   // 0b10001010 = 0x8A
    0,   // 0b10001011 = 0x8B
    1,   // 0b10001100 = 0x8C
    0,   // 0b10001101 = 0x8D
    0,   // 0b10001110 = 0x8E
    1,   // 0b10001111 = 0x8F
    0,   // 0b10010000 = 0x90
    1,   // 0b10010001 = 0x91
    1,   // 0b10010010 = 0x92
    0,   // 0b10010011 = 0x93
    1,   // 0b10010100 = 0x94
    0,   // 0b10010101 = 0x95
    0,   // 0b10010110 = 0x96
    1,   // 0b10010111 = 0x97
    1,   // 0b10011000 = 0x98
    0,   // 0b10011001 = 0x99
    0,   // 0b10011010 = 0x9A
    1,   // 0b10011011 = 0x9B
    0,   // 0b10011100 = 0x9C
    1,   // 0b10011101 = 0x9D
    1,   // 0b10011110 = 0x9E
    0,   // 0b10011111 = 0x9F
    0,   // 0b10100000 = 0xA0
    1,   // 0b10100001 = 0xA1
    1,   // 0b10100010 = 0xA2
    0,   // 0b10100011 = 0xA3
    1,   // 0b10100100 = 0xA4
    0,   // 0b10100101 = 0xA5
    0,   // 0b10100110 = 0xA6
    1,   // 0b10100111 = 0xA7
    1,   // 0b10101000 = 0xA8
    0,   // 0b10101001 = 0xA9
    0,   // 0b10101010 = 0xAA
    1,   // 0b10101011 = 0xAB
    0,   // 0b10101100 = 0xAC
    1,   // 0b10101101 = 0xAD
    1,   // 0b10101110 = 0xAE
    0,   // 0b10101111 = 0xAF
    1,   // 0b10110000 = 0xB0
    0,   // 0b10110001 = 0xB1
    0,   // 0b10110010 = 0xB2
    1,   // 0b10110011 = 0xB3
    0,   // 0b10110100 = 0xB4
    1,   // 0b10110101 = 0xB5
    1,   // 0b10110110 = 0xB6
    0,   // 0b10110111 = 0xB7
    0,   // 0b10111000 = 0xB8
    1,   // 0b10111001 = 0xB9
    1,   // 0b10111010 = 0xBA
    0,   // 0b10111011 = 0xBB
    1,   // 0b10111100 = 0xBC
    0,   // 0b10111101 = 0xBD
    0,   // 0b10111110 = 0xBE
    1,   // 0b10111111 = 0xBF
    0,   // 0b11000000 = 0xC0
    1,   // 0b11000001 = 0xC1
    1,   // 0b11000010 = 0xC2
    0,   // 0b11000011 = 0xC3
    1,   // 0b11000100 = 0xC4
    0,   // 0b11000101 = 0xC5
    0,   // 0b11000110 = 0xC6
    1,   // 0b11000111 = 0xC7
    1,   // 0b11001000 = 0xC8
    0,   // 0b11001001 = 0xC9
    0,   // 0b11001010 = 0xCA
    1,   // 0b11001011 = 0xCB
    0,   // 0b11001100 = 0xCC
    1,   // 0b11001101 = 0xCD
    1,   // 0b11001110 = 0xCE
    0,   // 0b11001111 = 0xCF
    1,   // 0b11010000 = 0xD0
    0,   // 0b11010001 = 0xD1
    0,   // 0b11010010 = 0xD2
    1,   // 0b11010011 = 0xD3
    0,   // 0b11010100 = 0xD4
    1,   // 0b11010101 = 0xD5
    1,   // 0b11010110 = 0xD6
    0,   // 0b11010111 = 0xD7
    0,   // 0b11011000 = 0xD8
    1,   // 0b11011001 = 0xD9
    1,   // 0b11011010 = 0xDA
    0,   // 0b11011011 = 0xDB
    1,   // 0b11011100 = 0xDC
    0,   // 0b11011101 = 0xDD
    0,   // 0b11011110 = 0xDE
    1,   // 0b11011111 = 0xDF
    1,   // 0b11100000 = 0xE0
    0,   // 0b11100001 = 0xE1
    0,   // 0b11100010 = 0xE2
    1,   // 0b11100011 = 0xE3
    0,   // 0b11100100 = 0xE4
    1,   // 0b11100101 = 0xE5
    1,   // 0b11100110 = 0xE6
    0,   // 0b11100111 = 0xE7
    0,   // 0b11101000 = 0xE8
    1,   // 0b11101001 = 0xE9
    1,   // 0b11101010 = 0xEA
    0,   // 0b11101011 = 0xEB
    1,   // 0b11101100 = 0xEC
    0,   // 0b11101101 = 0xED
    0,   // 0b11101110 = 0xEE
    1,   // 0b11101111 = 0xEF
    0,   // 0b11110000 = 0xF0
    1,   // 0b11110001 = 0xF1
    1,   // 0b11110010 = 0xF2
    0,   // 0b11110011 = 0xF3
    1,   // 0b11110100 = 0xF4
    0,   // 0b11110101 = 0xF5
    0,   // 0b11110110 = 0xF6
    1,   // 0b11110111 = 0xF7
    1,   // 0b11111000 = 0xF8
    0,   // 0b11111001 = 0xF9
    0,   // 0b11111010 = 0xFA
    1,   // 0b11111011 = 0xFB
    0,   // 0b11111100 = 0xFC
    1,   // 0b11111101 = 0xFD
    1,   // 0b11111110 = 0xFE
    0,   // 0b11111111 = 0xFF
};

#define PARITY(X) paritytable8[(uint8_t) X]

#else // PARITYBYTETABLE
/**
 *  @note  All 32 bits are used. The index 0 corresponds to the highest bit
 */
static uint32_t paritytable32[] = {
    0b10010110011010010110100110010110,   // 0x00 -> 0x1F
    0b01101001100101101001011001101001,   // 0x20 -> 0x3F
    0b01101001100101101001011001101001,   // 0x40 -> 0x5F
    0b10010110011010010110100110010110,   // 0x60 -> 0x7F
    0b01101001100101101001011001101001,   // 0x80 -> 0x9F
    0b10010110011010010110100110010110,   // 0xA0 -> 0xBF
    0b10010110011010010110100110010110,   // 0xC0 -> 0xDF
    0b01101001100101101001011001101001,   // 0xE0 -> 0xFF
};

#define PARITY(X) (paritytable32[(uint8_t) ((X)>>5)]&(0x8000000>>((X)&0x1F)

#endif // PARITYBYTETABLE

static NAND_RC
h2d_encode( uint8_t *data, uint32_t *pecc) {
const uint8_t maskq1  = 0x55;  // 0x0x0x0x: Only odd bits
const uint8_t maskp1  = 0xAA;  // x0x0x0x0: Only even bits
const uint8_t maskq2  = 0x33;  // 00xx00xx: Bits 5,4,1,0
const uint8_t maskp2  = 0xCC;  // xx00xx00: Bits 7,6,3,2
const uint8_t maskq4  = 0x0F;  // 00xx00xx: Bits 3,2,1,0
const uint8_t maskp4  = 0xF0;  // 00xx00xx: Bits 7,6,5,4
uint8_t p1=0,q1=0,p2=0,q2=0,p4=0,q4=0;
uint8_t p = 0;
uint32_t wp = 0; // parity word

    // Iterate over all values in page
    for(int i=0;i<512;i++) {
        uint8_t v = data[i];
        //printf("%3d -> %02X\n",i,(unsigned) v);
        p ^= v;
        if( PARITY(v) ) {
            // Address has 9 bits = 2^9 = 512
            //                 Pn        Qn = Pn'
            wp ^= ((i&1)   ? BIT(7)  : BIT(6));       //  P8/Q8
            wp ^= ((i&2)   ? BIT(9)  : BIT(8));       //  P16/Q16
            wp ^= ((i&4)   ? BIT(11) : BIT(10));      //  P32/Q32
            wp ^= ((i&8)   ? BIT(13) : BIT(12));      //  P64/Q64
            wp ^= ((i&16)  ? BIT(15) : BIT(14));      //  P128/Q128
            wp ^= ((i&32)  ? BIT(17) : BIT(16));      //  P256/Q256
            wp ^= ((i&64)  ? BIT(19) : BIT(18));      //  P512/Q512
            wp ^= ((i&128) ? BIT(21) : BIT(20));      //  P1024/Q1024
            wp ^= ((i&256) ? BIT(23) : BIT(22));      //  P2048/Q2048
        }
    }
    q1 = PARITY(p&maskq1);
    p1 = PARITY(p&maskp1);
    q2 = PARITY(p&maskq2);
    p2 = PARITY(p&maskp2);
    q4 = PARITY(p&maskq4);
    p4 = PARITY(p&maskp4);
    wp |= (p4<<5)| (q4<<4)| (p2<<3)| (q2<<2)| (p1<<1)| (q1<<0);

    *pecc = wp;
    return NAND_OK;
}
#endif // VERIFYDEVICEHAMMING


/**
 *  @brief  Verify ECC
 *
 *  @note    There are four cases to be handled.
 *
 *  | Case        |  ECC Result                                                |
 *  |-------------|------------------------------------------------------------|
 *  | No Error    | Syndrome has all valid Pn, Pn'. Pn = Pn' = 0               |
 *  | 1-bit Error | For all valid syndrome (Pn, Pn') pairs, Pn = not(Pn')      |
 *  | ECC Error   | Syndrome has only 1 bit set. Error is in the ECC itself    |
 *  | Hard Error  | Other cases. Multiple (2 or more) bits are incorrect.      |
 *
 *  @note  When there is an 1-bin error, it can be corrected.
 *
 *  @note  For 8-bit wide data the position of the incorrect bit is  indicated
 *         by bit pattern (P4, P2, P1); the position of the incorrect byte is
 *         indicated by (..., P32, P16, P8).
 *
 *  @note  For 16-bit wide  data the position of the incorrect bit  is
 *         (P8, P4, P2, P1); the incorrect byte number is indicated by
 *         (..., P64, P32, P16)
 *
 *  @note  In case of ECC error (1 bit set), there is no error in the data.
 *
 *  @note  All other results imply a multiple error without the possibility of
 *         correction.
 *
 *  @return
 *         0: No error
 *         1: 1-bit error. Parameters pos and bit shows where the error occurs
 *         2: Error in checksum
 *        -1: Errors in multiple bits
 */

static NAND_RC
h2d_verify( uint32_t wp1, uint32_t wp2, uint16_t *ppos, uint8_t *pbit) {

    // Calculate syndrome = XOR of parity words
    uint32_t syndrome = wp1^wp2;

    // If syndrome is all zero, no error
    if( syndrome == 0 ) {
        return NAND_OK;
    }

    // When only one bit set, there is an error in the ECC field
    if( ispower2(syndrome) ) {
        return NAND_CHECKSUM_ERROR;
    }

    // For all valid (?) syndrome (Pn,Qn) pairs
    // if Pn = !Qn, there is 1-bit error
    //
    uint32_t ps = syndrome&0xAAAAAA; // even bits
    uint32_t qs = syndrome&0x555555; // odd bits
    uint32_t e = ((ps)^(qs<<1))^0xAAAAAA;

    if( e != 0 ) {
        return NAND_MULTIPLE_ERRORS;
    }

    uint32_t w = ps;
    int bit   =  ((w&BIT(1))?1:0x0)
                |((w&BIT(3))?2:0x0)
                |((w&BIT(5))?4:0x0);
    int pos   =  ((w&BIT(7))?1:0x0)
                |((w&BIT(9))?2:0x0)
                |((w&BIT(11))?0x4:0x0)
                |((w&BIT(13))?0x8:0x0)
                |((w&BIT(15))?0x10:0x0)
                |((w&BIT(17))?0x20:0x0)
                |((w&BIT(19))?0x40:0x0)
                |((w&BIT(21))?0x80:0x0)
                |((w&BIT(23))?0x100:0x0);

    // one error detected. It can be corrected
    *ppos = pos;
    *pbit = bit;

    return NAND_CORRECTEABLE_ERROR;
}

/*********************** Status routines **************************************/

/**
 *  @brief  Return status from device using command status
 */
uint32_t NAND_Status(void) {
uint8_t status;

    EnableCE();
    SendCommand(CMD_READSTATUS,0);
    status = NAND_DATA;
    DisableCE();

    return status&NAND_STATUS_ALL; // Show only the valid status bits
}

/**
 *  @brief  Wait until Ready/Busy bit in Status Register indicates Ready
 *
 *  @note   Bit R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when error!
 */
NAND_RC
NAND_WaitUntilReadyStatus(void) {

    nano_delay(NANO_DELAY);
    uint32_t timeout = NAND_TIMEOUT;

    do {
        NAND_COMMAND = CommandList[CMD_READSTATUS].v[0];
    } while ( --timeout && (NAND_DATA&NAND_STATUS_READY) );

    return ((NAND_DATA&NAND_STATUS_READY) != 0) ? NAND_READY : NAND_TIMEOUT_ERROR;

}

/**
 *  @brief  Check if Ready using Status Register
 *
 *  @note   Pin R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when Busy
 */
NAND_RC
NAND_CheckReadyStatus(void) {

    nano_delay(NANO_DELAY);
    NAND_COMMAND = CommandList[CMD_READSTATUS].v[0];

    return (NAND_DATA&NAND_STATUS_READY) ? NAND_READY : NAND_BUSY;
}

///@{
/**
 * @brief  Ready/Busy functions
 *
 * @note
 */

NAND_RC
NAND_Ready(void) {

    return ((GPIO_ReadPins(RB_GPIO)&RB_PINMASK)!=0) ? NAND_READY : NAND_BUSY;
}

NAND_RC
NAND_Busy(void) {

    return ((GPIO_ReadPins(RB_GPIO)&RB_PINMASK)!=0) ? NAND_BUSY : NAND_READY;
}

/**
 *  @brief  Wait until Ready/Busy Pin indicates Ready
 *
 *  @note   Pin R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when error!
 */
NAND_RC
NAND_WaitUntilReadyPin(void) {

    nano_delay(NANO_DELAY);
    uint32_t timeout = NAND_TIMEOUT;
    while ( --timeout && ( (GPIO_ReadPins(RB_GPIO)&RB_PINMASK)==0) )
        {}

    return (GPIO_ReadPins(RB_GPIO)&RB_PINMASK) ? NAND_READY : NAND_TIMEOUT_ERROR;
}

/**
 *  @brief  Check if Ready using R/B# pin
 *
 *  @note   Pin R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when Busy
 */
NAND_RC
NAND_CheckReadyPin(void) {

    nano_delay(NANO_DELAY);
    return (GPIO_ReadPins(RB_GPIO)&RB_PINMASK) ? NAND_READY : NAND_BUSY;
}
///@}

/*********************** Copy routines ****************************************/

/**
 *  @brief  Copy data from memory to Flash device
 *
 *  @note   Preparation to use DMA
 */
static NAND_RC
CopyToFlash(uint8_t *data, uint16_t n) {

    for(uint16_t i=0;i<n;i++) {
        NAND_DATA = data[i];
    }
    return NAND_OK;
}

/**
 *  @brief  Copy data from Flash device into memory
 *
 *  @note   Preparation to use DMA
 */
static NAND_RC
CopyFromFlash(uint8_t *data, uint16_t n) {

    for(uint16_t i=0;i<n;i++) {
        data[i] = NAND_DATA;
    }
    return NAND_OK;
}

/**
 *  @brief  Copy data from Flash device to memory (ECC version)
 *
 *  @note   Prepared to use DMA
 */
static NAND_RC
CopyFromFlashECC(uint8_t *data, uint16_t n, uint32_t *pchksum) {

    if( n != NAND_PAGESIZE ) // Must be 512!!!
        return NAND_IRRECOVERABLE_ERROR;

    // read Areas A and B (page area)
    EBI->CMD = EBI_CMD_ECCCLEAR;
    EBI->CMD = EBI_CMD_ECCSTART;
    for(uint16_t i=0;i<NAND_PAGESIZE;i++) {
        data[i] = NAND_DATA;
    }
    EBI->CMD = EBI_CMD_ECCSTOP;
    *pchksum = EBI->ECCPARITY;

    // Read area C (spare area)
    for(uint16_t i=0;i<NAND_SPARESIZE;i++) {
        data[NAND_PAGESIZE+i] = NAND_DATA;
    }
    return NAND_OK;
}

/**
 *  @brief  Copy data from memory to Flash device ECC (ECC version)
 *
 *  @note   Prepared to use DMA
 */
static NAND_RC
CopyToFlashECC(uint8_t *data, uint16_t n) {

    if( n != NAND_PAGESIZE ) // Must be 512!!!!
        return NAND_ERROR;

    EBI->CMD = EBI_CMD_ECCCLEAR;
    EBI->CMD = EBI_CMD_ECCSTART;
    for(uint16_t i=0;i<NAND_PAGESIZE;i++) {
        NAND_DATA = data[i];
    }
    EBI->CMD = EBI_CMD_ECCSTOP;
    // result in EBI->ECCPARITY;
    uint32_t chksum1 = EBI->ECCPARITY;
#ifdef VERIFYDEVICEHAMMING
    uint32_t chksum2 =  data[NAND_PAGESIZE+NAND_CHECKSUM_POS_0]
                      +(data[NAND_PAGESIZE+NAND_CHECKSUM_POS_1]<<8)
                      +(data[NAND_PAGESIZE+NAND_CHECKSUM_POS_2]<<16);

    if( chksum1 != chksum2 )
        return NAND_IRRECOVERABLE_ERROR;
#endif
    data[NAND_PAGESIZE+NAND_CHECKSUM_POS_0] = chksum1&0xFF;
    data[NAND_PAGESIZE+NAND_CHECKSUM_POS_1] = (chksum1>>8)&0xFF;
    data[NAND_PAGESIZE+NAND_CHECKSUM_POS_2] = (chksum1>>16)&0xFF;

    for(uint16_t i=0;i<NAND_SPARESIZE;i++) {
        NAND_DATA = data[i];
    }

    return NAND_OK;
}

/*********************** Read Routines ****************************************/

/**
 *  @brief  Return signature of device
 */

NAND_RC
NAND_ReadSignature(uint8_t data[NAND_SIGNATURESIZE]) {

    EnableCE();
    SendCommand(CMD_READSIGNATURE,0);

    NAND_RC rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    CopyFromFlash(data,2);

    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/**
  * @brief   NAND_ReadSpare
  *
  * @note    Read a 16 byte from NAND device
  */
NAND_RC
NAND_ReadSpare(uint32_t pageaddr, uint8_t data[NAND_SPARESIZE]) {
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    SendCommand(CMD_READC,addr);

    NAND_RC rc = NAND_WaitUntilReadyPin();

    if( rc != 0 ) {
        CopyFromFlash(data,NAND_SPARESIZE);
    }

    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/**
  * @brief   NAND_ReadPage
  *
  * @note    Read a page (512 bytes) from NAND device starting at a given
  *          page aligned address
  */
NAND_RC
NAND_ReadPage(uint32_t pageaddr, uint8_t data[NAND_PAGESIZE]) {
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    SendCommand(CMD_READA,addr);
    NAND_RC rc = NAND_WaitUntilReadyPin();
    if( rc == NAND_READY ) {
        CopyFromFlash(data,NAND_PAGESIZE);
    }

    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/**
  * @brief   NAND_ReadFullPage
  *
  * @note    Read a page (528 bytes) from NAND device starting at a given
  *          page address
  */
NAND_RC
NAND_ReadFullPage(uint32_t pageaddr, uint8_t data[NAND_FULLPAGESIZE]) {
const uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    SendCommand(CMD_READA,addr);

    NAND_RC rc = NAND_WaitUntilReadyPin();
    if( rc != 0 ) {
        CopyFromFlash(data,NAND_FULLPAGESIZE);
    }

    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/**
  * @brief   NAND_ReadFullPage
  *
  * @note    Read a page (528 bytes) from NAND device starting at a given
  *          page address
  */
NAND_RC
NAND_ReadFullPageECC(uint32_t pageaddr, uint8_t data[NAND_FULLPAGESIZE]) {
const uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    SendCommand(CMD_READA,addr);

    NAND_RC rc;
    rc = NAND_WaitUntilReadyPin();
    uint32_t checksum1 = 0;
    if( rc != 0 ) {
        CopyFromFlashECC(data,NAND_FULLPAGESIZE,&checksum1);
    }

#ifdef VERIFYDEVICEHAMMING
    uint32_t checksum2 = 0;
    h2d_encode(data,&checksum2);

    // Verify
    if( checksum1 != checksum2 )
        return NAND_IRRECOVERABLE_ERROR;
#endif

    DisableCE();

    // Get Checksum from read data (Little endian)
    uint32_t chksumread = data[NAND_PAGESIZE+NAND_CHECKSUM_POS_0]
                        +(data[NAND_PAGESIZE+NAND_CHECKSUM_POS_1]<<8)
                        +(data[NAND_PAGESIZE+NAND_CHECKSUM_POS_2]<<16);

    uint16_t pos = 0;
    uint8_t bit = 0;
    rc = h2d_verify( chksumread, checksum1, &pos, &bit);
    if( rc == NAND_CORRECTEABLE_ERROR ) {
        data[pos] ^= (1<<bit);
    }
    //rc = NAND_Status();
    // Same return values as ReadFullPageECC
    return rc;
}

/*********************** Write Routines ***************************************/

/**
  * @brief   NAND_WriteSpare
  *
  * @note    Write a 16 bytes onto NAND device
  */
NAND_RC
NAND_WriteSpare(uint32_t pageaddr, uint8_t data[NAND_SPARESIZE]) {
const uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    DisableWP();
    SendCommand(CMD_PROGRAMA,addr);

    NAND_RC rc = NAND_WaitUntilReadyPin();

    CopyToFlash(data, NAND_SPARESIZE);

    SendCommand(CMD_CONFIRM,addr);

    EnableWP();
    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/**
  * @brief   NAND_WritePage
  *
  * @note    Write a page (512 bytes) into NAND device starting at given page
  *          address
  */
NAND_RC
NAND_WritePage(uint32_t pageaddr, uint8_t data[NAND_PAGESIZE]) {
const uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }
    EnableCE();
    DisableWP();

    SendCommand(CMD_PROGRAMA,addr);

    NAND_RC rc;
    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    CopyToFlash(data, NAND_PAGESIZE);

    SendCommand(CMD_CONFIRM,addr);

    EnableWP();
    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/**
  * @brief   NAND_WriteFullPage
  *
  * @note    Write a full page (528 bytes) into NAND device starting at given
  *          address. The address must be aligned to a page address.
  *
  * @note    A confirm command is needed after a half page
  */
NAND_RC
NAND_WriteFullPage(uint32_t pageaddr, uint8_t data[NAND_FULLPAGESIZE]) {
const uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    DisableWP();

    SendCommand(CMD_PROGRAMA,addr);

    NAND_RC rc;
    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    CopyToFlash(data, NAND_FULLPAGESIZE);

    SendCommand(CMD_CONFIRM,addr);

    EnableWP();
    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/**
  * @brief   NAND_WriteFullPage
  *
  * @note    Write a full page (528 bytes) into NAND device starting at given
  *          address. The address must be aligned to a page address.
  *
  * @note    A confirm command is needed after a half page
  */
NAND_RC
NAND_WriteFullPageECC(uint32_t pageaddr, uint8_t data[NAND_FULLPAGESIZE]) {
const uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    DisableWP();

    SendCommand(CMD_PROGRAMA,addr);
    NAND_RC rc;
    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    uint32_t chksum = 0;
    h2d_encode(data, &chksum);
    data[NAND_PAGESIZE+NAND_CHECKSUM_POS_0] = chksum&0xFF;
    data[NAND_PAGESIZE+NAND_CHECKSUM_POS_1] = (chksum>>8)&0xFF;
    data[NAND_PAGESIZE+NAND_CHECKSUM_POS_2] = (chksum>>16)&0xFF;

    CopyToFlashECC(data, NAND_FULLPAGESIZE);

    SendCommand(CMD_CONFIRM,addr);

    EnableWP();
    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}


/*********************** Erase Routines ***************************************/

/**
  * @brief   NAND_EraseBlock
  *
  * @note    Erase a full block of 32 pages
  */
NAND_RC
NAND_EraseBlock(uint32_t blockaddr) {
const uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(blockaddr);

    if( !IsBlockAddress(addr)) {
        return NAND_INVALID_ADDRESS;
    }

    EnableCE();
    DisableWP();

    SendCommand(CMD_ERASEBLOCK,addr);
    NAND_RC rc;
    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    EnableWP();
    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}

/*********************** Copy Back Routine ************************************/
/**
  * @brief   NAND_CopyBack
  *
  * @note    Copy the page at *pageaddr_src* to the page addressed by *pageaddr_dst*
  */
NAND_RC
NAND_CopyBack(uint32_t pageaddr_src, uint32_t pageaddr_dst) {

uint32_t srcaddr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr_src);
uint32_t dstaddr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr_dst);

    if( ! (IsPageAddress(srcaddr)&&IsPageAddress(dstaddr)) ) {
        return NAND_INVALID_ADDRESS;
    }

    // They must have the same value for the highest address bit
    if( ((srcaddr^dstaddr)&NAND_COPYREGIONMASK) != 0 ) {
        return NAND_IRRECOVERABLE_ERROR;
    }

    EnableCE();
    DisableWP();

    SendCommand(CMD_COPYBACK1, srcaddr);

    NAND_RC rc;
    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    SendCommand(CMD_COPYBACK2, dstaddr);
    rc = NAND_WaitUntilReadyPin();

    EnableWP();
    DisableCE();

    rc = NAND_WaitUntilReadyPin();
    if( rc != NAND_READY )
        return NAND_ERROR;

    return NAND_OK;
}
