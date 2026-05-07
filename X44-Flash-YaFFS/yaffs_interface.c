/**
 *  @file  yaffs_interface.c
 *
 *  @note
 *          YAFFS interface
 *
 *  @data   04/05/2024
 *  @author Hans
 */



#include "em_device.h"


#define u8  uint8_t

/**
 * @brief  YAFFS Hardware Interface
 */

#if 0
/**
 * @brief  Write a chunk into flash
 *
 * @note   This function writes the specified chunk data and oob/spare data to flash.
 *         This function should return YAFFS_OK on success or YAFFS_FAIL on failure.
 *         If this is a Yaffs2 device, or Yaffs1 with use_nand_ecc set, then this function
 *         must take care of any ECC that is required.
 */
int DRIVE_write_chunk(struct yaffs_dev *dev, int nand_chunk,
            const u8 *data, int data_len,
            const u8 *oob, int oob_len) {

    return YAFFS_OK;
}

/**
 *  @brief  Read function
 *
 *  @note  This function reads the specified chunk data and oob/spare data from flash.
 *         This function should return YAFFS_OK on success or YAFFS_FAIL on failure.
 *         If this is a Yaffs2 device, or Yaffs1 with use_nand_ecc set, then this function
 *         must take care of any ECC that is required and set the ecc_result.
 */
int DRIVE_read_chunk(struct yaffs_dev *dev, int nand_chunk,
            u8 *data, int data_len,
            u8 *oob, int oob_len,
            enum yaffs_ecc_result *ecc_result) {

    return YAFFS_OK;
}


/**
 *  @brief  Erase function
 *
 *  @note   This function erases the specified block. This function should return YAFFS_OK
 *          on success or AFFS_FAIL on failure.
 */
int DRIVE_erase(struct yaffs_dev *dev, int block_no) {

    return YAFFS_OK;
}


/**
 *  @brief  Mark a bad block
 *
 *  @note   This function is only required for Yaffs2 mode. It marks a block bad.
 */
int DRIVE_mark_bad(struct yaffs_dev *dev, int block_no) {

    return YAFFS_OK;
}


/**
 *  @brief  Check is a sector is a a bad block
 *
 *  @note   This function is only required for Yaffs2 mode. It check if it is bad.block.
 */
int DRIVE_check_bad(struct yaffs_dev *dev, int block_no) {

    return YAFFS_OK;
}


/**
 *  @brief  Initialization
 *
 *  @note   This function provides hooks for initialising the flash driver
 */
int DRIVE_initialise(struct yaffs_dev *dev) {

    return YAFFS_OK;
}


/**
 *  @brief  De-initialization
 *
 *  @note   This function provides hooks for deinitialising the flash driver
 */
int DRIVE_deinitialise(struct yaffs_dev *dev) {

    return YAFFS_OK;
}

#endif

