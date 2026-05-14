43  NAND FLash
==============

Introduction
------------

There are basically two types of flash memory devices:

* NAND Flash devices
* NOR Flash devices

The main difference is the interface. NAND Flash device are suited for parallel
access, demanding  more pins and delivering higher speed. NOR devices,
generally, have a serial interface, typically SD, which is a development of SPI.

Another very important difference is the susceptibility of error in NAND Flash
devices. Most of them come with some regions marked as defective. In some
cases, a test must be done to mark these regions at initialization. But it can
be worse. Some defects can appears during the life time, even when at a number
of write operations below the specified value. This errors must be handled and
the system must copy its contents to a new region and mark the region with
errors as defective.

Summarizing, the table below show the main features of both types of Flash
memories.

| Characteristic      |  NAND flash                | NOR flash                 |
|---------------------|----------------------------|---------------------------|
| Storage density     | Higher                     | Lower                     |
| Read performance    | Fast                       | Fast*                     |
| Write performance   | Faster                     | Slower*                   |
| Erase performance   | Faster (low ms typically)  | Slower (possibly seconds) |
| Storage reliability | Lower (without management) | Better                    |
| Life span           | Higher                     | Lower                     |
| Price               | Lower                      | Higher                    |

* May be slowed by serial access


> One feature of Flash devices is that it is possible to change a bit from 1 to a 0, but to change from 0 to 1, an erase operation is needed and it generally erase a large ares..

On NAND Flash, the erase operation must be done on a large chunk of bits, generally called block.

There are many categories of the NAND flash devices. An important category is
the classic one, with page size in the range 256 to 512 and modern ones, with page size in the range 1024 to 2048, or even larger.


Middleware
----------

So, the driver software for NAND devices must have:

* Bad blocks management
* Wear Leveling
* Garbage Collection

There are some FREE middleware options for Flash devices:

* FatFS
* YaFFS
* LittleFS
* SPIFFS


FatFS is media agnostic and is not suited for NAND devices since it does not
have wear leveling and bad block management. FAT file systems were not conceived
for storing data in devices like NAND flash devices. Another aspect is that the erase operations are done in a group of pages. To use FatFS, all these aspects must be considered in the low level firmware driver.

LittleFS is a fail safe filesystem for microcontroller. It has dynamic wear leveling, power loss resilience, and has low demand on RAM and ROM.

SPIFFS is used on ESP32 devices. But is slow and has some significant problems. In some cases, it is beeing replaced by LittFS.

YaFFs has two versions. The version 2 is faster, support devices with large pages. Both support wear leveling and bad block management.


Extended Bus Interface (EBI)
----------------------------

Suport to external devices in the EFM32 Giant Gecko family is done by the
Extended Bus Interface (EBI) peripheral. It enables an external device to appear in the memory map.

The devices supported are:

* RAM
* Flash
* TFT

The support for NAND devices is partial. Their ports appears in the memory
space but the addressing inside the device must be done explicitely.

The EBI handles the data and address pins, the read and write pins, the latch
signal pins and the acknowledge pins, including timing.

The EBI support many data and addressing modes:

* D8A8: 8-bin non-multiplexed data bus, 8-bit addressing mode
* D16A16ALE: 16-bit multiplexed data and address mode
* D8A24ALE: 8-bit multiplexed data, 24 bit addressing mode
* D16: 16-bit non multiplexed data, N-bit addressing mode

There are four memory regions used by the EBI.

  * Region 0: 0x8000_0000-0x83FF_FFFF - 64 MByte
  * Region 1: 0x8400_0000-0x87FF_FFFF - 64 MByte
  * Region 2: 0x8800_0000-0x8BFF_FFFF - 64 MByte
  * Region 3: 0x8C00_0000-0x8CFF_FFFF - 64 MByte

  These regions are not cacheable. To run code, these regions can appear at the address ranges below

  * Region 0: 0x1200_0000-0x13FF_FFFF - 32 MByte
  * Region 1: 0x1400_0000-0x15FF_FFFF - 32 MByte
  * Region 2: 0x1600_0000-0x17FF_FFFF - 32 MByte
  * Region 3: 0x1800_0000-0x1FFF_FFFF - 128 MByte

  Alternatively,

  * Region 0: 0x8000_0000-0x8FFF_FFFF - 256 MByte
  * Region 1: 0x9000_0000-0x9FFF_FFFF - 256 MByte
  * Region 2: 0xA000_0000-0xAFFF_FFFF - 256 MByte
  * Region 3: 0xB000_0000-0xBFFF_FFFF - 256 MByte

