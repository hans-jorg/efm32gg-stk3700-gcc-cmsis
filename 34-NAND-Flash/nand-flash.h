#ifndef NAND_FLASH_H
#define NAND_FLASH_H

/**
 * @file nand-flash.h
 * @brief Interface routines for the ST NAND256 Flash device in the EFM32GG-STK3700 board
 *
 * @note
 *      They will be used as the device interface for the FatFS middleware
 *
 *      It does not have Garbage Collection nor Wear-Leveling mechanims
 *
 * @version 1.0.0
 * Date:    7 October 2023
 *
 */

#include <stdint.h>

/**
 *  @brief  NAND parameters (all sizes in bytes)
 */

// Capacities
#define NAND_CAPACITY               (32*1024L*1024L)
#define NAND_PAGEMAXCOUNT           (65536L)
#define NAND_BLOCKMAXCOUNT          (2048L)
#define NAND_PAGESPERBLOCK          (32)

// maximal address for pages and blocks
#define NAND_PAGEADDRMAX            (NAND_PAGEMAXCOUNT-1)
#define NAND_BLOCKADDRMAX           (NAND_BLOCKMAXCOUNT-1)

// Page, Block and Spare size
#define NAND_PAGESIZE               (512)
#define NAND_HALFPAGESIZE           (256)
#define NAND_SPARESIZE              (16L)
#define NAND_FULLPAGESIZE           (NAND_PAGESIZE+NAND_SPARESIZE)
#define NAND_BLOCKSZE               (NAND_PAGESIZE*NAND_PAGESPERBLOCK)

// NAND_PAGESIZE demands 9 bits for addressing inside a page
// NAND_BLOCKMAXCOUNT demands 14 bits for addressing inside a block
#define NAND_PAGEADDRESS_POS        (9)
#define NAND_BLOCKADDRESS_POS       (14)

// Converting Page Address and Block Address to a Full Address
#define NAND_FULLADDR_FROM_PAGEADDR(PAGEADDR)   ((PAGEADDR)<<NAND_PAGEADDRESS_POS)
#define NAND_FULLADDR_FROM_BLOCKADDR(BLOCKADDR) ((BLOCKADDR)<<NAND_BLOCKADDRESS_POS)

#define NAND_PAGECOUNT2             (NAND_CAPACITY/NAND_PAGESIZE)
#define NAND_BLOCKCOUNT2            (NAND_PAGECOUNT/NAND_PAGESPERBLOCK)

// These symbols are for full addresses
#define NAND_PAGEADDRMASK           ((NAND_PAGEADDRMAX)<<NAND_PAGEADDRESS_POS)
#define NAND_BLOCKADDRMASK          ((NAND_BLOCKADDRMAX)<<NAND_BLOCKADDRESS_POS)
#define NAND_COPYREGIONMASK         (0x1000000)
#define NAND_MAXADDRESS             (NAND_CAPACITY-1)

// Consistency
/*
 *
 * #if   (NAND_PAGECOUNT) != (NAND_PAGECOUNT2)       \
 *    || (NAND_BLOCKCOUNT) != (NAND_BLOCKCOUNT2)
 *    #error "NAND Flash parameters with errors"
 * #endif
 */

// Align macros
#define NAND_ALIGNTOHALFPAGEADDRESS(A)   ((A)&0x1FFFF00)
#define NAND_ALIGNTOPAGEADDRESS(A)       ((A)&0x1FFFE00)
#define NAND_ALIGNBLOCKADDRESS(A)        ((A)&0x1FFC000)

#define NAND_TIMEOUT                (1000)


/**
 * @brief  Status bits of Status Data
 */
///@{
#define NAND_STATUS_WRITEPROTECTED           (0x80)
#define NAND_STATUS_READY                    (0x40)
#define NAND_STATUS_ERROR                    (0x01)
#define NAND_STATUS_ALL  ( \
                     NAND_STATUS_WRITEPROTECTED \
                    |NAND_STATUS_READY          \
                    |NAND_STATUS_ERROR   )
///@}

int32_t  NAND_Init(void);
uint32_t NAND_Status(void);

int32_t  NAND_Ready(void);
int32_t  NAND_Busy(void);
int32_t  NAND_Error(void);

int32_t  NAND_WaitUntilReadyStatus(void);
int32_t  NAND_CheckReadyStatus(void);

int32_t  NAND_WaitUntilReadyPin(void);
int32_t  NAND_CheckReadyPin(void);

int32_t  NAND_ReadPage(uint32_t pageaddr, uint8_t *data);
int32_t  NAND_ReadFullPage(uint32_t pageaddr, uint8_t *data);
int32_t  NAND_ReadSpare(uint32_t pageaddr, uint8_t *data);

int32_t  NAND_WritePage(uint32_t pageaddr, uint8_t *data);
int32_t  NAND_WriteFullPage(uint32_t pageaddr, uint8_t *data);
int32_t  NAND_WriteSpare(uint32_t pageaddr, uint8_t *data);

int32_t  NAND_ReadSignature(uint8_t *data);
int32_t  NAND_EraseBlock(uint32_t blockaddr);
int32_t  NAND_CopyBack(uint32_t pageaddr_src, uint32_t pageaddr_dst);

// For debug
void     NAND_EnableWriteProtect(void);
void     NAND_DisableWriteProtect(void);


#endif // NAND_FLASH_H
