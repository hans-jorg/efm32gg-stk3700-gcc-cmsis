YaFFS
=====

Introduction
------------

YaFFS (Yet another Flash File System) is a open source middleware for NAND and NOR Flash devices.
It has a dual license (GPL or commercial). Closed source projects must pay a royalty-free
license. Open source projects do not.

It is a mature, fast and robust software. It can run on Linux, RTOS and barebones systems.

From its documentation, on how people treat computers differently from embedded systems

| Issue                         |   Computer                              |     Embedded system                  |
|-------------------------------|-----------------------------------------|--------------------------------------|
| Shut down                     | User expects to do a clean shut down.   | User just unplugs device.            |
| Reinstalling software         | Users expect to have to deal with corruption and reinstall software occasionally. | Users do not expect to deal with upgrades or data loss or corruption. |
| Software Upgrade Failure      | Annoying                                | Can be devastating                   |
| Attitude to data loss         | Annoying, but expected                  | System has failed.                   |
| Cost of failure               | Relatively minor                        | Can shut down factory or even cause   injury/death. |
| Delayed boot due to file system recovery | Annoying, but expected.      | This is as bad as a failure.         |

Now it has two versions:

* Version 1 (yaffs): Old original version
* Version 2 (yaffs2): Current version. faster, supports larger page widths (> 2048),
                      Strictly follows Write Once Rule.

Features
--------

YaFFS does not have allocation tables or similar structures
No tables, then no caching of them
Fewer writes since there is no need to update tables
No deep caching. It uses a small cache, that is needed for alignment issues.
Always flush data immediately hen the file is closed.
Designed for NAND devices. So it has bad block management and wear leveling.
Obeys "Write once rule" (YAFFS2 full, YAFFS1 partial)
Zero overwrites (YAFFS2)

Flash Translation Layer presents the Flash device or Flash File System as an ordinary hard disk.


Architecture
------------

It is built as a layered system.

    | Application          |
    |----------------------|-----> API Interface (POSIX like)
    |  YAFFS               |
    |----------------------|-----> Hardware interface/RTOS interface
    | Hardware |  RTOS     |

In YAFFS, data is stored as objects, that can be of different types, identified by
an **object id (obj_id)**.


* Regular data files
* Directories
* Hard-links
* Symbolic links
* Special objects (pipes, devices etc).

The NAND Flash devices are organized in pages, that is the unit of allocation (**chunk**) and
programming.  The pages (chunks) are grouped in 32 to 128 blocks. The block is the unit of erasure.

NAND devices are shipped with some defects (bad blocks) and can have more during their lifetime.
An Error Correction Code (ECC) is need to detect and correct there errors.

YAFFS2 uses a real log structure, only writing data sequentially. Yaffs1 uses deletion marks,
thus required non sequential write operations. The entries in the log structure are one chunk
 is size and contains either:

* Data chunk.
* Object Header. A descriptor for an object.

Each chunk has tags associated with it, such as:

* Object id (obj_id):
* Chunk id (chunk_id): 0 is the header, other are sequentially assigned.
* Deletion Marker (is_deleted): (Only Yaffs1)
* Byte Count (n_bytes):
* Serial Number (serial number): (Only Yaffs1)


There is a garbage collection mechanism, that erases and reuses outdated (deleted) chunks.
YAFF2 does not uses deletion marks. It uses:

* Sequence number (seq_number): Not the same as the Yaffs1.
* Shrink Header Marker (shrink_header):






API interface
-------------

They are POSIX like.

    yaffs_open  yaffs_read  yaffs_close yaffs_write




RTOS interface
---------------

They must be implemented even for barebone systems.

    void yaffsfs_SetError(int err): Called by Yaffs to set the system error.
    void yaffsfs_Lock(void): Called by Yaffs to lock Yaffs from multi-threaded access.
    void yaffsfs_Unlock(void): Called by Yaffs to unlock Yaffs.
    u32 yaffsfs_CurrentTime(void): Get current time from RTOS.
    void *yaffsfs_malloc(size_t size): Called to allocate memory.
    void yaffsfs_free(void *ptr): Called to free memory.
    void yaffsfs_OSInitialisation(void): Called to initialise RTOS context.
    void yaffs_bug_fn(const char *file_name, int line_no): Function to report a bug.
    int yaffsfs_CheckMemRegion(const void *addr, size_t size, int write_request) : Function
                   to check if accesses to a memory region is valid. This function must return
                   zero if the test passes and negative if it fails.