The corresponding code regions are the same as above.

Except for NAND Flash, the EDI can handle the Wait/Acknowledge pin of an external device. This is configured in the EBI_CTRL register.

There are four sets of registers as show below, one for each region.
But it is possible to use the parameters of Region 0 for all other regions
when the ITS bit in the CTRL register is set to 0.

| Bank 0 or all    | Bank 1           | Bank 2            | Bank 3            |
|------------------|------------------|-------------------|-------------------|
| EBI_ADDRTIMING   | EBI_ADDRTIMING1  | EBI_ADDRTIMING2   | EBI_ADDRTIMING3   |
| EBI_RDTIMING     | EBI_RDTIMING1    | EBI_RDTIMING2     | EBI_RDTIMING3     |
| EBI_WRTIMING     | EBI_WRTIMING1    | EBI_WRTIMING2     | EBI_WRTIMING3     |
| EBI_POLARITY     | EBI_POLARITY1    | EBI_POLARITY2     | EBI_POLARITY3     |



### NAND Flash Support

It supports partially NAND Flash devices, because these devices do not have
address pins. These device have generally 8 or 16-bin data bus. The Error
Correction Code can be generated/verified by the EBI.

The typical connection is (14.3.14)

    |---------------------|                        |-----------------------|
    |                A25  |------------------------| ALE                   |
    |                     |                        |                       |
    |                A24  |------------------------| CLE                   |
    |       EBI           |                        |       NAND            |
    |                 RE  |------------------------| RE    Flash           |
    |                     |                        |                       |
    |                 WE  |------------------------| WE                    |
    |                     |                        |                       |
    |                     |                        |                       |
    |                ADx  |------------------------| IOn                   |
    |                     |                        |                       |
    |               GPIO  |------------------------| WP                    |
    |               GPIO  |------------------------| R/B                   |
    |               GPIO  |------------------------| CE                    |
    |---------------------|                        |-----------------------|

Note:
1 - The R/B signal must be handled by software using GPIO as Write Protect and the Chip Enable pins.
2 - It is possible to the EBI to handle the Chip Select signal (connected to
the Chip Enable) of non standard NAND flash devices.

The ARDY signal can not be used for NAND Flash because the bus access can be blocked for a large amount of time.

The timing information is critical for the working of the EBI. This is
configured in the EBI_WRTIMINGn, EBI_RDTIMINGn and EBI_ADDRTIMINGn  registers. Additionally, there are also EBI_POLARITYn registers. The ITS bit in the
EBI_CTRL register is 0, the values in these register are used in all regions.
If it is 1, these values are used only in Region 0. The other regions use
the values in EBI_WRTIMINGn, EBI_RDTIMINGn and EBI_ADDRTIMINGn, where n is
the number of the region.

Their important fields for timing include:

| Parameter  | Descripton                                                      |
|------------|-----------------------------------------------------------------|
|  ADDRHOLD  |Number of cycles the address setup before WEn is asserted.       |
|  ADDRSETUP |Number of cycles the WEn is held active.                         |
|  WRHOLD    |Number of cycles CSn is held active after the WEn is deassertede |
|  WRSETUP   |Number of cycles the address setup before WEn is asserted.       |
|  WRSTRB    |Number of cycles the WEn is held active.                         |
|  RDHOLD    |Number of cycles CSn is held active after the REn is deasserted. |
|  RDSETUP   |Number of cycles the address setup before REn is asserted.       |
|  RDSTRB    |Number of cycles the REn is held active.                         |



