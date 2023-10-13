#ifndef NAND_FLASH_H
#define NAND_FLASH_H

/**
 * @file nand-flash.h
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


int NAND_Init(void);
int NAND_Write(uint32_t address, uint8_t *data, uint32_t size);
int NAND_Read(uint32_t address, uint8_t *data, uint32_t size);

int NAND_EnableWriteProtect(void);
int NAND_DisableWriteProtect(void);


#end // NAND_FLASH_H
