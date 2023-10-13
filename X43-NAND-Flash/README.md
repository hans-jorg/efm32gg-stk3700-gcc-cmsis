43  NAND FLash
==============

Introduction
------------

There are basically two types of flash memory devices:

* NAND Flash devices
* NOR Flash devices

The main difference is the interface. NAND Flash device are suited for parallel access, demanding more pins and delivering higher speed. NOR devices, generally, have a serial interface, typically SD, that is a development of SPI.



NAND Flash device
-----------------

The ST NAND256W3A features the following:

* 32 MBytes (=256 Mbit)
* 538/264 Word Page = (=512+16/256+8). The non power of two sizes area due to the spare area(16/8).
* Multiplexed Data/Address lines with up to 16 bit width.
* support to over that 100000 erases cycles.
* 8-bit wide data path. Other devices can have 16-bit wide data path.
* It has at least 2008 valid blocks from 2048.

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
| VDD     | Supply Voltage                                            |
| VSS     | Ground                                                    |
| NC      | Not Connected Internally                                  |
| DU      | Do Not Use                                                |


| Bus Operation | E# | AL | CL | R# | W# | WP# |   IO0-7   |
|---------------|----|----|----|----|----|-----|-----------|
| Command input |  0 |  0 |  1 |  1 |Rise|  X  | Command   |
| Address input |  0 |  1 |  0 |  1 |Rise|  X  | Address   |
| Data input    |  0 |  0 |  0 |  1 |Rise|  X  |Data input |
| Data output   |  0 |  0 |  0 |Fall|  1 |  X  |Data output|
| Write protect |  X |  X |  X |  X |  X |  0  |    X      |
| Standby       |  1 |  X |  X |  X |  X |  X  |    X      |

The memory array is organized in blocks, where each block has 32 pages. Each page is 528x8 large.


>> The NAND Flash can have bad blocks already identified during manufacturing or can develop them during its lifetime.


The 256 Mbits devices has at least 2008 valid blocks from a 2048 blocks. The bad blocks can be managed using Bad Blocks Management, Block Replacement or Error Correction Code.

Each page widht is divided in three parts:
* 1st half page (256 bytes)
* 2nd half page (256 bytes)
* Spare area (15 bytes)

The spare area can be used to store Error Correction Code (ECC) data, software flags, Bad Block identification or just increase the storage area.

The operation of a NAND Flash envolves getting the contents of a page into a page buffer (528x8), exactly the same width of a page, update it and eventually write it back.

The operation of the device is controlled by commands, generally a sequence of up to thre bytes.

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

The addresses are input by an up to four bytes (bug generally, only three are used).

|   Bus cycle   | IO7  | IO7  | IO5  | IO4  | IO3  | IO2  | IO1  | IO0  |
|---------------|------|------|------|------|------|------|------|------|
|    1st        | A7   | A6   | A5   | A4   | A3   | A2   | A1   | A0   |
|    2nd        | A16  | A15  | A14  | A13  | A12  | A11  | A10  | A9   |
|    3rd        | A24  | A23  | A22  | A21  | A20  | A19  | A18  | A17  |
|    4th        |  -   |  -   |  -   |  -   |  -   |  -   | A26  | A25  |

> NOTE 1: A8 is set Low or High by the 0x00 or 0x01 command. It defines the half page to be read.

> NOTE 2: The 4th byte is optional for device with 256 MBytes or less.

The address can be interpreted as composed of many fields:

| Address bits   |  Description                    |
|----------------|---------------------------------|
|   A7-A0        | Column address                  |
|   A8           | Half page                       |
|   A13-A9       | Address in block                |
|   A26-A14      | Block address                   |
|   A26-A9       | Page address                    |


When reading the spare area, only address bit A3-A0 are used. Address bits A7-A4 are ignored.

> The device defaults to Area A after power up or a reset.

> The Read B command in only effective for one operation

There are the following types of read operation available.

* Random read. The first read after a command. It tranfers data from page to page buffer.
* Page Read. The following read operations get data from page buffer. Pulsing RE make the next read get the byte in next column.
* Sequential Row Read. After data in last column is output, by pulsing RE, the next page is automatically loaded into the page buffer. This is limited to the current block!!! To terminate the operation, set CD high for a while.

> The Ready/Busy signal indicates that the transfer to the page buffer is completed.


The sequence for reading the Area A (1st half page) is