NAND Flash device
-----------------

The EMF32GG-STK3700 depending on the board version has

* an ST NAND256W3A with 8-bit parallel data interface with 256 Mb (=32 MBytes)
* an Winbond W29N01HVDINA with 1 Mb (=128 MBytes).

So both use an 8-bit parallel data interface.

There are differences in capacity, page size and command formats.


### ST NAND256W3A

The ST NAND256W3A features the following:

* 32 MBytes (=256 Mbits)
* 538/264 Word Page = (=512+16/256+8). The size is not a power of two size due to the spare area (16/8).
* Multiplexed Data/Address lines with up to 16 bit width.
* Support to over than 100.000 erases cycles.
* 8-bit wide data path. Other devices of the family can have 16-bit wide data path.
* It has at least 2008 valid blocks from the original 2048.


The table below shows some important parameters of the NAND256

| Parameter                       |   Value     |  Unit |
|---------------------------------|-------------|-------|
| Page program time               |   200-500   |   us  |
| Block erase time                |       2-3   |   ms  |
| Program/Erase cycles            |   100.000   | cycles|
| Data retention                  |        10   | years |


#### Organization

The memory array is organized as 2048 blocks, and each block has 32 pages. Each page is 528x8 large
and is divided in three parts:
* 1st half page (256 bytes)
* 2nd half page (256 bytes)
* Spare area (16 bytes)

This means that each block has 16KBytes (16384 bytes) of data area and 512 bytes of spare area.

The spare area can be used to store Error Correction Code (ECC) data, software flags,
Bad Block identification or just to increase the storage area.

> Read operations can be done on pages, but erase operations can only be done on blocks.


#### Electronic signature

Manufacturer code:  0x20
Device code:        0x75


#### Errors

The NAND Flash can have bad blocks already identified during manufacturing or can develop them during its lifetime.

A bad block(page?) does not contain an 0xFF (all ones) value in the 6th byte in the spare area.

> ATTENTION: This value can be overwritten and get lost.

The 256 Mbits device should have at least 2008 valid blocks from the 2048 total.

The bad blocks can be managed using Bad Blocks Management, Block Replacement or Error Correction Code.

## Addressing
The addresses are input by an up to four bytes (bug generally, only three are used).

|   Bus cycle   | IO7  | IO7  | IO5  | IO4  | IO3  | IO2  | IO1  | IO0  |
|---------------|------|------|------|------|------|------|------|------|
|    1st        | A7   | A6   | A5   | A4   | A3   | A2   | A1   | A0   |
|    2nd        | A16  | A15  | A14  | A13  | A12  | A11  | A10  | A9   |
|    3rd        | A24  | A23  | A22  | A21  | A20  | A19  | A18  | A17  |
|    4th        |  -   |  -   |  -   |  -   |  -   |  -   | A26  | A25  |

## Commands

!!! The operation of a NAND Flash envolves getting the contents of a page into a page buffer (528x8),
!!! exactly the same width of a page, update it and eventually write it back.!!!!!!

The operation of the device is controlled by commands, generally a sequence of up to three bytes.

| Command                  | 1st byte  | 2nd byte  | 3rd byte  |
|--------------------------|-----------|-----------|-----------|
| Read A (1st half page)   |  0x00     |    -      |    -      |
| Read B (2nd half page)   |  0x01     |    -      |    -      |
| Read C (Spare area)      |  0x50     |    -      |    -      |
| Read Electronic Signature|  0x90     |    -      |    -      |
| Read Status Register     |  0x70     |    -      |    -      |
| Page Program             |  0x80     |  0x10     |    -      |
| Copy Back Program        |  0x00     |  0x8A     |  0x10     |
| Block Erase              |  0x60     |  0xD0     |    -      |
| Reset                    |  0xFF     |    -      |    -      |



> NOTE 1: A8 is set Low or High by the 0x00 or 0x01 command. It defines the half page to be read.

> NOTE 2: The 4th byte is optional for device with 256 MBytes or less.


