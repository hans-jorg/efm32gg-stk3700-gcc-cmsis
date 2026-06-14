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
some cases, a test must be done to mark these regions at initialization. But it
can be worse. Some defects can appears during the life time, even when at a number of write operations below the specified value. This errors must be handled and the system must copy its contents to a new region and mark the region with errors as defective.

Summarizing, the table below show the main features of both types of
Flash memories.

| Characteristic      | NAND Flash                 | NOR flash                 |
| ------------------- | -------------------------- | ------------------------- |
| Storage density     | Higher                     | Lower                     |
| Read performance    | Fast                       | Fast\*                    |
| Write performance   | Faster                     | Slower\*                  |
| Erase performance   | Faster (low ms typically)  | Slower (possibly seconds) |
| Storage reliability | Lower (without management) | Better                    |
| Life span           | Higher                     | Lower                     |
| Price               | Lower                      | Higher                    |

- May be slowed by serial access

> One main feature of Flash devices is that it is possible to change a bit
> from 1 to a 0, but to change from 0 to 1, an erase operation is needed
> and it generally erase a large area. On NAND Flash, the erase operation must be done on a large
> chunk of bits, generally called block.

There are many categories of the NAND flash devices. An important category
is the classic one, with page size in the range 256 to 512 and modern
ones, with page size in the range 1024 to 2048, or even larger.

Another category is the type of cells used:
* Single-Level Cell (SLC): Long life, more erase-write cycles, less errors
* Multi-Level Cell (MLC):  Higher capacity
* Triple-Level Cell (TLC): Yet higher capacitiy. Wear out fast.

A NAND Flash can be delivered with some defect cells called Bad Blocks and can develop more defect during its use. The software driver must handle this defects.

