# 43 NAND FLash

## Introduction

There are basically two types of flash memory devices:

- NAND Flash devices
- NOR Flash devices

The main difference is the interface. NAND Flash device are suited for
parallel access, demanding more pins and delivering higher speed. NOR
devices, generally, have a serial interface, typically SD, which is a
development of SPI.

Another very important difference is the susceptibility of error in NAND
Flash devices. Most of them come with some regions marked as defective. In
some cases, a test must be done to mark these regions at
initialization. But it
can be worse. Some defects can appears during the life time, even when
at a number of write operations below the specified value. This errors
must be handled and the system must copy its contents to a new region
and mark the region with errors as defective.

Summarizing, the table below show the main features of both types of
Flash memories.

| Characteristic     | NAND Flash                | NOR flash              |
|--------------------|---------------------------|------------------------|
| Storage density    | Higher                    | Lower                  |
| Read performance   | Fast                      | Fast\*                 |
| Write performance  | Faster                    | Slower\*               |
| Erase performance  | Faster (low ms)           | Slower (seconds)       |
| Storage reliability| Lower (without management)| Better                 |
| Price              | Lower                     | Higher                 |

\* May be slowed by serial access

> One main feature of Flash devices is that it is possible to change a bit
> from 1 to a 0, but to change from 0 to 1, an erase operation is needed
> and it generally erase a large area. On NAND Flash, the erase operation
> must be done on a large chunk of bits, generally called block.

There are many categories of the NAND flash devices. An important category
is the classic one, with page size in the range 256 to 512 and modern
ones, with page size in the range 1024 to 2048, or even larger.

Another category is the type of cells used:
* Single-Level Cell (SLC): Long life, more erase-write cycles, less errors
* Multi-Level Cell (MLC):  Higher capacity
* Triple-Level Cell (TLC): Yet higher capacitiy. Wear out fast.

A NAND Flash can be delivered with some defect cells called Bad Blocks
and can develop more defect during its use. The software driver must
handle this defects.

