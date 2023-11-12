13  Serial Communication using Direct Memory Access
===================================================


##Introduction

This documents describes a DMA based implementation of serial communication. 

The device used for serial communication is the UART0 (not USART0), because it is connected to the board controller and the board controller creates a VCP (Virtual Communicatio Port) and redirects all data received from the EFM32 MCU into it and all data received from the VCP port to the EFM32.


     +----------+             +----------+
     |          |             |          |
     | EFM32  TX|------->-----|RX BC     |
     |        RX|-------<-----|TX    VCP |<----> USB
     |          |             |          |
     +----------+             +----------+


It uses the PE0 and PE1 pins for RX and TX, respectively. There is a switch between the Board Controller and the EFM32 MCU and it must be enable in order the data can flow between them. This is controlled by the PF7 pin.

##Direct Memory Access (DMA)

DMA is a hardware based mechanism that can transfer data between memory regions without use of the processor. So reducing the load on the processor.

It can automatically transfer a prespecified amount of data 

* From a memory region to another memory region
* From a memory region to a register (write into device)
* From a register to a memory region (read from device)

Transfer can happen unconditionally or depend on a condition.
It can configured to be 8-bit, 16-bit or 32-bit at a time.

##DMA on the EFM32GG

The DMA periphal on the EFM32GG is an implementation of the ARM PL230 uDMA controller.

The EMF32GG990F1024 has 12 Direct Memory Access (DMA) channels. Some channels have additional capacities. Channels 0 and 1 support looped transfers. Channel 0 support 2D copy.

The following peripherals can be the source/destination of a DMA transfer: USART, UART. LEUART, TIMER, DAC, ADC, DAC, EBI.

A descriptor contains the information about the DMA transfer and it contains:

* Source address
* Destination address
* Number of bytes to be transferred
* Condition
* Transfer width
* Increment of source address
* Increment of destination address
* Priority



Each DMA channel has one primary and one alternate descriptor. The descriptors must be stored in a single area, accessible to both peripherals.

	+01B0   Alternate descriptor for channel 11
	+01A0   Alternate descriptor for channel 10
	+0190	Alternate descriptor for channel 9
	+0180	Alternate descriptor for channel 8
	+0170	Alternate descriptor for channel 7	
	+0160   Alternate descriptor for channel 6
	+0150	Alternate descriptor for channel 5
	+0140	Alternate descriptor for channel 4
	+0130	Alternate descriptor for channel 3	
	+0120   Alternate descriptor for channel 2
	+0110	Alternate descriptor for channel 1
	+0100	Alternate descriptor for channel 0
	+00F0   Not used
	+00E0   Not used
	+00D0   Not used
	+00C0   Not used
	+00B0	Descriptor for channel 11
	+00A0	Descriptor for channel 10
	+0090	Descriptor for channel 9
	+0080	Descriptor for channel 8
	+0070	Descriptor for channel 7
	+0060	Descriptor for channel 6
	+0050	Descriptor for channel 5
	+0040	Descriptor for channel 4
	+0030	Descriptor for channel 3
	+0020	Descriptor for channel 2
	+0010	Descriptor for channel 1
	+0000	Descriptor for channel 0

This area can be 384 bytes large (12+12)*16 bytes but it can shortened according the largest number of the used channels. The address of this area is specified by the contents of the CTRLBASE register. The ALTCTRKBASE register contains the address of the alternate descriptor and is automaticall set.

> The address of this area must be a multiple of 256, because the 8 lower order bits are used to address a channel descriptor.

A channel descriptor has the following layout

	+000C	Unused
	+0008   Control
	+0004   Destination Pointer
	+0000   Source Pointer

> These pointers point the the end of the area. The address to be accessed is calculated by subtractin 

The control word has the following layout.

| Bits  |     Description                 |
| 31-30 |   Destination pointer increment |
| 29-28 |   Destination data size         |
| 27-26 |   Source pointer increment      |
| 25-24 |   Source data size              |
| 21    |   Priority for writes           |
| 18    |   Priority for reads            |
| 17-14 |   Arbitration                   |
| 13-4  |   Number of transfers (NM1)     |
| 3     |   Use burst                     |
| 2-0   |   Mode                          |

The Arbitration field specified when the DMA peripheral checks if there is a demand for a memory access. It is the power of 2 of the number of accesses done without arbitrations

> Destination and source data size must be identical

| Value  | Description                                  |
|--------|----------------------------------------------|
|  0     | +1 increment                                 |
|  1     |,+2 increment (not valid for byte transfers)  |
|  2     | +4 increment (only valid for word transfers) |
|  3     | no increment                                 |

The maximum number of bytes to be transferred is then

| Transfer size   |   Maximum number of bytes         |
|-----------------|-----------------------------------|
|    Byte         |        1024                       |
|  Halfword       |        2048                       |
|    Word         |        4096                       |

The NM1 field is update at each DMA transfer.

There is a ping-pong mode, alternating between the two descriptors and modifying one when the other is in use.

The address calculation is done by subtracting a multiple of NM1 from the end address. The multiplication factor is the transfer size.

The channels 0 and 1 can do looped transfers. Loop transfer is an alternative to the ping-pong mode. There are two registers: LOOP0 and LOOP1. The NM1 value is reloaded by the completion of transfer.
So there is a continuous transfer happening.

The channel 0 can transfer a region (rectangle) inside the memory area. This is very handy for LCD controllers. See 8.4.5.


##Transmiting a sequence of bytes


##Receiving a sequence of bytes

##Notes

1.  Before developing application that uses the serial-USB bridge, it is necessary to update the firmware in the board controller.
2.  The interface appears as a */dev/ttyACMx* in Linux machines and *COMx* in Window machines.

##More information

1.  [EFM32 STK Virtual COM port](https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/2015/07/06/efm32_stk_virtualco-aT2m)[17]
2.  [Using stdio on Silicon Labs platforms](https://os.mbed.com/teams/SiliconLabs/wiki/Using-stdio-on-Silicon-Labs-platforms)[18]
3.  [AN0045](http://www.silabs.com/Support%20Documents/TechnicalDocs/AN0045.pdf)[19]
