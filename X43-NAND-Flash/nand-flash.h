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


/**
 *  @brief  NAND features
 */
#define NAND_CAPACITY                           (32*1024L*1024L)
#define NAND_PAGESIZE                           (512L)
#define NAND_SPARESIZE                          (16L)
#define NAND_BLOCKSIZE                          (32*NAND_PAGESIZE)
#define NAND_PAGECOUNT                          (NAND_CAPACITY/NAND_PAGESIZE)
#define NAND_PAGEADDRMASK                       ((NAND_PAGECOUNT-1)<<9)





int NAND_Init(void);
int NAND_WritePage(uint32_t pageaddr, uint8_t *data);
int NAND_ReadPage(uint32_t pageaddr, uint8_t *data);

int NAND_EnableWriteProtect(void);
int NAND_DisableWriteProtect(void);


#endif // NAND_FLASH_H
