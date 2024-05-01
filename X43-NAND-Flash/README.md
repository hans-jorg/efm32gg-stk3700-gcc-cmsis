43  NAND FLash
==============

Introduction
------------

There are basically two types of flash memory devices:

* NAND Flash devices
* NOR Flash devices

The main difference is the interface. NAND Flash device are suited for parallel access, demanding
 more pins and delivering higher speed. NOR devices, generally, have a serial interface,
 typically SD, that is a development of SPI.

Another very important difference is the successibilty of error in NAND Flash devices. Most of them
come with some regions marked as deffective. In some cases, a test must be done to mark these
regions at initialization. But it can be worse. Some deffects can appears during the life time, even
when at a number of write operations below the specified value. This errors must be handled and
the system must copy its contents to a new region and mark the region with errors as deffective.

Summarizing.


| Characteristic      |  NAND flash                          | NOR flash                           |
|---------------------|--------------------------------------|-------------------------------------|
| Storage density     | Higher                               | Lower                               |
| Read performance    | Fast                                 | Fast*                               |
| Write performance   | Faster                               | Slower*                             |
| Erase performance   | Faster (low ms typically)            | Slower (possibly seconds)           |
| Storage reliability | Lower (without management)           | Better                              |
| Life span           | Higher                               | Lower                               |
| Price               | Lower                                | Higher                              |

* May be slowed by serial access


> One feature of Flash devices is that it is possible to change a bit 1 to a 0, but to change from
0 to 1, an erase operation is needed.

On NAND Flash, the erase operation must be done on a large chunk of bits, generally called block.

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
have wear leveling and bad block management. FAT file systems were not conceived for storing data
in devices like NAND flash devices. Another aspect is that the erase operations are done in a
group of pages. To use FatFS, all these aspects must be considered in the low level firmware driver.

LittleFS is a fail safe filesystem for microcontroller. It has dynamic wear leveling, power loss
resilience, and has low demand on RAM and ROM.

SPIFFS is used on ESP32 devices. But is slow and has some significant problems. In some cases, it
is beeing replaced by LittFS.

YaFFs has two versions. The version 2 is faster, support devices with large pages. Both support
wear leveling and bad block management.

NAND Flash device
-----------------

### Introduction

The ST NAND256W3A features the following:

* 32 MBytes (=256 Mbit)
* 538/264 Word Page = (=512+16/256+8). The non power of two sizes area due to the spare area(16/8).
* Multiplexed Data/Address lines with up to 16 bit width.
* Support to over than 100.000 erases cycles.
* 8-bit wide data path. Other devices can have 16-bit wide data path.
* It has at least 2008 valid blocks from 2048.


The table below shows some important parameters of the NAND256

| Parameter                       |   Value     |  Unit |
|---------------------------------|-------------|-------|
| Page program time               |   200-500   |   us  |
| Block erase time                |       2-3   |   ms  |
| Program/Erase cycles            |   100.000   | cycles|
| Data retention                  |        10   | years |


### Organization

The memory array is organized in blocks, and each block has 32 pages. Each page is 528x8 large
and is divided in three parts:
* 1st half page (256 bytes)
* 2nd half page (256 bytes)
* Spare area (16 bytes)

The spare area can be used to store Error Correction Code (ECC) data, software flags,
Bad Block identification or just to increase the storage area.

> Read operations can be done on pages, but erase operations can only be done on blocks.


### Electronic signature

Manufacturer code:  0x20
Device code:        0x75


### Errors


The NAND Flash can have bad blocks already identified during manufacturing or can develop them
during its lifetime.

A bad block(page?) does not contain an 0xFF (all ones) value in the 6th byte in the spare area.

> ATTENTION: This value can be overwritten and get lost.


The 256 Mbits device should have at least 2008 valid blocks from the 2048 total.

The bad blocks can be managed using Bad Blocks Management, Block Replacement or Error Correction
Code.


### Summary