1. Send command 0x00
2. Send address
3. Read data
4. 

The Page Program (command 0x80) is the standard way to program data into the memory array.

> There is a limit of three consecutive partial program in the same page. After it, a Block Erase must be issued.

### Programming (Writing) the device

The sequence for programming the area A (1st half page) is

1. Send command 0x00
2. Send command 0x80
3. Send address
4. Send data
5. Send command 0x10
6. Send command 0x00 (Select Area A). Not neccessary.
7. Send command 0x80
8. Send address
9. Read data
10. ....

The sequence for programming the area B (2nd half page) is

1. Send command 0x01
2. Send command 0x80
3. Send address
4. Send data
5. Send command 0x10
6. Send command 0x01
7. Send command 0x80
8. Send address
9. Write data
10. ....

The sequence for programming the area B (2nd half page) is

1. Send command 0x50
2. Send command 0x80
3. Send address
4. Send data
5. Send command 0x10
6. Send command 0x50
7. Send command 0x80
8. Send address
9. Write data
10. ......


### Read Status

### Read Electronic Signature


1. Send command 0x90
2. Read two bytes

### Software Algorithms

### Bad block management

All locations inside a bad block are set to all 1s (=0xFF). After manufacturing the 6 byte of the spare indicates a bad block when it is not 0xFF.

> This information can be erased. It is recommended to create a Bad Block table.

When detecting an error, by testing the Status Register, there is a recommended procedure for each type of failed operation.

| Operation      |  Recommended Procedure               |
|----------------|--------------------------------------|
| Erase          | Block Replacement                    |
| Program        | Block Replacement or ECC             |
| Read           | ECC                                  |


### Error Correction Code (ECC)

Error Correction Code (ECC) can be used to detect and correct errors. For every 2048 bits in the device, it is neccessary to use 22 bits for ECC: 16  for line parity and 6 for column parity).


### Garbage Collection

When a data page needs to be modified, it is faster to write to the first available page.
The previous page would be marked then as invalid. After several updates it is necessary to remove invalid pages to free some memory space. Using Garbage Collection, the valid pages are copied into a free area and the block containing the invalid pages is erased.


### Wear-Leveling Algorithm

The number of write operationg in Flash devices is limited. NAND Flash memories are programmed and erased by Fowler-Nordheim tunneling using a high voltage. Exposing the device to a high voltage for extended periods can cause the oxide layer to be damaged.

To extended the life of the device, an algorithm is used to an equal use of writing operations of the pages.

There are tow wear-level procedures:

* First Level Wear-Leveling: New data is written to the free blocks that have the fewest writing cycles.
* Second Level Wear-Leveling: Bkocks with long lived data gives room to new data, after their contents are written to other blocks.



NAND Flash on the STK3700
-------------------------

The EMF32GG-STK3700 has an 32 MBytes (=256 Mbit) NAND Flash device  (ST NAND256W3A), with an 8 bit parallel interface and support for ECC (E Correction Code).

The device can be power up or down by the NAND_PWR_EN (PB15).

The EFM32GG family has a EBI (External Bus Interface) peripheral that handles the interface and map the device into the MCU memory. It uses a multiplexing mechanism to reduce the pin count.

The pins used for this interface are show below.


| MCU Pin | PCB Signal  |  NAND Signal | MCU Signal  | Description                   |
|---------|-------------|--------------|-------------|-------------------------------|
| PD13    | NAND_WP#    |    WP#       | GPIO_PD13   | Write Protect                 |
| PD14    | NAND_CE#    |    E#        | GPIO_PD14   | Chip Enable                   |
| PD15    | NAND_R/B#   |    R/B#      | GPIO_PD15   | Ready/Busy indicator          |
| PC1     | NAND_ALE    |    AL        | EBI_A24     | Address Latch Enable/A24      |
| PC2     | NAND_CLE    |    CL        | EBI_A25     | Command Latch Enable/A25      |
| PF8     | NAND_WE#    |    W#        | EBI_WEn     | Write Enable                  |
| PF9     | NAND_RE#    |    R#        | EBI_REn     | Read Enable                   |
| PE15    | NAND_IO7    |    I/O7      | EBI_AD7     | I/O bit #7                    |
| PE14    | NAND_IO6    |    I/O6      | EBI_AD6     | I/O bit #6                    |
| PE13    | NAND_IO5    |    I/O5      | EBI_AD5     | I/O bit #5                    |
| PE12    | NAND_IO4    |    I/O4      | EBI_AD4     | I/O bit #4                    |
| PE11    | NAND_IO3    |    I/O3      | EBI_AD3     | I/O bit #3                    |
| PE10    | NAND_IO2    |    I/O2      | EBI_AD2     | I/O bit #2                    |
| PE9     | NAND_IO1    |    I/O1      | EBI_AD1     | I/O bit #1                    |
| PE8     | NAND_IO0    |    I/O0      | EBI_AD0     | I/O bit #0                    |
| PB15    | NAND_PWR_EN |      -       | GPIO_PB15   | Power enable (TS5A3166 switch)|