The bit A8 of the address is used to specify which Area (A or B) to access. When 0, access is done
starting at Area A. When 1, Area B. When reading the spare area, only address bit A3-A0 are used.
Address bits A7-A4 are ignored.

> The device defaults to Area A after power up or a reset.

> The Read B command in only effective for one operation



### Summary

|  Feature                |   Size      |  Unit   |
|-------------------------|-------------|---------|
|  1st Half Page          |         256 |  Bytes  |
|  2st Half Page          |         256 |  Bytes  |
|  Spare area             |          16 |  Bytes  |
| *Without spare area*    |             |         |
|  Page                   |         512 |  Bytes  |
|  Block                  |          32 |  Pages  |
|  Block                  |       16384 |  Bytes  |
|  Number of blocks       |        2048 |  Blocks |
|  Maximum Capacity       |  33.554.432 |  Bytes  |
|  Maximum Capacity       | 268.435.456 |  bits   |
|  Maximum Capacity       |         256 |  Mbits  |
|  Maximum Capacity       |          32 |  MBytes |
| *Spare area as starage* |             |         |
|  Page                   |         528 |  Bytes  |
|  Block                  |          32 |  Pages  |
|  Block                  |       16896 |  Bytes  |
|  Number of blocks       |        2048 |  Blocks |
|  Maximum Capacity       |  34.603.008 |  Bytes  |
|  Maximum Capacity       | 276.824.064 |  bits   |
|  Maximum Capacity       |         264 |  Mbits  |
|  Maximum Capacity       |          33 |  MBytes |


### Windond W29N01HVDINA

The ST NAND256W3A features the following:

* 32 MBytes (=256 Mbits)
* 538/264 Word Page = (=512+16/256+8). The size is not a power of two size due to the spare area (16/8).
* Multiplexed Data/Address lines with up to 16 bit width.
* Support to over than 100.000 erases cycles.
* 8-bit wide data path. Other devices of the family can have 16-bit wide data path.
* It has at least 2008 valid blocks from the original 2048.


The table below shows some important parameters of the NAND256


| Parameter                       |   Value     |  Unit |
|---------------------------------|-------------|-------|
| Page program time               |   200-500   |   us  |
| Block erase time                |       2-3   |   ms  |
| Program/Erase cycles            |   100.000   | cycles|
| Data retention                  |        10   | years |

#### Addressing

|          |I/O7 |I/O6 |I/O5 |I/O4 |I/O3 |I/O2 |I/O1 |I/O0 |
|----------|-----|-----|-----|-----|-----|-----|-----|-----|
|1st cycle |  A7 |  A6 |  A5 |  A4 |  A3 |  A2 |  A1 |  A0 |
|2nd cycle |  L  |  L  |  L  |  L  | A11 | A10 |  A9 | A8  |
|3rd cycle | A19 | A18 | A17 | A16 | A15 | A14 | A13 | A12 |
|4th cycle | A27 | A26 | A25 | A24 | A23 | A22 | A21 | A20 |

A11-A0 select the column
A27-A12 select the row.

#### Commands

| Command                     | 1st byte | 2nd byte| 3rd byte|
|-----------------------------|----------|---------|---------|
| Page read                   |  0x00    |   0x30  |    -    |
| Read for copy back          |  0x00    |   0x35  |    -    |
| Read status                 |  0x70    |    -    |    -    |
| Read Electronic Signature/ID|  0x90    |    -    |    -    |
| Read Status Register        |  0x70    |    -    |    -    |
| Page Program                |  0x80    |  0x10   |    -    |
| Copy Back Program           |  0x85    |  0x10   |  0x10   |
| Block Erase                 |  0x60    |  0xD0   |    -    |
| Reset                       |  0xFF    |    -    |    -    |
| Random Data Input*          |  0x85    |    -    |    -    |
| Random Data Output*         |  0x05    |  0xE0   |    -    |
| Read Parameter Page         |  0xEC    |    -    |    -    |


#### Electronic signature

When using adress 0x00, one gets

Manufacturer code:  0xEF
Device ID:          0xF1
Cache supported:    0x00
Oage size,etc:      0x95