|  Feature                |   Size     |  Unit   |
|-------------------------|------------|---------|
|  1st Half Page          |   256      |  Bytes  |
|  2st Half Page          |   256      |  Bytes  |
|  Spare area             |    16      |  Bytes  |
| Without spare area      |            |         |
|  Page                   |   512      |  Bytes  |
|  Block                  |    32      |  Pages  |
|  Block                  | 16384      |  Bytes  |
|  Number of blocks       |  2048      |  Blocks |
|  Maximum Capacity       | 33.554.432 |  Bytes  |
|  Maximum Capacity       |268.435.456 |  bits   |
|  Maximum Capacity       |   256      |  Kbits  |
| Spare area as starage   |            |         |
|  Page                   |   528      |  Bytes  |
|  Block                  |    32      |  Pages  |
|  Block                  | 16896      |  Bytes  |
|  Number of blocks       |  2048      |  Blocks |
|  Maximum Capacity       | 34.603.008 |  Bytes  |
|  Maximum Capacity       |276.824.064 |  bits   |
|  Maximum Capacity       |   270,336  |  Kbits  |


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
| VDD     | Supply Voltage                                            |
| VSS     | Ground                                                    |
| NC      | Not Connected Internally                                  |
| DU      | Do Not Use                                                |


### Operation


| Bus Operation | E# | AL | CL | R# | W# | WP# |   IO0-7   |
|---------------|----|----|----|----|----|-----|-----------|
| Command input |  0 |  0 |  1 |  1 |Rise|  X  | Command   |
| Address input |  0 |  1 |  0 |  1 |Rise|  X  | Address   |
| Data input    |  0 |  0 |  0 |  1 |Rise|  X  |Data input |
| Data output   |  0 |  0 |  0 |Fall|  1 |  X  |Data output|
| Write protect |  X |  X |  X |  X |  X |  0  |    X      |
| Standby       |  1 |  X |  X |  X |  X |  X  |    X      |



### Commands

!!! The operation of a NAND Flash envolves getting the contents of a page into a page buffer (528x8),
!!! exactly the same width of a page, update it and eventually write it back.!!!!!!

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
|   A8           | Which half page                 |
|   A13-A9       | Address in block                |
|   A26-A14      | Block address                   |
|   A26-A9       | Page address                    |


The bit A8 of the address is used to specify which Area (A or B) to access. When 0, access is done
starting at Area A. When 1, Area B. When reading the spare area, only address bit A3-A0 are used.
Address bits A7-A4 are ignored.

> The device defaults to Area A after power up or a reset.

> The Read B command in only effective for one operation


### Device operation


These are the operations for a x8 device.

#### Pointer operation

1. Send READ_A command (0x00) to set pointer to Area A or
   Send READ_B command (0x01) to set pointer to Area B or
   Send READ_C command (0x50) to set pointer ot Area C (Spare area)

   OBS: READ_B in only effective for one operation



### Read operation

There are the following types of read operation available.

* Random read. The first read after a command. It tranfers data from page to page buffer.
* Page Read. The following read operations get data from page buffer. Pulsing RE make the next read
get the byte in next column.
* Sequential Row Read. After data in last column is output, by pulsing RE, the next page is
  automatically loaded into the page buffer. This is limited to the current block!!!
  To terminate the operation, set CE high for a while.

> The Ready/Busy signal indicates that the transfer to the page buffer is completed.

The low-order bits of the address specifies the start of the read. When using x8 devices

* When the pointer is set to Area A, A7-0 specifies the start in the Area A when it is less than
 128 or Area B (when higher).
* When the pointer is set to Area B, only Area B is read.
* When the pointer is set to Area C, only A3-0 is used.

The sequence for reading the Area A (1st half page) is

1. Send command READ_A (0x00)
2. Send address
3. Wait until READY
3. Read data

The sequence for reading the Area B (2nd half page) is

1. Send command READ_B(0x01)
2. Send address
3. Wait until READY
3. Read data

The sequence for reading the Area C (spare area) is

1. Send command READ_C (0x50)
2. Send address
3. Wait until READY
3. Read data



### Write Operation

The Page Program (command 0x80) is the standard way to program data into the memory array.

> There is a limit of three consecutive partial program in the same page. After it, a Block Erase
> must be issued.

The sequence for programming the area A (1st half page) is