Some pins are controlled directly by the GPIO module. Others by the EBI module. The EBI_ROUTE register controls
which pins are used.

    | Field        |  Bits    |  Description                           |  Value       |
    |--------------|----------|----------------------------------------|--------------|
    | LOCATION     | 30-28    | LOC# for EBI_IOn pins                  |   0,1,2      |
    | ALB          | 17-16    | EBI_A lower pin enabled (0,8,16,*24*)  |   3          |
    | APEN         | 22-18    | EBI bit field for A A25-A24            |  26          |
    | NANDPEN      |  12      | NANDREn and NANDWEn pins enabled       |   1          | 





Address map

 |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|19|09|08|07|06|05|04|03|02|01|00|
 |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
 |     BYTE #3           |     BYTE #2           |     BYTE #1           |     BYTE #0           |

A 256 Mbit/32MByte device needs 24 bits (A0 to A23) to address the full capacity of the device. This corresponds to three bytes.
The next address bits, A24 and A25, are used to drive the Address Latch Enable (ALE) and command Latch Enable (CLE) lines. So, when addressing memory with the A24 bit set, the ALE line is set and the data transmitted is interpreted as an address. Similarly, when addressing with the A25 bit set, the CLE line is set.

The device register are mapped into the MCU memory map.

| Register     |  Address                   |
|--------------|----------------------------|
| Data         | 0x8000_0000                |
| Address      | 0x8100_0000                |
| Command      | 0x8200_0000                |


The External Bus Interface (EBI)
--------------------------------

The external bus Interface (EBI) support devices with up to 28 address lines and up to 16-bin data lines in multiplexed and non multiplexed mode.

It supports two kinds of devices:

* SRAM/Flash
* TFT RGB



There are four EBI regions that can be used to access the NAND Flash device.

| EBI Region   |  Address Range             |    Size    |
|--------------|----------------------------|------------|
|    #0        | 0x8000_0000 - 0x83FF_FFFF  |   64 MB    |
|    #1        | 0x8400_0000 - 0x87FF_FFFF  |   64 MB    |
|    #2        | 0x8800_0000 - 0x8BFF_FFFF  |   64 MB    |
|    #3        | 0x8C00_0000 - 0x8FFF_FFFF  |   64 MB    |


Pinout




FatFS
-----

FatFS is a middleware. It implements a FAT system on a multitude of storage devices, like MMC/SD/TF,U-Disk,NAND Flash).

It used a layered approach. FatFS provides an interface to application thru an Application Programming Interface (API). 

    Application:  f_open, f_read, 
    FatFS:        disk_read, disk_write, get_fattime,
    Interface:    (SPI,I2C,USB,SD,NAND)
    Hardware:     (MMC/SD/TF,USB-MSC, NAND-FTL)



The API (Application Programming Interface) includes the functions below. Most names are similar to the POSIX
system calls or POSIX commands. Not all calls are not always available. Some compilation flags can enable or 
disable them as described below.

    f_mount     f_open      f_close     f_read      f_write     f_sync      f_lseek     f_opendir
    f_closedir  f_readdir   f_findfirst f_findnext  f_stat      f_getfree   f_truncate  f_unlink
    f_mkdir     f_rename    f_chdir     f_chdrive   f_getcwd    f_chmod     f_utime     f_getlabel
    f_setlabel  f_expand    f_forward   f_mkfs      f_fdisk     f_putc      f_puts      f_printf    
    f_gets


*FatFS* is modular. There is a set of compilation flags:

FF_FS_MINIMIZE	   [0-3] Enable or disable diverse API functions (values can be 0, 1, 2, 3)
FF_FS_READONLY	   
FF_USE_STRFUNC	   
FF_FS_RPATH        [0-2] 
FF_USE_FIND	       Enable f_findfirst, f_findnext
FF_USE_CHMOD       Enable f_chmod
FF_USE_EXPAND	   Enable f_expand
FF_USE_LABEL       
FF_USE_MKFS	       Enable f_mkfs
FF_USE_FORWARD     
FF_MULTI_PARTITION 
FF_PRINT_FLOAT     Enable f_printf
FF_STRF_ENCODE     [0-3]
FF_PRINT_LLI       Enable long long support in f_printf




To access the hardware, it uses an set of functions. 

    Always                   disk_status, disk_initialize, disk_read
    FF_FS_READONLY == 0      disk_write, disk_ioctl, get_fattime
    FF_USE_MKFS == 1         additional functions of disk_ioctl
    FF_USE_LFN != 0          ff_uni2oem, ff_oem2uni, ff_wtoupper
    FF_FS_REENTRANT == 1     ff_mutex_create, ff_mutex_delete, ff_mutex_take, ff_mutex_give
    FF_USE_LFN == 3          ff_mem_alloc, ff_mem_free


Annex A - EBI Pin Usage
 