Due to the susceptibility to errors, a Error Correcting Code approach
is needed. The most common is a SECDED (Single Error Correction-Double
Error Detection Hamming coding. It is also possible to use 
BCH (Bose-) and Reed-Solomon Codes.

Note that even in large page sizes, the ECC is applied to chunks of
512 bytes.

## NAND Flash Organization

A NAND Flash is composed of a number of blocks. Each block has a certain
number of pages. A page is composed of a data area and an area called spare area
(smaller than the data area).

Just for comparison, the main features of two different NAND Flash
devices are shown below.

| Feature            | NAND256          | W29N01HV    | TH58NVG4S0HTA20 |
|--------------------|------------------|-------------|-----------------|
| Size in bits       |    256 Mb        | 1 Gb        | 16 Gb           |
| Size in bytes      | 32 MB            | 256 MB      | 2 GB            |
| # of blocks/device | 64               |             |                 |
| # of pages/block   | 32               | 64          |                 |
| # of blocks/device | 2048 (2008)      | 1024 (1004) | 8192 (8032)     |
| # of pages/device  | 65536            | 65536       | 128K            |
| Page size in bytes | 512+16           | 2048+64     | 4096+256        |
| Page size in bits  | 4096+128         | 65536+512   | 128K+2038       |
| Block size in bytes| 16384            | 132K        |                 |
| Block size in bits | 131072           | 1024        |                 |

## Usage

The NAND flash device supports a certain number of program/erase
cycles. Generally in the order of 100.000.

The data retention is in the order of 10 years.

There are a lot of restrictions that must be observed during the use:

- On the NAND256, the maximum number of consecutive partial page
  program is three. After this, a block erase must be done.
- The NAND256 uses the 5th byte of the spare area of the first page of
  a block to indicate a Bad Block.
- The W29N01HV uses the 1st byte of the spare area of the first or
  second page of a block to indicate a Bad Block.
- The W29N01HV requires that all pages inside a block are written
  sequentially, but is supports partial write operation limited to
  four write inside a block.
- Modern devices do not accept partial writing operations.
- Some devices presents an additional restriction concerning districts.

Wear Leveling Algorithms are a technique used to spread writes (program)
among blocks, avoiding a concentration of write on certain blocks. Two
algorithms are commonly used.

- First Level Wear Leveling. Always write new data on the free block
with the fewest write cycles.
- Second Level Weaer Leveling. Long lived data is copied elsewhere,
so a block can be used to store more frequently changed data.

## Middleware

So, the driver software for NAND devices must have:

- Bad blocks management _ Wear Leveling _ Garbage Collection

There are some FREE middleware options for Flash devices:

* FatFS
* YaFFS
* LittleFS
* SPIFFS

FatFS is media agnostic and is not suited for NAND devices since it does
not have wear leveling and bad block management. FAT file systems were
not conceived for storing data in devices like NAND flash devices. Another
aspect is that the erase operations are done in a group of pages. To use
FatFS, all these aspects must be considered in the low level firmware
driver.

LittleFS is a fail safe filesystem for microcontrollers. It has dynamic
wear leveling, power loss resilience, and has low demand on RAM and ROM.

SPIFFS is used on ESP32 devices. But is slow and has some significant
problems. In some cases, it is being replaced by LittleFS.

YAFFS is the go-to solution in many kind of devices, including Linux kernels.
But it demands a lot of RAM, about 64 KB for a 32 MB Flash device, that is
generally, to big, for most microcontrollers.

LittleFS
--------

*LittleFS* is a middleware that implements an interface to NAND devices designed for 
microcontrollers with emphasys on little.

*Dynamic wear leveling* - littlefs is designed with flash in mind, and provides wear leveling over dynamic blocks. 
*Bad Block Management* - littlefs can detect bad blocks and work around them.
*Bounded RAM/ROM* - littlefs is designed to work with a small amount of memory. RAM usage is strictly bounded, which means RAM consumption does not change as the filesystem grows. The filesystem contains no unbounded recursion and dynamic memory is limited to configurable buffers that can be provided statically.

As a middleware it sits between the application and hardware drivers.

    |  Application  |
    <-----API------->
    |  LittleFS     |
    <---Interface--->
    | Device Driver |
    <--- Registers ->
    |    Hardware   |

The Application Programming Interface (API)
-------------------------------------------

The API consists of:

* functions:
* data structures:


The functions are listed below:

**lfs_format**:Format a block device with the littlefs.

**lfs_mount**: Mounts a littlefs partition

**lfs_unmount**: Unmounts a littlefs partition

**lfs_remove**: Removes a file or directory

**lfs_rename**: Rename or move a file or directory

**lfs_stat**: Find info about a file or directory

**lfs_getattr**: Get a custom attribute

**lfs_setattr**: Set custom attributes

**lfs_removeattr**: Removes a custom attribute

**lfs_file_open**: Open a file

**lfs_file_opencfg**: Open a file with extra configuration

**lfs_file_close**: Close a file

**lfs_file_sync**: Synchronize a file on storage

**lfs_file_read**: Read data from file

**lfs_file_write**: Write data to file

**lfs_file_seek**: Change the position of the file

**lfs_file_truncate**: Truncates the size of the file to the specified size

**lfs_file_tell**: Return the position of the file

**lfs_file_rewind**: Change the position of the file to the beginning of the file

**lfs_file_size**: Return the size of the file

**lfs_mkdir**: Create a directory

**lfs_dir_open**: Open a directory

**lfs_dir_close**: Close a directory

**lfs_dir_read**: Read an entry in the directory

**lfs_dir_seek**: Change the position of the directory

**lfs_dir_tell**: Return the position of the directory

**lfs_dir_rewind**: Change the position of the directory to the beginning of 
the directory

**lfs_fs_stat**: Find on-disk info about the filesystem

**lfs_fs_size**: Finds the current size of the filesystem

**lfs_fs_traverse**: Traverse through all blocks in use by the filesystem

**lfs_fs_mkconsistent**: Attempt to make the filesystem consistent and ready for writing

**lfs_fs_grow**: Grows the filesystem to a new size, updating the superblock with the new block count.

**lfs_migrate**: Attempts to migrate a previous version of littlefs

The functions prototypes are listed below. Generally, a negative returned 
value means an error occurred.

    int lfs_format(lfs_t *lfs, const struct lfs_config *config);
    int lfs_mount(lfs_t *lfs, const struct lfs_config *config);
    int lfs_unmount(lfs_t *lfs);
    int lfs_rename(lfs_t *lfs, const char *oldpath, const char *newpath);
    int lfs_stat(lfs_t *lfs, const char *path, struct lfs_info *info);
    lfs_ssize_t lfs_getattr(lfs_t *lfs, const char *path,
            uint8_t type, void *buffer, lfs_size_t size);
    int lfs_setattr(lfs_t *lfs, const char *path,
          uint8_t type, const void *buffer, lfs_size_t size);
    int lfs_removeattr(lfs_t *lfs, const char *path, uint8_t type);
    int lfs_file_open(lfs_t *lfs, lfs_file_t *file,
          const char *path, int flags);
    int lfs_file_opencfg(lfs_t *lfs, lfs_file_t *file,
          const char *path, int flags,
          const struct lfs_file_config *config);
    int lfs_file_close(lfs_t *lfs, lfs_file_t *file);
    int lfs_file_sync(lfs_t *lfs, lfs_file_t *file);
    lfs_ssize_t lfs_file_read(lfs_t *lfs, lfs_file_t *file,
            void *buffer, lfs_size_t size);
    lfs_ssize_t lfs_file_write(lfs_t *lfs, lfs_file_t *file,
            const void *buffer, lfs_size_t size);
    lfs_soff_t lfs_file_seek(lfs_t *lfs, lfs_file_t *file,
        lfs_soff_t off, int whence);
    int lfs_file_truncate(lfs_t *lfs, lfs_file_t *file, lfs_off_t size);
    int lfs_file_rewind(lfs_t *lfs, lfs_file_t *file);
    lfs_soff_t lfs_file_size(lfs_t *lfs, lfs_file_t *file);
    int lfs_mkdir(lfs_t *lfs, const char *path);
    int lfs_dir_open(lfs_t *lfs, lfs_dir_t *dir, const char *path);
    int lfs_dir_close(lfs_t *lfs, lfs_dir_t *dir);
    int lfs_dir_read(lfs_t *lfs, lfs_dir_t *dir, struct lfs_info *info);
    int lfs_dir_seek(lfs_t *lfs, lfs_dir_t *dir, lfs_off_t off);
    lfs_soff_t lfs_dir_tell(lfs_t *lfs, lfs_dir_t *dir);
    int lfs_dir_rewind(lfs_t *lfs, lfs_dir_t *dir);
    int lfs_fs_stat(lfs_t *lfs, struct lfs_fsinfo *fsinfo);
    lfs_ssize_t lfs_fs_size(lfs_t *lfs);
    int lfs_fs_traverse(lfs_t *lfs, int (*cb)(void*, lfs_block_t), void *data);
    int lfs_fs_mkconsistent(lfs_t *lfs);
    int lfs_fs_gc(lfs_t *lfs);s
    int lfs_fs_grow(lfs_t *lfs, lfs_size_t block_count);
    int lfs_migrate(lfs_t *lfs, const struct lfs_config *cfg);
   
Synchronize a file on storage means that any pending writes are written
out to storage.

Calling the function **lfs_fs_gc** is not required, but it allows the offloading of
expensive janitorial work to a less time-critical code path. It currently:

1. Calls **mkconsistent** if not already consistent
2. Compacts metadata > compact_thresh
3. Populates the block allocator

There are some compiler flags that controls the availability of certain functions
of the API.

* LFS_READONLY
* LFS_NO_MALLOC
* LFS_SHRINKNONRELOCATING
* LFS_MIGRATE

The main data structures are:

**lfs_t**: contains transitory information

**lfs_config**: a read only area that contains the flash parameters

**lfs_info**: 

**lfs_file_t**: 

The most important is **lfs_config**, that must be initialized before
any use of the API. It includes pointer to functions of the device
driver.

    const struct lfs_config cfg = {
        // block device operations
        .read  = user_provided_block_device_read,
        .prog  = user_provided_block_device_prog,
        .erase = user_provided_block_device_erase,
        .sync  = user_provided_block_device_sync,
    
        // block device configuration
        .read_size = 512,
        .prog_size = 512,
        .block_size = 8192,
        .block_count = 2048,
        .cache_size = 16,
        .lookahead_size = 16,
        .block_cycles = 500,
    };

| Parameter | Type        | Description                                                 |
|-----------|-------------|-------------------------------------------------------------|
| read_size | lfs_size_t  | Minimum size of a block read in bytes. All read operations will be a multiple of this value. |
| prog_size | lfs_size_t  | Minimum size of a block program in bytes. All program operations will be a multiple of this value. |
| block_size | lfs_size_t | Size of an erasable block in bytes. This does not impact ram consumption and may be larger than the physical erase size. However, non-inlined files take up at minimum one block. Must be a multiple of the read and program sizes. |
| block_count | lfs_size_t | Number of erasable blocks on the device. Defaults to block_count stored on disk when zero.|
| block_cycles | int32_t | Number of erase cycles before littlefs evicts metadata logs and moves the metadata to another block. Suggested values are in the range 100-1000, with large values having better performance at the cost of less consistent wear distribution. Set to -1 to disable block-level wear-leveling.
| cache_size | lfs_size_t | Size of block caches in bytes. Each cache buffers a portion of a block in RAM. The littlefs needs a read cache, a program cache, and one additional cache per file. Larger caches can improve performance by storing more data and reducing the number of disk accesses. Must be a multiple of the read and program sizes, and a factor of the block size.| 
| lookahead_size | lfs_size_t | Size of the lookahead buffer in bytes. A larger lookahead buffer increases the number of blocks found during an allocation pass. The lookahead buffer is stored as a compact bitmap, so each byte of RAM can track 8 blocks. |
| compact_thresh | lfs_size_t | Threshold for metadata compaction during lfs_fs_gc in bytes. Metadata pairs that exceed this threshold will be compacted during lfs_fs_gc. Defaults to ~88% block_size when zero, though the default may change in the future. Set to -1 to disable metadata compaction during lfs_fs_gc.| 
| read_buffer | lfs_size_t | Optional statically allocated read buffer. Must be cache_size. By default lfs_malloc is used to allocate this buffer. |
| prog_buffer | void * | Optional statically allocated program buffer. Must be cache_size. By default lfs_malloc is used to allocate this buffer. |
| lookahead_buffer | void  * | Optional statically allocated lookahead buffer. Must be lookahead_size. By default lfs_malloc is used to allocate this buffer.|
| name_max | lfs_size_t | Optional upper limit on length of file names in bytes. No downside for larger names except the size of the info struct which is controlled by  the LFS_NAME_MAX define. Defaults to LFS_NAME_MAX or name_max stored on disk when zero.
| file_max  | lfs_size_t | Optional upper limit on files in bytes. No downside for larger files but must be <= LFS_FILE_MAX. Defaults to LFS_FILE_MAX or file_max stored on disk when zero. |
| attr_max  | lfs_size_t | Optional upper limit on custom attributes in bytes. No downside for larger attributes size but must be <= LFS_ATTR_MAX. Defaults to LFS_ATTR_MAX or attr_max stored on disk when zero.|
| metadata_max | lfs_size_t | Optional upper limit on total space given to metadata pairs in bytes. On devices with large blocks (e.g. 128kB) setting this to a low size (2-8kB) can help bound the metadata compaction time. Must be <= block_size. Defaults to block_size when zero. |
| inline_max | lfs_size_t | Optional upper limit on inlined files in bytes. Inlined files live in metadata and decrease storage requirements, but may be limited to improve metadata-related performance. Must be <= cache_size, <= attr_max, and <= block_size/8. Defaults to the largest possible inline_max when zero. Set to -1 to disable inlined files. | disk_version | uint32_t | On-disk version to use when writing in the form of 16-bit major version + 16-bit minor version. This limiting metadata to what is supported by older minor versions. Note that some features will be lost. Defaults to to the most recent minor version when zero.| 

Device Driver
-------------

Below one can find the functions needed for the littlefs to
correctly access the Flash device.

>> Since they are accessed thru pointers, the functions names
>> are irrelevant, but their pointers must correctly initialized

**flash_read**: Read a region in a block. 

**flash_prog**: Program a region in a block. The block must have previously
been erased. May return LFS_ERR_CORRUPT if the block should be considered bad.

**flash_erase**: Erase a block. A block must be erased before being programmed.
The state of an erased block is undefined. May return LFS_ERR_CORRUPT if the block 
should be considered bad.

**flash_sync**: Sync the state of the underlying block device. 

In all of them, negative error codes are propagated to the user.

The function prototypes are listed below.

    int flash_read(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size);
    int flash_prog(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, const void *buffer, lfs_size_t size);
    int flash_erase(const struct lfs_config *c, lfs_block_t block);
    int flash_sync(const struct lfs_config *c);


References
----------

1. [LittleFS](https://github.com/littlefs-project/littlefs) 
2. [SPIFFS (SPI Flash File System)](https://github.com/pellepl/spiffs)
6. [PrimeCell DMA Controller (PL230) Technical Reference Manual
   ](https://developer.arm.com/documentation/ddi0417/a/?lang=en)
7. [NAND FLASH ECC verification principle and  
   implementation](https://en.eeworld.com.cn/news/mcu/eic312689.html)
8. [Micron AN1819 Bad Block Management in NAND Flash Memory
   Introductio](https://d1.amobbs.com/bbs_upload782111/files_46/ourdev_684398U97OG3.pdf)
9. [NAND Flash ECC Algorithm (Error Checking &
   Correction)](https://www.elnec.com/sw/samsung_ecc_algorithm_for_256b.pdf)
10. [NAND flash replacement on EFM32GG starter
    kit](https://community.silabs.com/s/question/0D51M00007xeRcrSAE/nand-flash-replacement-on-efm32gg-starter-kit?language=sv)
11. [NAND128-A
    NAND256-A](https://www.mouser.com/catalog/specsheets/stmicroelectronics_xxx-a.pdf)
12. [NAND128-A, NAND256-A, NAND512-A,
    NAND01G-A](https://www.jotrin.com/product/parts/NAND256W3A?srsltid=AfmBOoqWVFEtRdYKOohBaCeSeRtZ3tlxIbY-aqEn1JG7Wl7xxhRV0o2v)