1. Send command READ_A (0x00)
2. Send command PAGE_PROGRAM (0x80)
3. Send address
4. Send data
5. Send command PAGE_PROGRAM_2 (0x10)
6. Send command READ_A (0x00). Optional.
7. Send command PAGE_PROGRAM_1 (0x80)
8. Send address
9. Send data
10.Send command PAGE_PROGRAM_2 (0x10)

The sequence for programming the area B (2nd half page) is

1. Send command READ_B (0x01)
2. Send command PAGE_PROGRAM_1 (0x80)
3. Send address
4. Send data
5. Send command PAGE_PROGRAM_2 (0x10)
6. Send command READ_B (0x01). Obrigatory.
7. Send command PAGE_PROGRAM_1 (0x80)
8. Send address
9. Send data
10.Send command PAGE_PROGRAM_2 (0x10)

The sequence for programming the area C (Spare area) is

1. Send command READ_C (0x50)
2. Send command PAGE_PROGRAM (0x80)
3. Send address
4. Send data
5. Send command PAGE_PROGRAM_2 (0x10)
6. Send command READ_A (0x00). Optional.
7. Send command PAGE_PROGRAM_1 (0x80)
8. Send address
9. Send data
10.Send command PAGE_PROGRAM_2 (0x10)


### Read Status

### Read Electronic Signature

1. Send command 0x90
2. Read two bytes

### Copy Back Program

This operation transfers data from one page to another, without external access.

1. Send command READ_A (0x00)
2. Send source address
3. Send command COPY_BACK (0x8a)
4. Send destination address
5. Send command (0x10)
6. Send command 0x70
7. Read data (SR0)


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

Error Correction Code (ECC) can be used to detect and correct errors. For every 2048 bits in the
device, it is neccessary to use 22 bits for ECC: 16  for line parity and 6 for column parity).


### Garbage Collection

When a data page needs to be modified, it is faster to write to the first available page.
The previous page would be marked then as invalid. After several updates it is necessary to remove
invalid pages to free some memory space. Using Garbage Collection, the valid pages are copied into
a free area and the block containing the invalid pages is erased.


### Wear-Leveling Algorithm

The number of write operationg in Flash devices is limited. NAND Flash memories are programmed and
 erased by Fowler-Nordheim tunneling using a high voltage. Exposing the device to a high voltage
  for extended periods can cause the oxide layer to be damaged.

To extended the life of the device, an algorithm is used to an equal use of writing operations of
the pages.

There are tow wear-level procedures:

* First Level Wear-Leveling: New data is written to the free blocks that have the fewest writing
cycles.
* Second Level Wear-Leveling: Bkocks with long lived data gives room to new data, after their
contents are written to other blocks.

### Timing

From the NAND data sheet


| Parameter |  Description                          |  Value   |  Unit    |
|-----------|---------------------------------------|----------|----------|
| tADL      | ?                                     |    0     |   ns     |
| tALS      | AL Setup time                         |    0     |   ns     |
| tCS       | E# Setup time                         |    0     |   ns     |
| tCLS      | CL Setup time                         |    0     |   ns     |
| tDS       | Data Setup time                       |   20     |   ns     |
| tALH      | AL Hold time                          |   10     |   ns     |
| tCH       | E Hold time                           |   10     |   ns     |
| tCLH      | CL Hold time                          |   10     |   ns     |
| tDH       | Data Hold time                        |   10     |   ns     |
| tWC       | Write cycle time                      |   50     |   ns     |
| tWH       | W# High Hold time                     |   15     |   ns     |
| tWP       | W# Pulse Width                        |   25     |   ns     |
| tWB       | Write Enable High to Ready/Busy Low   |  100     |   ns     |
| tCEA      | Chip Enable Low to Output Valid       |   45     |   ns     |
| tREA      | Read Enable Low to Output             |   35     |   ns     |
| tRP       | Read Enable Low to Read Enable Low    |   30     |   ns     |
| tRHZ      | Read Enable High to Output Hi-Z       |   30     |   ns     |
| tREH      | Read Enable High to Read Enable Low   |   15     |   ns     |
| tRC       | Read Enable Low to Read Enable Low    |   60     |   ns     |
| tRR       | Ready/Busy High to Read Enable Low    |   20     |   ns     |
| tAR       | Address Latch Low to Read Enable Low  |   10     |   ns     |
| tCLR      | Command Latch Low to Read Enable Low  |   10     |   ns     |
| tIR       | Data Hi-Z to Read Enable Low          |    0     |   ns     |
| tWC       | Write Enable Low to Write Enable Low  |   50     |   ns     |


