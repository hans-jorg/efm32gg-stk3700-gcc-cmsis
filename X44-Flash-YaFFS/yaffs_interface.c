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

// Prototypes
int nand_write_chunk(   struct yaffs_dev *dev,
                        int nand_chunk,
                        const u8 *data,
                        int data_len,
                        const u8 *oob,
                        int oob_len
                        );

int nand_read_chunk(    struct yaffs_dev *dev, int nand_chunk,
                        u8 *data,
                        int data_len,
                        u8 *oob,
                        int oob_len,
                        enum yaffs_ecc_result *ecc_result
                        );

int nand_erase(         struct yaffs_dev *dev,
                        int block_no
                        );

int nand_mark_bad(      struct yaffs_dev *dev,
                        int block_no
                        );

int nand_check_bad(     struct yaffs_dev *dev,
                        int block_no
                        );

int nand_initialise(    struct yaffs_dev *dev  );

int nand_deinitialise(  struct yaffs_dev *dev  );


struct yaffs_dev nand256 = {
    // Parameter configuration
    .param.name = "nand256",                  // Just a name
    .param.is_yaffs2 = 0,                     // At first, use yaffs1 because it is simpler
    .param.inband_tags = 0.                   // ?????
    .param.total_bytes_per_chunk = 512,       // 512 or 528
    .param.chunks_per_block = 32,             //
    .param.spare_bytes_per_chunk = 16,
    .param.start_block = 0,                    // These give the
    .param.end_block = 2047,                   // capacity of Flash
    .param.n_reserved_blocks = 5,              // >= 2, 5 typical
    .param.n_caches = 0,                       // disable cache to reduce RAM usage
    .param.use_nand_ecc = 0,                   //
    .param.emptyLostAndFound = 1,              // Empty Lost+Found when mounting
    // Driver configuration
    .drv.drv_write_chunk_fn  = 0;
    .drv.drv_read_chunk_fn = 0;
    .drv.drv_erase_fn = 0;
    .drv.drv_mark_bad_fn = 0;
    .drv.drv_check_bad_fn = 0;
    .drv.drv_initialise_fn = 0;
    .drv.drv_deinitialise_fn = 0,
};

/**
 * @brief  YAFFS Hardware Interface
 */

/**
 * @brief  Write a chunk into flash
 *
 * @note   This function writes the specified chunk data and oob/spare data to flash.
 *         This function should return YAFFS_OK on success or YAFFS_FAIL on failure.
 *         If this is a Yaffs2 device, or Yaffs1 with use_nand_ecc set, then this function
 *         must take care of any ECC that is required.
 */
int nand_write_chunk(   struct yaffs_dev *dev,
                        int nand_chunk,
                        const u8 *data,
                        int data_len,
                        const u8 *oob,
                        int oob_len )
{

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
int nand_read_chunk(    struct yaffs_dev *dev, int nand_chunk,
                        u8 *data,
                        int data_len,
                        u8 *oob,
                        int oob_len,
                        enum yaffs_ecc_result *ecc_result )
{

    return YAFFS_OK;
}


/**
 *  @brief  Erase function
 *
 *  @note   This function erases the specified block. This function should return YAFFS_OK
 *          on success or AFFS_FAIL on failure.
 */
int nand_erase(         struct yaffs_dev *dev,
                        int block_no )
{

    return YAFFS_OK;
}


/**
 *  @brief  Mark a bad block
 *
 *  @note   This function is only required for Yaffs2 mode. It marks a block bad.
 */
int nand_mark_bad(      struct yaffs_dev *dev,
                        int block_no
                        )
{

    return YAFFS_OK;
}


/**
 *  @brief  Check is a sector is a a bad block
 *
 *  @note   This function is only required for Yaffs2 mode. It check if it is bad.block.
 */
int nand_check_bad(     struct yaffs_dev *dev,
                        int block_no )
{

    return YAFFS_OK;
}


/**
 *  @brief  Initialization
 *
 *  @note   This function provides hooks for initialising the flash driver
 */
int nand_initialise(    struct yaffs_dev *dev  ) {

    return YAFFS_OK;
}


/**
 *  @brief  De-initialization
 *
 *  @note   This function provides hooks for deinitialising the flash driver
 */
int nand_deinitialise(  struct yaffs_dev *dev  ) {

    return YAFFS_OK;
}