When using address 0x20, one gets

ONFI #0:            0x4F
ONFI #1:            0x4E
ONFI #2:            0x46
ONFI #3:            0x49

#### Summary

|  Feature                |    Size      |  Unit   |
|-------------------------|--------------|---------|
|  1st Half Page          |         1024 |  Bytes  |
|  2st Half Page          |         1024 |  Bytes  |
|  Spare area             |           64 |  Bytes  |
| Without spare area      |              |         |
|  Page                   |         2048 |  Bytes  |
|  Block                  |           64 |  Pages  |
|  Block                  |      131.072 |  Bytes  |
|  Number of blocks       |         1024 |  Blocks |
|  Maximum Capacity       |  134.217.728 |  Bytes  |
|  Maximum Capacity       |  268.435.456 |  bits   |
|  Maximum Capacity       |1.073.741.824 |  bits   |
|  Maximum Capacity       |         1024 |  Kbits  |
|  Maximum Capacity       |          128 |  MBytes |
| Spare area as storage   |              |         |
|  Page                   |         2112 |  Bytes  |
|  Block                  |           64 |  Pages  |
|  Block                  |      135.168 |  Bytes  |
|  Number of blocks       |         1024 |  Blocks |
|  Maximum Capacity       |  138.412.032 |  Bytes  |
|  Maximum Capacity       |1.107.296.256 |  bits   |
|  Maximum Capacity       |         1056 |  Kbits  |
|  Maximum Capacity       |          132 |  MBytes |


### Pinout


The pinout is show below.

|   Pin   | Description                                               |
|---------|-----------------------------------------------------------|
| I/O0-7  | Data Input/Outputs, Address Inputs, or Command Inputs     |
| AL      | Address Latch Enable                                      |
| CL      | Command Latch Enable                                      |
| E       | Chip Enable                                               |
| R       | Read Enable                                               |
| RB      | Ready/Busy (open-drain output)                            |
| W       | Write Enable                                              |
| WP      | Write Protect                                             |
                                            |

### Connections

Both device share the same connections.

| Pin     | Board signal|   MCU pin                         |
|---------|-------------|-----------------------------------|
| E       | NAND_PWR_EN |   MCU_PB15                        |
| WP      | NAND_WP#    |   MCU_PD13                        |
| I/O0-7  | NAND_IO0-7  |   MCU_PE8-15/EBI_AD0-7            |
| RB      | NAND_RB#    |   MCU_PD15                        |
| CE      | NAND_CE#    |   MCU_PD14                        |
| RE      | NAND_RE#    |   MCU_PF9/EBI_REn                 |
| WE      | NAND_WE#    |   MCU_PF8/EBI_WEn                 |
| ALE     | NAND_ALE    |   MCU_PC1/EBI_A24                 |
| CLE     | NAND_CLE    |   MCU_PC2/EBI_A25                 |

The interface to the NAND device is built on the use of three ports and pins.
The ports are implemented by the EBI (External Bus Interface).
The EBI peripheral takes care of the Read (RE) and Write (WE) signals.
The A24 and A25 signals are connected to Address Strobe (ALE) and Command Strobe (CLE) signals, respectively.

|   A25 |   A24  |                                                            |
|-------|--------|------------------------------------------------------------|
|     0 |     0  |  Data bus                                                  |
|     0 |     1  |  Address port                                              |
|     1 |     0  |  Command port                                              |
|     1 |     1  |  Invalid                                                   |


|   Port          |  Address       |   Description                             |
|-----------------|----------------|-------------------------------------------|
| DATA_PORT       |  0x8000_0000   | Data bus     (RW)                         |
| ADDRESS_PORT    |  0x8100_0000   | Address port (W)                          |
| COMMAND_PORT    |  0x8200_0000   | Command port (W)                          |

### Operation