### Read sequence

From RM:

AA typical 528-byte page read sequence for an 8-bit wide NAND Flash is
as follows:

1. Configuration: Enable and select the memory bank connected to the NAND
Flash device via the EN and BANKSEL bitfields in the EBI_NANDCTRL register. Set
the MODE field of the EBI_CTRL register to D8A8 indicating that the attached
device is 8-bit wide. Program the EBI_RDTIMING and EBI_WRTIMING registers
to fulfill the NAND timing requirements.

2. Command and address phase: Program the NAND Command register to the
page read command and program the NAND Address register to the required read
address. This can be done via Cortex-M3 or DMA writes to the memory mapped NAND
Command and Address registers. The automatic data access width conversions
described in Section 14.3.11 (p. 188) can be used if desired to for example
automatically perform 4 consecutive address byte transactions in response
to one 32-bit word AHB write to the NAND Address register (in this case the
2 address LSBs should not be used to map onto the NAND ALE/CLE signals).

3. Data transfer phase: Wait for the NAND Flash internal data transfer phase
to complete as indicated via its ready/busy (R/B) pin. The user can use the
GPIO interrupt functionality for this. The 528-byte data is now ready for
sequential transfer from the NAND Flash Data register.

4. Read phase: Clear the ECC_PARITY register and start Error Code Correction
(ECC) parity generation by setting both the ECCSTART and ECCCLEAR bitfields
in the EBI_CMD register to 1. Now all subsequently transferred data to/from
the NAND Flash devices is used to generate the ECC parity code into the
EBI_ECCPARITY register. Read 512 subsequent bytes of main area data from
the NAND Flash Data register via DMA transfers. This can for example be
done via 32-bit word DMA transfers (as long as the two address LSBs are not
used to map onto the NAND ALE/CLE signals). Stop ECC parity generation by
setting the ECCSTOP bitfield in the EBI_CMD register to 1 so that following
transactions will not modify the parity result. Read out the final 16 bytes
from the NAND Flash spare data area.

5. Error correction phase: Compare the ECC code contained in the read spare
area data against the computed ECC code from the EBI_ECCPARITY register. The
user software can accept, correct, or discard the read data according the
comparison result. No automatic correction is performed.

### Program (write) sequence

A typical 528-byte page program sequence for an 8-bit wide NAND Flash is
as follows:

1. Configuration: Configure the EBI for NAND Flash support via the
EBI_NANDCTRL, EBI_CTRL, EBI_RDTIMING and EBI_WRTIMING registers.

2. Command and address phase: Program the NAND Command register to command
for page programming (serial data input) and program the NAND Address register
to the desired write address.

3. Write phase: Clear the ECC_PARITY register and start Error Code Correction
(ECC) parity generation by setting both the ECCSTART and ECCCLEAR bitfields
in the EBI_CMD register to 1. Now all subsequently transferred data to/from
the NAND Flash devices is used to generate the ECC parity code into the
EBI_ECCPARITY register. Write 512 subsequent bytes of user main data to
the NAND Flash Data register via for example DMA transfers. Stop ECC parity
generation and read out the computed ECC parity data from EBI_ECCPARITY. Write
the final 16 bytes of spare data including the computed ECC parity data bytes.

3. Program phase: Write the auto program command to the NAND Flash Command
register after which the NAND Flash will indicate that it is busy via its
read/busy (R/B) pin. After read/busy goes high again, the success of the
program command can be verified by programming the read status command.


NAND Flash on the STK3700
-------------------------

The EMF32GG-STK3700 has an 32 MBytes (=256 Mbit) NAND Flash device  (ST NAND256W3A), with an
8 bit parallel interface and support for ECC (E Correction Code).