Due to the susceptibility to errors, a Error Correcting Code approach is needed. The most
common is a SECDED (Single Error Correction-Double Error Detection Hamming coding. It is also possible to use BCH (Bose-) and Reed-Solomon Codes.

Note that even in large page sizes, the ECC is applied to chunks of 512 bytes.

## NAND Flash Organization

A NAND Flash is composed of a number of blocks. Each block has a certain number
of pages. A page is composed of a data area and an area called spare area (smaller than the data area).

Just for comparison, the main features of two different NAND Flash devices are
shown below.

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

The NAND flash device supports a certain number of program/erase cycles. Generally in the order of 100.000.

The data retention is in the order of 10 years.

There are a lot of restrictions that must be observed during the use:

- On the NAND256, the maximum number of consecutive partial page program is
  three. After this, a block erase must be done.
- The NAND256 uses the 5th byte of the spare area of the first page of a block
  to indicate a Bad Block.
- The W29N01HV uses the 1st byte of the spare area of the first or second page
  of a block to indicate a Bad Block.
- The W29N01HV requires that all pages inside a block are written sequentially,
  but is supports partial write operation limited to 4 write inside a block.
- Modern devices do not accept partial writing operations.
- Some devices presents an additional restriction concerning districts.

Wear Leveling Algorithms are a technique used to spread writes (program) among blocks, avoiding a concentration of write on certain blocks. Two algorithms are
commonly used.

- First Level Wear Leveling. Always write new data on the free block with the
  fewest write cycles.
- Second Level Weaer Leveling. Long lived data is copied elsewhere, so a block
  can be used to store more frequently changed data.

## Middleware

So, the driver software for NAND devices must have:

- Bad blocks management _ Wear Leveling _ Garbage Collection

There are some FREE middleware options for Flash devices:

- FatFS
- YaFFS
- LittleFS
- SPIFFS

YaFFS is the go-to option, complete and widely used, including the Linux kernel, but it demands a lot of RAM.

FatFS is media agnostic and is not suited for NAND devices since it does
not have wear leveling and bad block management. FAT file systems were
not conceived for storing data in devices like NAND flash devices. Another
aspect is that the erase operations are done in a group of pages. To use
FatFS, all these aspects must be considered in the low level firmware
driver.

LittleFS is a fail safe filesystem for microcontroller. It has dynamic
wear leveling, power loss resilience, and has low demand on RAM and ROM.

SPIFFS is used on ESP32 devices. But is slow and has some significant
problems. In some cases, it is beeing replaced by LittleFS.

YaFFS
-----

YAFFS is a middleware that implements an interface to NAND devices. It
features:

- Open source/Commercial license. Closed source projects must pay for
  a license. _ Wear leveling by avoiding repeated erases/writes on the
  same place. _ Bad blocks management.

There are two versions of YAFFS:

* version 1: Supports 512-byte pages. In maintenance mode. Uses deletion
  markers.
* version 2: Supports 512 and 2k pages. Active.

           |-----------------------------------------------|
           |               Application                     |
           |-----------------------------------------------|
           |           POSIX Interface                     |
           |-----------------------------------------------|
           |           YAFFS Direct Interface              |
           |-----------------------------------------------|
           |           YAFFS Core Filesystem               |
           |-----------------------------------------------|
           | RTOS interface |            | Flash interface |
           |----------------|            |-----------------|

Yaffs1 uses deletion markers, which breaks the sequential write rule. Yaffs2 does not use deletion markers and is thus a true log structured file system without ever overwriting the flash. Modern devices forbids overwritting and requires sequential write operations.

>> There is never an overwrite in Yaffs2. Data is written in other area and it is
identified as a new version. The only overwrite in Yaffs1 occurs when writing the deletion marker.

YaFFS store objects in a NAND device. Objects can be:

- Data files _ Directories _ Hand-links _ Symbolic-links _ Special objects
  (pipes, devices, etc.)

All objects have an **obj_id**, an unique integer.

YaFFS handles the objects in **chunks**, an unit of allocation, that is
typically the NAND page size. It also handles bad blocks (old and new)
and ECC.

YaFFS is a middle ware. It sits above a device driver and presents an
Application Programming Interface (API). Actually two API.

A page (chunk in Yaffs slang) can contain:

* Data
* Object Header

Each chunk has tags associated with it:

* Object Id (obj_id): Identifies which object the chunk belongs to.
* Chunk Id (chunk_id): Identifies where in the file this chunk belongs. A chunk_id of zero signifies that this chunk contains an objectHeader. chunk_id==1 signifies the first chunk in the file (ie. At file offset 0), chunk_id==2 is the next chunk and so on.
* Byte Count: (n_bytes): Number of bytes of data if this is a data chunk.

The Yaffs1 uses the following additional fields:

* Deletion Marker (is_deleted): (Yaffs1 only) Shows that this chunk is no longer in use.
* Serial Number (serial_number): (Yaffs1 only)

The Deletion Marker is overwritten. This does not work in modern devices.

### YAFFS RAM demands

Every block demands a yaffs_BlockInfo structure, each about 8-12 byte large
For each file, there must be 30-50 bytes to allocate information about it.
Finally, there is a highly variable part, the Tnodes. For small files the RAM
demands are small. Larger files demands a lot more.

Summarizing, for a 32 MB NAND Flash with 2048  blocks, one has:

* 24 KB (2048*12)
* 32 KB for 1000 files (this can be limited)
* 8 KB for Tnodes

Totalizing, 64 KB!!!!

*
 few

### YAFFS Device Driver

The Yaffs1 flash model expects an area of 16 spare bytes, some of which are used for tags.
The mapping between the spare bytes and the usage of the bytes is specified in yaffs_packed-
tags1.- [ ]

For Yaffs1, the following functions must be defined.

| Function         | Description                      |
|------------------|----------------------------------|
| drv_write_chunk  | Write a data chunk               |
| drv_read_chunk   | Read a data chunk                |
| drv_erase        | Erase a block                    |
| drv_initialise   | Initialisation                   |
| drv_deinitialise | De-initialise                    |

The Yaffs2 uses a similar approach but with additional functions.

| Function          |  Description                                      |
|-------------------|---------------------------------------------------|
| drv_write_chunk   | Write a data chunk                                |
| drv_read_chunk    | Read a data chunk.                                |
| drv_erase         | Erase a block                                     |
| drv_mark_bad      | Mark a block bad                                  |
| drv_check_bad     | Check bad block status of a block.                |
| drv_initialise    | Initialisation.                                   |
| drv_deinitialise  | De-initialise                                     |

The prototypes for these functions are show below.

    int  drv_write_chunk (struct yaffs_dev *dev,
                          int nand_chunk,
                          const u8 \*data,
                          int data_len,
                          const u8 \*oob,
                          int oob_len);

    int drv_read_chunk   (struct yaffs_dev *dev,
                          int nand_chunk,
                          u8 \*data,
                          int data_len,
                          u8 \*oob,
                          int oob_len,
                          enum yaffs_ecc_result \*ecc_result);

    int drv_erase        (struct yaffs_dev \*dev, int block_no);

    int drv_mark_bad     (struct yaffs_dev \*dev, int block_no);
    int drv_check_bad    (struct yaffs_dev \*dev, int block_no);

    int drv_initialise   (struct yaffs_dev \*dev);
    int drv_deinitialise (struct yaffs_dev \*dev);

There are also functions related to tags described below.

>> Where  it is used. Should there be an implementations of them

    int write_chunk_tags (struct yaffs_dev \*dev,
                          int nand_chunk,
                          const u8 \*data,
                          const struct yaffs_ext_tags \*tags);

    int read_chunk_tags  (struct yaffs_dev *dev,
                          int nand_chunk,
                          u8 *data,
                          struct yaffs_ext_tags *tags);

   	int query_block      (struct yaffs_dev *dev,
                          int block_no,
    			                enum yaffs_block_state \*state,
    			                u32 *seq_number);

    int mark_bad          (struct yaffs_dev *dev, int block_no);

All functions return YAFFS_OK on success and YAFFS_FAIL on failure.

A data structure called *yafs_dev* is used to store information about the device and pointers to the driver functions listed above. It must be initialized before its use and must be added to the
YAFFS by the function yaffs_add_device.

The data structure *yaffs_dev* is a huge structure and its most important parts are:

* *params*: Information about the device and configuration of YAFFS.
* *drv*: Pointer to the driver functions.
* *tagger*: Tag manipulation routines (WHO INITIALIZED THIS???)

The parameters to be initialized are listed in Annex A.

Addressin

### ST NAND256W3A

The ST NAND256W3A features the following:

- 32 MBytes (=256 Mbits)
- 528/264 Word Page = (=512+16/256+8).
- The size is not a power of two size due to the spare area (16/8).
- Multiplexed Data/Address lines with up to 16 bit width.
- Support to over than 100.000 erases cycles.
- 8-bit wide data path. Other devices of the family can have 16-bit wide data path.

>> Note: It has at least 2008 valid blocks from the original 2048.

### Summary

| Feature                 | Size        | Unit    |
|-------------------------|-------------|---------|
| 1st Half Page           | 256         | Bytes   |
| 2st Half Page           | 256         | Bytes   |
| Spare area              | 16          | Bytes   |
| _Without spare area_    |             |         |
| Page                    | 512         | Bytes   |
| Block                   | 32          | Pages   |
| Block                   | 16384       | Bytes   |
| Number of blocks        | 2048        | Blocks  |
| Maximum Capacity        | 33.554.432  | Bytes   |
| Maximum Capacity        | 268.435.456 | bits    |
| Maximum Capacity        | 256         | Mbits   |
| Maximum Capacity        | 32          | MBytes  |
| _Spare area as storage_ |             |         |
| Page                    | 528         | Bytes   |
| Block                   | 32          | Pages   |
| Block                   | 16896       | Bytes   |
| Number of blocks        | 2048        | Blocks  |
| Maximum Capacity        | 34.603.008  | Bytes   |
| Maximum Capacity        | 276.824.064 | bits    |
| Maximum Capacity        | 264         | Mbits   |
| Maximum Capacity        | 33          | MBytes  |


The table below shows some important parameters of the NAND256

| Parameter | Value | Unit |
|---------------------------------|-------------|-------| | Page
program time | 200-500 | us | | Block erase time
| 2-3 | ms | | Program/Erase cycles | 100.000 |
cycles| | Data retention | 10 | years |

#### Organization

The memory array is organized as 2048 blocks, and each block has 32
pages. Each page is 528x8 large and is divided in three parts: _ 1st
half page (256 bytes) _ 2nd half page (256 bytes) \* Spare area (16 bytes)

This means that each block has 16KBytes (16384 bytes) of data area and
512 bytes of spare area.

The spare area can be used to store Error Correction Code (ECC) data,
software flags, Bad Block identification or just to increase the
storage area.

> Read operations can be done on pages, but erase operations can only
> be done on blocks.

#### Electronic signature

Manufacturer code: 0x20 Device code: 0x75

#### Errors

The NAND Flash can have bad blocks already identified during manufacturing
or can develop them during its lifetime.

A bad block does not contain an 0xFF (all ones) value in the 6th
byte in the spare area.

> ATTENTION: This value can be overwritten and get lost.

The 256 Mbits device should have at least 2008 valid blocks from the
2048 total.

The bad blocks can be managed using Bad Blocks Management, Block
Replacement or Error Correction Code.

## Addressing

The addresses are input by an up to four bytes (generally, only three are used).

| Bus cycle | IO7 | IO7 | IO5 | IO4 | IO3 | IO2 | IO1 | IO0 |
| --------- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1st       | A7  | A6  | A5  | A4  | A3  | A2  | A1  | A0  |
| 2nd       | A16 | A15 | A14 | A13 | A12 | A11 | A10 | A9  |
| 3rd       | A24 | A23 | A22 | A21 | A20 | A19 | A18 | A17 |
| 4th       | -   | -   | -   | -   | -   | -   | A26 | A25 |

>> Note: There is no A8 because A8 is specified by the command.
>> Block A has A8=0, and Block B, A8=1

## Commands

!!! The operation of a NAND Flash envolves getting the contents of a
page into a page buffer (528x8), !!! exactly the same width of a page,
update it and eventually write it back.!!!!!!

The operation of the device is controlled by commands, generally a
sequence of up to three bytes.

| Command                  | 1st byte | 2nd byte | 3rd byte |
|--------------------------|----------|----------|----------|
| Read A (1st half page)   |    0x00  |     -    |     -    |
| Read B (2nd half page)   |    0x01  |     -    |     -    |
| Read C (Spare area)      |    0x50  |     -    |     -    |
| Read Electronic Signature|    0x90  |     -    |     -    |
| Read Status Register     |    0x70  |     -    |     -    |
| Page Program             |    0x80  |    0x10  |     -    |
| Copy Back Program        |    0x00  |    0x8A  |    0x10  |
| Block Erase              |    0x60  |    0xD0  |     -    |
| Reset                    |    0xFF  |     -    |     -    |

> NOTE 1: A8 is set Low or High by the 0x00 or 0x01 command. It defines
> the half page to be read.

> NOTE 2: The 4th byte is optional for device with 256 MBytes or less.

The bit A8 of the address is used to specify which Area (A or B) to
access. When 0, access is done starting at Area A. When 1, Area B. When
reading the spare area, only address bit A3-A0 are used. Address bits
A7-A4 are ignored.

> The device defaults to Area A after power up or a reset.

> The Read B command in only effective for one operation


### Software Algorithms

### Bad block management

All locations inside a bad block are set to all 1s (=0xFF). After
manufacturing the 6 byte of the
spare indicates a bad block when it is not 0xFF.

> This information can be erased. It is recommended to create a Bad
> Block table.

### Error Correction Code (ECC)

Error Correction Code (ECC) can be used to detect and correct errors.
In NAND Flash devices, it must be used because it is possible that some
memory cells stop working.

There are two types of NAND technology: Single eLevel Cell (SLC) aand
Multi level cell (MLC). MLC devices demand a more sophisticated ECC
technique.

There are many ways to implement ECC

* Hamming:
* Bose-Chaudhuri-Hocquenghem (BCH)
* Red Solomon (RS)

| Data width in bits | ECC bits        |
|--------------------|-----------------|
|           16       |          6      |
|           32       |          7      |
|           64       |          8      |
|          128       |          9      |
|          256       |         10      |
|          512       |         11      |
|         1024       |         12      |
|         2048       |         13      |

For every 2048 bits, it is neccessary to use 22 bits for ECC: 16 for
line parity and 6 for column parity).