NAND Hardware Interface
-----------------------

The interface is implemented by a set of functions below. Pointers to them must be set in
the yaffs_dev structure in the drv member, which is a struct yaffs_dev structure,
whose layout is show below.

| Function               |   Description                             |
|------------------------|-------------------------------------------|
| DRIVE_write_chunk      | Write a data chunk                        |
| DRIVE_read_chunk       | Read a data chunk                         |
| DRIVE_erase            | Erase a block                             |
| DRIVE_mark_bad         | Mark a block bad                          |
| DRIVE_check_bad        | Check bad block status of a block         |
| DRIVE_initialise       | Initialisation                            |
| DRIVE_deinitialise     | De-initialise                             |


### Write function

    int DRIVE_write_chunk(struct yaffs_dev *dev, int nand_chunk,
            const u8 *data, int data_len,
            const u8 *oob, int oob_len)

This function writes the specified chunk data and oob/spare data to flash. This function should
return YAFFS_OK on success or YAFFS_FAIL on failure. If this is a Yaffs2 device, or Yaffs1 with
use_nand_ecc set, then this function must take care of any ECC that is required.

### Read function

    int DRIVE_read_chunk(struct yaffs_dev *dev, int nand_chunk,
            u8 *data, int data_len,
            u8 *oob, int oob_len,
            enum yaffs_ecc_result *ecc_result)

This function reads the specified chunk data and oob/spare data from flash. This function should
return YAFFS_OK on success or YAFFS_FAIL on failure. If this is a Yaffs2 device, or Yaffs1
with use_nand_ecc set, then this function must take care of any ECC that is required and set the
ecc_result.


### Erase function

    int DRIVE_erase(struct yaffs_dev *dev, int block_no)

This function erases the specified block. This function should return YAFFS_OK on success or
AFFS_FAIL on failure.


### Back sector info

    int DRIVE_mark_bad(struct yaffs_dev *dev, int block_no);
    int DRIVE_check_bad(struct yaffs_dev *dev, int block_no);

These functions are only required for Yaffs2 mode. They mark a block bad or check if it is bad.

### Initialization and De-initialization

    int DRIVE_initialise(struct yaffs_dev *dev);
    int DRIVE_deinitialise(struct yaffs_dev *dev);

These functions provide hooks for initialising or deinitialising the flash driver.

> The nand interface is implemented in the yaffs.h file. The yaffs_dev structure is implemented
> in yaffs_guts.h. This structure must be initialized before calling the initialize function.

The struct yaffs_drv contains the struct yaffs_driver below, that is a set of pointers to the
functions listed above.


    struct yaffs_driver {
        	int (*drv_write_chunk_fn) (struct yaffs_dev *dev, int nand_chunk,
        				   const u8 *data, int data_len,
        				   const u8 *oob, int oob_len);
        	int (*drv_read_chunk_fn) (struct yaffs_dev *dev, int nand_chunk,
        				   u8 *data, int data_len,
        				   u8 *oob, int oob_len,
        				   enum yaffs_ecc_result *ecc_result);
        	int (*drv_erase_fn) (struct yaffs_dev *dev, int block_no);
        	int (*drv_mark_bad_fn) (struct yaffs_dev *dev, int block_no);
        	int (*drv_check_bad_fn) (struct yaffs_dev *dev, int block_no);
        	int (*drv_initialise_fn) (struct yaffs_dev *dev);
        	int (*drv_deinitialise_fn) (struct yaffs_dev *dev);
     };


References
----------

1. [YaFFS](https://yaffs.net/)
2. [YAFFS  Wikipedia](https://en.wikipedia.org/wiki/YAFFS)