The device can be power up or down by the NAND_PWR_EN (PB15).

The EFM32GG family has a EBI (External Bus Interface) peripheral that handles the interface and
map the device into the MCU memory. It uses a multiplexing mechanism to reduce the pin count.

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


Some pins are controlled directly by the GPIO module. Others by the EBI module. The EBI_ROUTE
register controls
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

A 256 Mbit/32MByte device needs 24 bits (A0 to A23) to address the full capacity of the device.
This corresponds to three bytes. The next address bits, A24 and A25, are used to drive the Address
Latch Enable (ALE) and command Latch Enable (CLE) lines. So, when addressing memory with the A24
bit set, the ALE line is set and the data transmitted is interpreted as an address.
Similarly, when addressing with the A25 bit set, the CLE line is set.

The device register are mapped into the MCU memory map.

| Register     |  Address                   |
|--------------|----------------------------|
| Data         | 0x8000_0000                |
| Address      | 0x8100_0000                |
| Command      | 0x8200_0000                |


The External Bus Interface (EBI)
--------------------------------

The external bus Interface (EBI) support devices with up to 28 address lines and up to 16-bin
data lines in multiplexed and non multiplexed mode.

It supports four banks of different memory devices, including:

* SRAM
* Flash
* TFT RGB



There are four EBI regions that can be used to access the NAND Flash device.

| EBI Region   |  Address Range             |    Size    |
|--------------|----------------------------|------------|
|    #0        | 0x8000_0000 - 0x83FF_FFFF  |   64 MB    |
|    #1        | 0x8400_0000 - 0x87FF_FFFF  |   64 MB    |
|    #2        | 0x8800_0000 - 0x8BFF_FFFF  |   64 MB    |
|    #3        | 0x8C00_0000 - 0x8FFF_FFFF  |   64 MB    |


### Support for NAND Flash devices

NAND Flash devices work using a page access and use and indirect interface. NOR Flash devices
supports random read access but are smaller and slower than NAND devices. Another important
difference is that NAND devices has more succeptible to errors, and in general, an Error Correction
Code (ECC) is used.

The EBI supports 8 and 16-bit wide Flash devices. It is easy (and glueless) to connect a flash
device to a EFM32GG.
A mixed scheme of EBI and GPIO controlled pins is used.

| NAND Signal | Name         |  EFM32GG Signal       |
|-------------|--------------|-----------------------|
| R#          | Read         | EBI_NANDREn           |
| W#          | Write        | EBI_NANDWEn           |
| CL          | Command latch| EBI_A25               |
| AL          | Address latch| EBI_A24               |
| WP#         | Write Protect| GPIO_PORTxy (output)  |
| I/O7-0      | Bus          | EBI_AD7:0             |
| E#          | Enable       | GPIO_PORTxy (output)  |
| R/B         | Ready/Busy   | GPIO_Portxy (input)   |

> There is a class of NAND Flash devices called Chip Enable Don't Care (CEDC), that demands that
an EBI Chip Select EBI_CSn is used and controlled by the EBI module. CEDC Flash devices do not
support automatic sequential support.

> There are extra lines EBI_AD15_8 that are used in 16 bit wide devices.

The table below shows the mapping when AL is connected to  A24 and CL to A25.

| Address     |  A25   |  A24   |  Flash register                 |
|-------------|--------|--------|---------------------------------|
| 0x8000_0000 |   0    |   0    | Data register                   |
| 0x8100_0000 |   0    |   1    | Address register                |
| 0x8200_0000 |   1    |   0    | Command register                |
| 0x8300_0000 |   1    |   1    | Invalid/Undefined               |


> The A24 and 25 are interchangable. It only alters the memory mapping.


### Timing

Almost all parameters are set as multiple of the HFCORECLOCK clock period. In the worst  case,
maximum clock frequency, the clock period is 1/48 MHz = 20,83 ns.

There are four set  of four registers for timing configuration, one for each bank.