| Bus Operation | E# | AL | CL | R# | W# | WP# |   IO0-7   |
|---------------|----|----|----|----|----|-----|-----------|
| Command input |  0 |  0 |  1 |  1 |Rise|  X  | Command   |
| Address input |  0 |  1 |  0 |  1 |Rise|  X  | Address   |
| Data input    |  0 |  0 |  0 |  1 |Rise|  X  |Data input |
| Data output   |  0 |  0 |  0 |Fall|  1 |  X  |Data output|
| Write protect |  X |  X |  X |  X |  X |  0  |    X      |
| Standby       |  1 |  X |  X |  X |  X |  X  |    X      |



> Note: Only address bits 25 to 24 are used by the EBI to interface the Flash device. The address bis 31 to 26 are used internally. The remaining are don't
care. It is recommended to be used as zero. Do not confuse these address to the
internal address used to access a specific data inside the NAND Flash device.


Additionally, some signals are controlled thru the GPIO.

| Signal      | GPIO pin | Direction| Active|Description                       |
|-------------|----------|----------|------------------------------------------|
| NAND_PWR_EN | MCU_PB15 | Output   | High  | Turn On switch to power device   |
| NAND_WP#    | MCU_PD13 | Output   | Low   | Enable program and eraserations  |
| NAND_RB#    | MCU_PD15 | Input    | High  | Status: Ready (1)/Busy(0)        |
| NAND_CE#    | MCU_PD14 | Output   | Low   | Enable device                    |


### Timing

Configuration for the maximal clock frequency (48 MHz). This corresponds to
a period of 20.8 ns.

| Parameter | NAND256W3A | W29N01HVDINA | NAND256W3A | W29N01HVDINA  |
|-----------|------------|--------------|------------|---------------|
|  WRHOLD   |            |              |      1     |               |
|  WRSETUP  |            |              |      0     |               |
|  WRSTRB   |            |              |      2     |               |
|  RDHOLD   |            |              |      1     |               |
|  RDSETUP  |            |              |      0     |               |
|  RDSTRB   |            |              |      2     |               |
|  ADDRHOLD |            |              |      0     |               |
|  ADDRSETUP|            |              |      0     |               |

### Polarity

Polarity is active low for all control signals.


### Software Algorithms

### Bad block management

All locations inside a bad block are set to all 1s (=0xFF). After manufacturing the 6 byte of the
 spare indicates a bad block when it is not 0xFF.

> This information can be erased. It is recommended to create a Bad Block table.

When detecting an error, by testing the Status Register, there is a recommended procedure for
 each type of failed operation.

| Operation      |  Recommended Procedure               |
|----------------|--------------------------------------|
| Erase          | Block Replacement                    |
| Program        | Block Replacement or ECC             |
| Read           | ECC                                  |


### Error Correction Code (ECC)

Error Correction Code (ECC) can be used to detect and correct errors.
In NAND Flash devices, it must be used because it is possible that some
memory cells stop working.

There are two types of NAND technology: Single eLevel Cell (SLC) aand Multi level cell (MLC). MLC devices demand a more sophisticated ECC technique.

There are many ways to implement ECC

* Hamming: 
* Bose-Chaudhuri-Hocquenghem (BCH)
* Red Solomon (RS)

|  Data width in bits |  ECC bits       |               |
|---------------------|-----------------|---------------|
|            16       |         6       |               |
|            32       |         7       |               |
|            64       |         8       |               |
|           128       |         9       |               |
|           256       |        10       |               |
|           512       |        11       |               |
|          1024       |        12       |               |
|          2048       |        13       |               |



For every 2048 bits, it is neccessary to use 22 bits for ECC: 16  for line parity and 6 for column parity).


### Garbage Collection

When a data page needs to be modified, it is faster to write to the first available page.
The previous page would be marked then as invalid. After several updates it is necessary to remove
invalid pages to free some memory space. Using Garbage Collection, the valid pages are copied into
a free area and the block containing the invalid pages is erased.

YaFFS
-----

YAFFS is a middleware that implements an interface to NAND devices. It features:

* Open source/Commercial license. Closed source projects must pay for a license.
* Wear leveling by avoiding repeated erases/writes on the same place.
* Bad blocks management.


There are two versions of YAFFS:

* version 1: Supports 512-byte pages. In maintenance mode. Uses deletion markers.
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
         |     RTOS       |            |      Flash      |
         |----------------|            |-----------------|


YaFFS store objects in a NAND device. Objects can be:

* Data files
* Directories
* Hand-links
* Symbolic-links
* Special objects (pipes, devices, etc.)

All objects have an **obj_id**, an unique integer.

YaFFS handles the objects in **chunks**, an unit of allocation, that is typically the NAND page
size.  It also handles bad blocks (old and new) and ECC.


Direct Memory Access
--------------------

Direct Memory Access (DMA) is a mechanism for transfering data between a peripheral and memory
without demanding the processor to handle the transfer.

The DMA controller in the EFM32GG is an licensed implementation of the PL230 uDMA developed by ARM.

The DMA controller in the EFM32GG features:

* 12 channels
* Transfer from Memory (RAM/EBI/Flash) to peripheral
* Transfer from Peripheral to memory (RAM/EBI)
* Transfer from Memory (RAM/EBI/Flash) to memory (RAM/EBI)
* Use one descriptor (primary) or two descriptors (primary/alternate)
* Transfer size: 8, 16 or 32 bits
* Transfer modes: basic, ping-pong, scatter-gatter (WTF)
* Programmable transfer length
* Looped transfers (Channels 1 and 2)
* 2D transfers (Channel 1)
* Interrupt upon transfer clompletion

The DMA controller is configured by:
* Setting registers of the DMA peripheral
* Setting descriptor in system memory



### Channel control data structure

There must be a contiguous area that both the DMA controller and the host processor can access.

This area must start at an address multiple of 256.
The controller uses the lower 8 address bits to enable it to access all of the elements
in the structure and therefore the base address must be at 0xXXXXXX00. It address must be written to
the DMA_CTRLBASE register.

Its size must lie between 16 bytes, when using only Channel 0, to 384 when using all primary and
alternate channels. For example, when using Channels 0 to 3, only 64 bytes are needed.


| Offset  | Description                                      |
|---------|--------------------------------------------------|
|      +0 | Primary Channel 0                                |
|     +10 | Primary Channel 1                                |
|     +20 | Primary Channel 2                                |
|     +30 | Primary Channel 3                                |
|     +40 | Primary Channel 4                                |
|     +50 | Primary Channel 5                                |
|     +60 | Primary Channel 6                                |
|     +70 | Primary Channel 7                                |
|     +80 | Primary Channel 8                                |
|     +90 | Primary Channel 9                                |
|     +A0 | Primary Channel 10                               |
|     +B0 | Primary Channel 11                               |
|     +C0 | One past the end                                 |
|    ...  | ...                                              |
|    +100 | Primary Channel 0                                |
|    +110 | Primary Channel 1                                |
|    +120 | Primary Channel 2                                |
|    +130 | Primary Channel 3                                |
|    +140 | Primary Channel 4                                |
|    +150 | Primary Channel 5                                |
|    +160 | Primary Channel 6                                |
|    +170 | Primary Channel 7                                |
|    +180 | Primary Channel 8                                |
|    +190 | Primary Channel 9                                |
|    +1A0 | Primary Channel 10                               |
|    +1B0 | Primary Channel 11                               |
|    +1C0 | One paste the end                                |

Each channel structure is 16 bytes long and has the following format.

| Offset  |   Description                                    |
|---------|--------------------------------------------------|
|     +0  | Source End Pointer (*src\_data\_end_pt*)         |
|     +4  | Destination End Pointer (*dst\_data\_end_pt*)    |
|     +8  | Control                                          |
|     +C  | Not used. Data can be store in it                |

The pointer fields are not written by the controller.

The control word is the only field updated by the controller
and has the following format

