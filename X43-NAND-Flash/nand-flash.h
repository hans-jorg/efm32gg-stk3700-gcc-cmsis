#ifndef NAND_FLASH_H
#define NAND_FLASH_H

/**
 * @file nand-flash.h
 * @brief Interface routine for the ST NAND256 Flash device in the EFM32GG-STK3700 board
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

#include <stdint.h>

/**
 *  @brief  NAND features
 */
#define NAND_CAPACITY               (32*1024*1024)
#define NAND_PAGESIZE               (512)
#define NAND_HALFPAGESIZE           (256)
#define NAND_SPARESIZE              (16L)
#define NAND_FULLPAGESIZE           (NAND_PAGESIZE+16)
#define NAND_PAGESPERBLOCK          (32)
#define NAND_BLOCKSIZE              (NAND_PAGESPERBLOCK*NAND_PAGESIZE)
#define NAND_PAGECOUNT              (NAND_CAPACITY/NAND_PAGESIZE)
#define NAND_BLOCKCOUNT             (NAND_PAGECOUNT/NAND_PAGESPERBLOCK)

#define NAND_COLUMN_POS             (0)
#define NAND_PAGEADDRESS_POS        (9)
#define NAND_BLOCKADDRESS_POS       (14)

#define NAND_PAGEADDRMASK           ((NAND_PAGECOUNT-1)<<NAND_PAGEADDRESS_POS)
#define NAND_BLOCKADDRMASK          ((NAND_BLOCKCOUNT-1)<<NAND_BLOCKADDRESS_POS)
#define NAND_COPYREGIONMASK         (0x1000000)
#define NAND_MAXADDRESS             (NAND_CAPACITY-1)

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

int32_t  NAND_ReadPage(uint32_t addr, uint8_t *data);
int32_t  NAND_ReadFullPage(uint32_t addr, uint8_t *data);
int32_t  NAND_ReadSpare(uint32_t addr, uint8_t *data);

int32_t  NAND_WritePage(uint32_t addr, uint8_t *data);
int32_t  NAND_WriteFullPage(uint32_t addr, uint8_t *data);
int32_t  NAND_WriteSpare(uint32_t addr, uint8_t *data);

int32_t  NAND_ReadSignature(uint8_t *data);
int32_t  NAND_EraseBlock(uint32_t blockaddr);
int32_t  NAND_CopyBack(uint32_t srcaddr, uint32_t dstaddr);

// For debug
void     NAND_EnableWriteProtect(void);
void     NAND_DisableWriteProtect(void);


#endif // NAND_FLASH_H