### Garbage Collection

When a data page needs to be modified, it is faster to write to the first
available page. The previous page would be marked then as invalid. After
several updates it is necessary to remove invalid pages to free some
memory space. Using Garbage Collection, the valid pages are copied into
a free area and the block containing the invalid pages is erased.

Annex A - Fields of yaffs_dev
-----------------------------

* *name*: An optional name for the device
* *totalBytesPerChunk* The number of bytes in the data area of the flash page. If
inbandTags is not set then this whole area will be used to
store data and the tags will be stored in the spare area. If inbandTags is set then part of this area will be used to store tags and the rest will be used to store data.
* *spareBytesPerChunk*: The number of available bytes in the spare area of the flash page. This excludes space used by ECC, bad block markers etc.
* *startBlock*: The first block in the partition to be used. 0 for first
block.
* *endBlock*: The last block in the partition to be used.
* *nReservedBlocks*: Number of good blocks reserved for garbage collection etc. Needs to be at least 2, but 5 would be a more typical number.
* *endBlock*: The last block in the partition to be used.
* *inbandTags* (Yaffs2 only): Flag indicating whether tags should be stored in the data area. If inbandTags is set then the nShortOpCaches must be enabled (ie nShortOpCaches must be non-zero)
* *use_nand_ecc* (Yaffs1 only): Flag indicating whether the driver performs ECC. If this is zero then Yaffs will perform ECC.
* *noTagsECC* (Yaffs2 only):Flag indicating whether the tags have ECC attached to them.
* *isYaffs2*: Flag indicating if this is using the Yaffs2 mechanism. If this is not set then Yaffs1 mode of operation is provided.
* *nShortOpCaches*: Number of chunks to store in the short operation cache. Zero disables caching. Typical value is 10 to 20 or so. Caching is required for inband tags.
* *emptyLostAndFound*: Flag to delete all files in lost and found on mount.
* *skipCheckpointRead* (Yaffs2 only): Flag to skip reading checkpoint on mount. If
set then a re-scan is forced.
* *skipCheckpointWrite* (Yaffs2 only): Flag to skip writing checkpoint on sync or
unmount.
* *refreshPeriod* (Yaffs2 only): How often Yaffs should do a block refresh.
Values less than 10 disable block refreshing. Typical values would be1000.
* *initialiseNAND*: Pointer to function to initialise flash driver.
* *deinitialiseNAND*: Pointer to function to de-initialise flash driver
* *eraseBlockInNAND*: Pointer to function to erase a flash block
* *writeChunkToNAND (Yaffs1 only)*: Pointer to function to write a chunk
* *readChunkFromNAND (Yaffs1 only)*: Pointer to function to read a chunk.
* *writeChunkWithTagsToNAND (Yaffs2 only): Pointer to function to write a chunk plus
tags
* *readChunkWithTagsFromNAND* (Yaffs2 only): Pointer to function to read a chunk plus tags
* *markNANDBlockBad* (Yaffs2 only): Function to mark a block bad
* *queryNANDBlock* (Yaffs2 only): Function to query a block state
* *gcControl*: Callback function that returns the garbage collector control flags. This is optional.
* *removeObjectCallback*: Callback function called when an object is removed. This is set by the wrapper.
* *markSuperBlockDirty*: Callback function that is called when a clean file system is first modified. This is set by the wrapper.
* *disableSoftDelete* (Yaffs1 only): Debug only. Disables soft deletion if non-zero.
* *useHeaderFileSize* (Debug only): Leave as zero
* *disableLazyLoading* (Debug only): Leave as zero.
* *wideTnodeDisabled* (Debug only): Leave as zero.
* *deferDirectoryUpdate*: Used to defer directory updates.

References
----------

1. [NAND Flash Memories: Bad Block Management and the YAFFS File
   System](https://www.eeweb.com/nand-flash-memories-bad-block-management-and-the-yaffs-file-system/)
2. [A Robust Flash File System Since 2002](https://yaffs.net/)
3. [FatFS - Generic FAT Filesystem Module](http://elm-chan.org/fsw/ff/)
4. [A Robust Flash File System Since 2002](https://yaffs.net/)
5. [LittleFS](https://github.com/littlefs-project/littlefs) 5. [SPIFFS
   (SPI Flash File System)](https://github.com/pellepl/spiffs)
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
13.