| Bit field | Description                                    |
|-----------|------------------------------------------------|
|  31-30    | Destination address increment (*dst\_inc*)     |
|  29-28    | Destination data size (*dst\_size*)            |
|  27-26    | Source address increment (*src\_inc*)          |
|  25-24    | Source size (*src\_size*)                      |
|  23-22    | Not used. Must be 0b00                         |
|  21-21    | Destination HPROT (*dst\_prot\_ctrl*)          |
|  20-19    | Not used. Must be 0b00                         |
|  18-18    | Source HPROT (*src\_prot\_ctrl*)               |
|  17-14    | M. Arbitrate after 2^M DMA Transfers (max 1024)|
|  13- 4    | Number of transfers minus one (*n_minus_1*)    |
|   3- 3    |  (*next\_userburst*)                           |
|   2- 0    | Operating mode (*cycle_ctrl*)                  |


The increments are defined by respective source or destination filter.

| Value    | Byte (0b00)   | Halfword(0x01)|   Word (0b10) |
|----------|---------------|---------------|---------------|
|   0b00   | byte          | reserved      | reserved      |
|   0b01   | halfword      | halfword      | reserved      |
|   0b10   | word          | word          | word          |
|   0b11   | no increment  | no increment  | no increment  |

| Mode  | Description                                      |
|-------|--------------------------------------------------|
| ob000 | Stop                                             |
| 0b001 | Basic                                            |
| 0b010 | Auto-request                                     |
| 0b011 | Ping-Pong                                        |
| 0b100 | Memory scatter/gather. Primary.                  |
| 0b101 | Memory scatter/gather. Alternate.                |
| 0b110 | Peripheral scatter/gather. Primary               |
| 0b111 | Peripheral scatter/gather. Alternate.            |

Calculation of source address and destination address

| src_inc  | dst_inc | Source Address                    | Destination Address                |
|----------|---------|-----------------------------------|------------------------------------|
|  0b00    |  0b00   | src_data_end_ptr - n_minus_1      | dst_data_end_ptr - n_minus_1       |
|  0b01    |  0b01   | src_data_end_ptr - (n_minus_1<<1) | dst_data_end_ptr - (n_minus_1<<1)  |
|  0b10    |  0b10   | src_data_end_ptr - (n_minus_1<<2) | dst_data_end_ptr - (n_minus_1<<2)  |
|  0b11    |  0b11   | src_data_end_ptr                  | dst_data_end_ptr                   |


References
----------

1. [NAND Flash Memories: Bad Block Management and the YAFFS File System](https://www.eeweb.com/nand-flash-memories-bad-block-management-and-the-yaffs-file-system/)
2. [A Robust Flash File System Since 2002](https://yaffs.net/)
2. [FatFS - Generic FAT Filesystem Module](http://elm-chan.org/fsw/ff/)
3. [A Robust Flash File System Since 2002](https://yaffs.net/)
4. [LittleFS](https://github.com/littlefs-project/littlefs)
5. [SPIFFS (SPI Flash File System)](https://github.com/pellepl/spiffs)
6. [PrimeCell DMA Controller (PL230) Technical Reference Manual ](https://developer.arm.com/documentation/ddi0417/a/?lang=en)
7. [NAND FLASH ECC verification principle and implementation](https://en.eeworld.com.cn/news/mcu/eic312689.html)
8. [Micron AN1819 Bad Block Management in NAND Flash Memory
Introductio](https://d1.amobbs.com/bbs_upload782111/files_46/ourdev_684398U97OG3.pdf)
9. [NAND Flash ECC Algorithm (Error Checking & Correction)](https://www.elnec.com/sw/samsung_ecc_algorithm_for_256b.pdf)
10. [NAND flash replacement on EFM32GG starter kit](https://community.silabs.com/s/question/0D51M00007xeRcrSAE/nand-flash-replacement-on-efm32gg-starter-kit?language=sv)
11. [NAND128-A NAND256-A](https://www.mouser.com/catalog/specsheets/stmicroelectronics_xxx-a.pdf)
12. [NAND128-A, NAND256-A, NAND512-A, NAND01G-A](https://www.jotrin.com/product/parts/NAND256W3A?srsltid=AfmBOoqWVFEtRdYKOohBaCeSeRtZ3tlxIbY-aqEn1JG7Wl7xxhRV0o2v)
9.
