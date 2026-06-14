/**
 * @file  lfs_interface.c
 * @brief Interface routine for the NAND256-A Flash device in the
 *        EFM32GG-STK3700 board to be used by LittleFS
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
 * Date:    31 May 2026
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

#include <em_device.h>
#include <stdint.h>
#include "lfs_interface.h"
#include "nand-flash.h"

#ifndef BIT
#define BIT(N)  (1U<<(N))
#endif

/**
 *  @brief
 *
 *  @note   All access by LittleFS is done either using the block number
 *          or the page number.
 *
 *  @note   The address used in the nand-flash.[ch] is in BYTES!!!!!!!!!!!
 */
#define FLASH_PAGESIZE                 NAND_PAGESIZE
//#define BLOCKSHIFT                     (5)
//#define FLASH_PAGEADDR(BLOCKN,OFFSET)  ((BLOCKN<<BLOCKSHIFT)|(OFFSET))


const struct lfs_config flash_config = {
    // block device operations
    .read  = flash_read,
    .prog  = flash_prog,
    .erase = flash_erase,
    .sync  = flash_sync,

    // block device configuration
    .read_size = 512,
    .prog_size = 512,
    .block_size = 8192,
    .block_count = 2048,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};

/**
 *  @brief  Initialized this flash interface
 */
int flash_init(const struct lfs_config *c) {

    return 0;
}

/**
 *  @brief  Read a flash buffer
 */
int flash_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size) {

    return 0;
}

/**
 *  @brief  Program/Write the contents of a flash buffer into
 */
int flash_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size) {

    return 0;
}

/**
 *  @brief  Erase a block
 */
int flash_erase(const struct lfs_config *c, lfs_block_t block) {

    return 0;
}
/**
 *  @brief  Sync data in RAM with data in the device
 */
int flash_sync(const struct lfs_config *c) {


    return 0;
}
