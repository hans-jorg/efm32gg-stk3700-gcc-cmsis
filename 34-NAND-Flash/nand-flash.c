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

#define IsPageAddress(A)             (((A)&PAGEMASK)==0)
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
 * @brief  Adresses used to access NAND Flash
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

/**
 *  @brief  Spare Area (Area C)
 */
///@{
#define BADBLOCKFLAG                     (5)
#define OKVALUE                          '\xFF'
#define ECC0_POS                         (6)
#define ECC1_POS                         (7)
#define ECC2_POS                         (8)

//static uint8_t spare[NAND_SPARESIZE];

/*
 *  @brief  Nano delay
 */
void nano_delay(uint32_t n) {
    while(n--) {
        __NOP();
    }
}

/*********************** Command routine **************************************/

/**
 *  @brief  Send Command
 */
static int32_t
SendCommand(int32_t cmd, uint32_t address) {
    for(int32_t i=0;i<CommandList[cmd].n;i++) {
        uint8_t command =  CommandList[cmd].v[i];
        if( command == FULLADDRESS ) {
            NAND_ADDRESS = address&0xFF;
            address >>= 9;   // No A8
            NAND_ADDRESS = address&0xFF;
            address >>= 8;
            NAND_ADDRESS = address&0xFF;
            // Only for 512 MB and 1 GB devices
            // address >>= 8;
            // NAND_ADDRESS = address&0xFF;
        } else if ( command == SHORTADDRESS ) {
            address >>= 9;
            NAND_ADDRESS = address&0xFF;
            address >>= 8;
            NAND_ADDRESS = address&0xFF;
            // Only for 512 MB and 1 GB devices
            // address >>= 8;
            // NAND_ADDRESS = address&0xFF;
        } else {
            NAND_COMMAND = command;
        }

    }

    int32_t rc = NAND_WaitUntilReadyPin();
    rc = NAND_WaitUntilReadyPin();

    return rc; // Nonzero if OK
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

static inline void EnablePWR(void) {
    // Set to High (positive logic)
    GPIO_SetPins(PWR_GPIO, PWR_PINMASK);
    NAND_WaitUntilReadyPin();
}

static inline void DisablePWR(void) {
    // Set to  Low (positive logic)
    GPIO_ClearPins(PWR_GPIO, PWR_PINMASK);
}

static inline void EnableCE(void) {
    // Set to Low (Active Low)
    GPIO_ClearPins(CE_GPIO, CE_PINMASK);
}

static inline void DisableCE(void) {
    // Set to High (Active Low)
    GPIO_SetPins(CE_GPIO, CE_PINMASK);
}

static inline void EnableWP(void) {
    // Set to Low due to the negative logic
    GPIO_ClearPins(WP_GPIO, WP_PINMASK);
}

static inline void DisableWP(void) {
    // Set to High due to the negative logic
    GPIO_SetPins(WP_GPIO, WP_PINMASK);
}

// For debug mainly
void  NAND_EnableWriteProtect(void) {
    // Enable Write Protect using the WP pin
    EnableWP();
}

// For debug mainly
void  NAND_DisableWriteProtect(void) {
    // Disable Write Protect using the WP pin
    DisableWP();
}
///@}

/*********************** Initialization routines ******************************/

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
 * @note   Only the pins directly controlled thru GPIO are configured here.
 *         The pins directly controlled by EBI are configured elsewhere
 *
 */
static inline void ConfigGPIOPins(void) {

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
static inline void ConfigEBIPins(void) {

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

static inline void ConfigEBI(void) {

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

int32_t NAND_Init(void) {

    ConfigGPIOPins();
    ConfigEBIPins();

    EnableEBIClock();
    ConfigEBI();

    EnablePWR();
    DisableCE();
    EnableWP();

    int32_t rc = NAND_Status();
    return rc;
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

    return status&NAND_STATUS_ALL;
}

/**
 *  @brief  Wait until Ready/Busy bit in Status Register indicates Ready
 *
 *  @note   Bit R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when error!
 */
int32_t  NAND_WaitUntilReadyStatus(void) {

    nano_delay(NANO_DELAY);
    uint32_t timeout = NAND_TIMEOUT;
    do {
        NAND_COMMAND = CommandList[CMD_READSTATUS].v[0];
    } while ( --timeout && (NAND_DATA&NAND_STATUS_READY) );
    return NAND_DATA&NAND_STATUS_READY;
}

/**
 *  @brief  Check if Ready using Status Register
 *
 *  @note   Pin R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when Busy
 */
int32_t  NAND_CheckReadyStatus(void) {

    nano_delay(NANO_DELAY);
    NAND_COMMAND = CommandList[CMD_READSTATUS].v[0];
    return NAND_DATA&NAND_STATUS_READY;
}

///@{
/**
 * @brief  Ready/Busy functions
 *
 * @note
 */

int32_t  NAND_Ready(void) {

    return (GPIO_ReadPins(RB_GPIO)&RB_PINMASK)!=0;
}

int32_t  NAND_Busy(void) {

    return !NAND_Ready();
}

/**
 *  @brief  Wait until Ready/Busy Pin indicates Ready
 *
 *  @note   Pin R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when error!
 */
int32_t  NAND_WaitUntilReadyPin(void) {

    nano_delay(NANO_DELAY);
    uint32_t timeout = NAND_TIMEOUT;
    while ( --timeout && ( (GPIO_ReadPins(RB_GPIO)&RB_PINMASK)==0) )
        {}
    return (GPIO_ReadPins(RB_GPIO)&RB_PINMASK);
}

/**
 *  @brief  Check if Ready using R/B# pin
 *
 *  @note   Pin R/B# is 1 when device is ready and 0 when busy
 *
 *  @returns 0 when Busy
 */
int32_t  NAND_CheckReadyPin(void) {

    nano_delay(NANO_DELAY);
    return (GPIO_ReadPins(RB_GPIO)&RB_PINMASK);
}
///@}

/*********************** Copy routines ****************************************/

/**
 *  @brief  Copy data from Flash device into memory
 *
 *  @note   Preparation to use DMA
 */
static void CopyFromFlash(uint8_t *data, uint16_t n) {

    for(uint16_t i=0;i<n;i++) {
        data[i] = NAND_DATA;
    }
}

/**
 *  @brief  Copy data from memory to Flash device
 *
 *  @note   Preparation to use DMA
 */
static void CopyToFlash(uint8_t *data, uint16_t n) {

    for(uint16_t i=0;i<n;i++) {
        NAND_DATA = data[i];
    }
}

/*********************** Read Routines ****************************************/

/**
 *  @brief  Return signature of device
 */

int32_t  NAND_ReadSignature(uint8_t data[NAND_SIGNATURESIZE]) {
int32_t rc;

    EnableCE();
    SendCommand(CMD_READSIGNATURE,0);
    rc = NAND_WaitUntilReadyPin();

    CopyFromFlash(data,2);

    DisableCE();
    rc = NAND_Status();

    return rc;
}

/**
  * @brief   NAND_ReadSpare
  *
  * @note    Read a 16 byte from NAND device
  */
int32_t NAND_ReadSpare(uint32_t pageaddr, uint8_t data[NAND_SPARESIZE]) {
int32_t rc;
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return -1;
    }

    EnableCE();
    SendCommand(CMD_READC,addr);

    rc = NAND_WaitUntilReadyPin();

    if( rc != 0 ) {
        CopyFromFlash(data,NAND_SPARESIZE);
    }

    DisableCE();
    rc = NAND_Status();

    return rc;
}

/**
  * @brief   NAND_ReadPage
  *
  * @note    Read a page (512 bytes) from NAND device starting at a given
  *          page aligned address
  */
int32_t NAND_ReadPage(uint32_t pageaddr, uint8_t data[NAND_PAGESIZE]) {
int32_t rc;
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return -1;
    }

    EnableCE();
    SendCommand(CMD_READA,addr);
    rc = NAND_WaitUntilReadyPin();
    if( rc != 0 ) {
        CopyFromFlash(data,NAND_PAGESIZE);
    }

    DisableCE();

    rc = NAND_Status();

    return rc;
}

/**
  * @brief   NAND_ReadFullPage
  *
  * @note    Read a page (528 bytes) from NAND device starting at a given
  *          page address
  */
int32_t NAND_ReadFullPage(uint32_t pageaddr, uint8_t data[NAND_FULLPAGESIZE]) {
int32_t rc;
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return -1;
    }

    EnableCE();
    SendCommand(CMD_READA,addr);
    rc = NAND_WaitUntilReadyPin();

    if( rc != 0 ) {
        CopyFromFlash(data,NAND_FULLPAGESIZE);
    }

    DisableCE();

    rc = NAND_Status();

    return rc;
}

/*********************** Write Routines ***************************************/

/**
  * @brief   NAND_WriteSpare
  *
  * @note    Write a 16 bytes onto NAND device
  */
int32_t NAND_WriteSpare(uint32_t pageaddr, uint8_t data[NAND_SPARESIZE]) {
int32_t rc;
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return -1;
    }

    EnableCE();
    DisableWP();
    SendCommand(CMD_PROGRAMA,addr);
    rc = NAND_WaitUntilReadyPin();

    CopyToFlash(data, NAND_SPARESIZE);

    SendCommand(CMD_CONFIRM,addr);

    EnableWP();
    DisableCE();

    rc = NAND_Status();

    return rc;
}