| Register        | Description                        |
|-----------------|------------------------------------|
| EBI_ADDRTIMING  | Address Timing Register            |
| EBI_RDTIMING    | Read Timing Register               |
| EBI_WRTIMING    | Write Timing Register              |
| EBI_POLARITY    | Polarity Register                  |
| EBI_RDTIMING1   | Read Timing Register 1             |
| EBI_WRTIMING1   | Write Timing Register 1            |
| EBI_POLARITY1   | Polarity Register 1                |
| EBI_ADDRTIMING2 | Address Timing Register 2          |
| EBI_RDTIMING2   | Read Timing Register 2             |
| EBI_WRTIMING2   | Write Timing Register 2            |
| EBI_POLARITY2   | Polarity Register 2                |
| EBI_ADDRTIMING3 | Address Timing Register 3          |
| EBI_RDTIMING3   | Read Timing Register 3             |
| EBI_WRTIMING3   | Write Timing Register 3            |
| EBI_POLARITY3   | Polarity Register 3                |

The fields important for NAND devices are:

| Field    | Description                                                                            |
|----------|----------------------------------------------------------------------------------------|
| ADDRHOLD | Number of cycles the address is held after ALE is asserted                             |
| ADDRSETUP| Number of cycles the address is driven onto the ADDRDAT bus before ALE is asserted     |
| RDSTRB   | Sets the number of cycles the REn is held active                                       |
| RDSETUP  | Sets the number of cycles the address setup before REn is asserted                     |
| WRSTRB   | Sets the number of cycles the WEn is held active                                       |
| WRSETUP  | Sets the number of cycles the address setup before WEn is asserted                     |

The timing parameters can now be determined. It is easy because most of parameters are minimum values
and are smaller than the clock period (20,83 ns).

| Parameter| Value    | Requirement                                  |
|----------|----------|----------------------------------------------|
| tADL     |          | <= t(WRHOLD) + t(WRSETUP) + t(WRSTRB)        |
| tALS     |          | <= t(WRSETUP) + t(WRSTRB)                    |
| tCS      |          | <= t(WRSETUP) + t(WRSTRB)                    |
| tCLS     |          | <= t(WRSETUP) + t(WRSTRB)                    |
| tDS      |          | <= t(WRSETUP) + t(WRSTRB)                    |
| tALH     |          | <= t(WRHOLD)                                 |
| tCH      |          | <= t(WRHOLD)                                 |
| tCLH     |          | <= t(WRHOLD)                                 |
| tDH      |          | <= t(WRHOLD)                                 |
| tWC      |          | <= t(WRHOLD) + t(WRSETUP) + t(WRSTRB)        |
| tWH      |          | <= t(WRHOLD) + t(WRSETUP)                    |
| tWP      |          | <= t(WRSTRB)                                 |
| tWB      |          |                                              |
| tCEA     |          | <= t(RDSETUP) + t(RDSTRB)                    |
| tREA     |          | <= t(RDSTRB)                                 |
| tRP      |          | <= t(RDSTRB)                                 |
| tRHZ     |          | <= t(RDHOLD)                                 |
| tREH     |          | <= t(RDHOLD) + t(RDSETUP)                    |
| tRC      |          | <= t(RDHOLD) + t(RDSETUP) + t(RDSTRB)        |
| tRR      |          | <= t(RDSETUP)                                |
| tAR      |          | <= t(RDSETUP)                                |
| tCLR     |          | <= t(RDSETUP)                                |
| tIR      |          | <= t(RDSETUP)                                |


### Error Correction Code



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





Annex A - EBI Pin Usage
=======================

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

1. [NAND Flash Memories: Bad Block Management and the YAFFS File System](https://www.eeweb.com/nand-flash-memories-bad-block-management-and-the-yaffs-file-system/)
2. [A Robust Flash File System Since 2002](https://yaffs.net/)
2. [FatFS - Generic FAT Filesystem Module](http://elm-chan.org/fsw/ff/)
3. [A Robust Flash File System Since 2002](https://yaffs.net/)
4. [LittleFS](https://github.com/littlefs-project/littlefs)
5. [SPIFFS (SPI Flash File System)](https://github.com/pellepl/spiffs)
