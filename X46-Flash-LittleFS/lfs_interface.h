#ifndef LFS_INTERFACE_H
#define LFS_INTERFACE_H

/**
 * @file  lfs_interface.h
 * @brief Interface for the LittleFS of the ST NAND256 Flash device in the
 *        EFM32GG-STK3700 board
 *
 * @note
 *
 * @version 1.0.0
 * Date:    31 May 2026
 */

#include <stdint.h>
#include "lfs.h"

/**
 *  @brief  NAND features
 */

int flash_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size);
int flash_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size);
int flash_erase(const struct lfs_config *c, lfs_block_t block);
int flash_sync(const struct lfs_config *c);

extern const struct lfs_config flash_config;

#endif // LFS_INTERFACE_H