/**
  * @brief   NAND_WritePage
  *
  * @note    Write a page (512 bytes) into NAND device starting at given page
  *          address
  */
int32_t NAND_WritePage(uint32_t pageaddr, uint8_t data[NAND_PAGESIZE]) {
int32_t rc;
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return -1;
    }
    EnableCE();
    DisableWP();

    SendCommand(CMD_PROGRAMA,addr);
    rc = NAND_WaitUntilReadyPin();

    CopyToFlash(data, NAND_PAGESIZE);

    SendCommand(CMD_CONFIRM,addr);

    EnableWP();
    DisableCE();

    rc = NAND_Status();

    return rc;
}

/**
  * @brief   NAND_WriteFullPage
  *
  * @note    Write a full page (528 bytes) into NAND device starting at given
  *          address. The address must be aligned to a page address.
  *
  * @note    A confirm command is needed after a half page
  */
int32_t NAND_WriteFullPage(uint32_t pageaddr, uint8_t data[NAND_FULLPAGESIZE]) {
int32_t rc;
uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr);

    if( !IsPageAddress(addr)) {
        return -1;
    }

    EnableCE();
    DisableWP();

    SendCommand(CMD_PROGRAMA,addr);
    rc = NAND_WaitUntilReadyPin();

    CopyToFlash(data, NAND_FULLPAGESIZE);

    SendCommand(CMD_CONFIRM,addr);

    EnableWP();
    DisableCE();

    rc = NAND_Status();

    return rc;
}