|  Pin        |LOC0 | LOC1 | LOC2 | Descriptiojn                                                |
|-------------|-----|------|------|-------------------------------------------------------------|
| EBI_A00     | PA12| PA12 | PA12 | External Bus Interface (EBI) address output pin 00.         |
| EBI_A01     | PA13| PA13 | PA13 | External Bus Interface (EBI) address output pin 01.         |
| EBI_A02     | PA14| PA14 | PA14 | External Bus Interface (EBI) address output pin 02.         |
| EBI_A03     | PB9 | PB9  | PB9  | External Bus Interface (EBI) address output pin 03.         |
| EBI_A04     | PB10| PB10 | PB10 | External Bus Interface (EBI) address output pin 04.         |
| EBI_A05     | PC6 | PC6  | PC6  | External Bus Interface (EBI) address output pin 05.         |
| EBI_A06     | PC7 | PC7  | PC7  | External Bus Interface (EBI) address output pin 06.         |
| EBI_A07     | PE0 | PE0  | PE0  | External Bus Interface (EBI) address output pin 07.         |
| EBI_A08     | PE1 | PE1  | PE1  | External Bus Interface (EBI) address output pin 08.         |
| EBI_A09     | PE2 | PC9  | PC9  | External Bus Interface (EBI) address output pin 09.         |
| EBI_A10     | PE3 | PC10 | PC10 | External Bus Interface (EBI) address output pin 10.         |
| EBI_A11     | PE4 | PE4  | PE4  | External Bus Interface (EBI) address output pin 11.         |
| EBI_A12     | PE5 | PE5  | PE5  | External Bus Interface (EBI) address output pin 12.         |
| EBI_A13     | PE6 | PE6  | PE6  | External Bus Interface (EBI) address output pin 13.         |
| EBI_A14     | PE7 | PE7  | PE7  | External Bus Interface (EBI) address output pin 14.         |
| EBI_A15     | PC8 | PC8  | PC8  | External Bus Interface (EBI) address output pin 15.         |
| EBI_A16     | PB0 | PB0  | PB0  | External Bus Interface (EBI) address output pin 16.         |
| EBI_A17     | PB1 | PB1  | PB1  | External Bus Interface (EBI) address output pin 17.         |
| EBI_A18     | PB2 | PB2  | PB2  | External Bus Interface (EBI) address output pin 18.         |
| EBI_A19     | PB3 | PB3  | PB3  | External Bus Interface (EBI) address output pin 19.         |
| EBI_A20     | PB4 | PB4  | PB4  | External Bus Interface (EBI) address output pin 20.         |
| EBI_A21     | PB5 | PB5  | PB5  | External Bus Interface (EBI) address output pin 21.         |
| EBI_A22     | PB6 | PB6  | PB6  | External Bus Interface (EBI) address output pin 22.         |
| EBI_A23     | PC0 | PC0  | PC0  | External Bus Interface (EBI) address output pin 23.         |
| EBI_A24     | PC1 | PC1  | PC1  | External Bus Interface (EBI) address output pin 24.         |
| EBI_A25     | PC2 | PC2  | PC2  | External Bus Interface (EBI) address output pin 25.         |
| EBI_A26     | PC4 | PC4  | PC4  | External Bus Interface (EBI) address output pin 26.         |
| EBI_A27     | PD2 | PD2  | PD2  | External Bus Interface (EBI) address output pin 27.         |
| EBI_AD00    | PE8 | PE8  | PE8  | External Bus Interface (EBI) address and data i/o pin 00.   |
| EBI_AD01    | PE9 | PE9  | PE9  | External Bus Interface (EBI) address and data i/o pin 01.   |
| EBI_AD02    | PE10| PE10 | PE10 | External Bus Interface (EBI) address and data i/o pin 02.   |
| EBI_AD03    | PE11| PE11 | PE11 | External Bus Interface (EBI) address and data i/o pin 03.   |
| EBI_AD04    | PE12| PE12 | PE12 | External Bus Interface (EBI) address and data i/o pin 04.   |
| EBI_AD05    | PE13| PE13 | PE13 | External Bus Interface (EBI) address and data i/o pin 05.   |
| EBI_AD06    | PE14| PE14 | PE14 | External Bus Interface (EBI) address and data i/o pin 06.   |
| EBI_AD07    | PE15| PE15 | PE15 | External Bus Interface (EBI) address and data i/o pin 07.   |
| EBI_AD08    | PA15| PA15 | PA15 | External Bus Interface (EBI) address and data i/o pin 08.   |
| EBI_AD09    | PA0 | PA0  | PA0  | External Bus Interface (EBI) address and data i/o pin 09.   |
| EBI_AD10    | PA1 | PA1  | PA1  | External Bus Interface (EBI) address and data i/o pin 10.   |
| EBI_AD11    | PA2 | PA2  | PA2  | External Bus Interface (EBI) address and data i/o pin 11.   |
| EBI_AD12    | PA3 | PA3  | PA3  | External Bus Interface (EBI) address and data i/o pin 12.   |
| EBI_AD13    | PA4 | PA4  | PA4  | External Bus Interface (EBI) address and data i/o pin 13.   |
| EBI_AD14    | PA5 | PA5  | PA5  | External Bus Interface (EBI) address and data i/o pin 14.   |
| EBI_AD15    | PA6 | PA6  | PA6  | External Bus Interface (EBI) address and data i/o pin 15.   |
| EBI_ALE     | PC11| PC11 |      | External Bus Interface (EBI) Address Latch Enable output.   |
| EBI_ARDY    | PF2 | PF2  | PF2  | External Bus Interface (EBI) Hardware Ready Control input.  |
| EBI_BL0     | PF6 | PF6  | PF6  | External Bus Interface (EBI) Byte Lane/Enable pin 0.        |
| EBI_BL1     | PF7 | PF7  | PF7  | External Bus Interface (EBI) Byte Lane/Enable pin 1.        |
| EBI_CS0     | PD9 | PD9  | PD9  | External Bus Interface (EBI) Chip Select output 0.          |
| EBI_CS1     | PD10| PD10 | PD10 | External Bus Interface (EBI) Chip Select output 1.          |
| EBI_CS2     | PD11| PD11 | PD11 | External Bus Interface (EBI) Chip Select output 2.          |
| EBI_CS3     | PD12| PD12 | PD12 | External Bus Interface (EBI) Chip Select output 3.          |
| EBI_CSTFT   | PA7 | PA7  | PA7  | External Bus Interface (EBI) Chip Select output TFT.        |
| EBI_DCLK    | PA8 | PA8  | PA8  | External Bus Interface (EBI) TFT Dot Clock pin.             |
| EBI_DTEN    | PA9 | PA9  | PA9  | External Bus Interface (EBI) TFT Data Enable pin.           |
| EBI_HSNC    | PA11| PA11 | PA11 | External Bus Interface (EBI) TFT Horiz. Synchroniz. pin.    |
| EBI_NANDREn | PC3 | PC3  | PC3  | External Bus Interface (EBI) NAND Read Enable output.       |
| EBI_NANDWEn | PC5 | PC5  | PC5  | External Bus Interface (EBI) NAND Write Enable output.      |
| EBI_REn     | PF5 | PF9  | PF5  | External Bus Interface (EBI) Read Enable output.            |
| EBI_VSNC    | PA10| PA10 | PA10 | External Bus Interface (EBI) TFT Vert. Synchroniz. pin.     |
| EBI_WEn     |     | PF8  |      | External Bus Interface (EBI) Write Enable output.           |


References
----------

1. [FatFS - Generic FAT Filesystem Module](http://elm-chan.org/fsw/ff/)
2. 