/*********************** Erase Routines ***************************************/

/**
  * @brief   NAND_EraseBlock
  *
  * @note    Erase a full block of 32 pages
  */
int32_t NAND_EraseBlock(uint32_t blockaddr) {
    int32_t rc;
    uint32_t addr = NAND_FULLADDR_FROM_PAGEADDR(blockaddr);

    if( !IsBlockAddress(addr)) {
        return -1;
    }

    EnableCE();
    DisableWP();

    SendCommand(CMD_ERASEBLOCK,addr);
    rc = NAND_WaitUntilReadyPin();

    EnableWP();
    DisableCE();

    rc = NAND_Status();

    return rc;
}

/*********************** Copy Back Routine ************************************/
/**
  * @brief   NAND_CopyBack
  *
  * @note    Copy the page at *pageaddr_src* to the page addressed by *pageaddr_dst*
  */
int32_t  NAND_CopyBack(uint32_t pageaddr_src, uint32_t pageaddr_dst) {
int32_t rc;
uint32_t srcaddr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr_src);
uint32_t dstaddr = NAND_FULLADDR_FROM_PAGEADDR(pageaddr_dst);

    if( ! (IsPageAddress(srcaddr)&&IsPageAddress(dstaddr)) ) {
        return -1;
    }

    // They must have the same value for the highest address bit
    if( ((srcaddr^dstaddr)&NAND_COPYREGIONMASK) != 0 ) {
        return -2;
    }

    EnableCE();
    DisableWP();

    SendCommand(CMD_COPYBACK1, srcaddr);
    rc = NAND_WaitUntilReadyPin();

    SendCommand(CMD_COPYBACK2, dstaddr);
    rc = NAND_WaitUntilReadyPin();

    EnableWP();
    DisableCE();

    rc = NAND_Status();

    return rc;
}
