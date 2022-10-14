32 USB Device
=============

## Introduction

This projects shows the implementation of a CDC (Communications Device Class)
device on the EFM32GG990-STK3700 Board.

Only a Full Speed version is, for now, implemented. There are two reasons for it.

1. In order to be a low speed device, a 4.7 K resistor must be connected to the
   DMUP pin, but this resistor is not available in the STM3700 Board.
2. Low speed devices do not use standard USB cables. Mouse and keyboards have
   attached cables conforming a specific standard.

> NOTE: To develop USB firmware for the EFM32 is a dauting task. Information is
> spread in many manuals, application notes, standards. Sometimes only looking
> at the example code for the application note can give the correct information.

> NOTE: efm32lib mentioned in the source code was renamed emlib, and is part of
> the GeckoSDK. It is now only distributed with the Simplicity IDE. But it
> is still possible to download Gecko_SDK from [11].

## The USB Protocol

The USB uses a host/device protocol, where the host delivers power to the
device and is in charge to initiate the communication. The host uses a
A-Connector and the device a B connector. With the introduction of the
On The Go Device, that can be a host or a device, according some circunstances,
a AB-Connector is used.

The USB protocol has many speeds:

| Speed               |     |Version| Throughput |  EFM32
|---------------------|-----|-------|------------|---------------|
| Low Speed           | LS  |  1.0  |  1.5 Mbps  | Supported     |
| Full Speed          | FS  |  1.0  |   12 Mpbs  | Supported     |
| High Speed          | HS  |  2.0  |  480 Mpbs  | Supported     |
| Super Speed         | SS  |  3.0  |    5 Gbps  | Not supported |
| Super Speed+        | SS+ |  3.1  |   10 Gbps  | Not supported |
| Super Speed 10 Gpbs | SS+ |  3.2  |   10 Gpbs  | Not supported |
| Super Speed 20 Gbps | SS+ |  3.2  |   20 Gbps  | Not supported |
| Super Speed 40 Gbps | SS+ |  4.0  |   40 Gbps  | Not supported |


The host delivers a 5 V with at least 100 mA. This can be upgraded to 1.5 A
after negotiation.


## This Implementation

This implementation has the following features:

* Only work as Device
* Only Full Speed
* Only Control and Bulk transfers (No interrupt and no isochronous transfers)
* Maximum Packet Size set to 64
* Use of DMA mode (Faster and simpler, but more difficult to debug)

> One motive is that "It should also be noted that according to USB specification,
> low-speed mode is defined to support a limited number of low-bandwidth
> devices, such as mice. Low-speed devices are not allowed to use standard USB
> cables, and a separate specification for low-speed cables exist." (An0046)
>
> Another motive is the absence of a 4.7 K resistor driven by  DMPU pin,
> to attend the USB impedance standards for devices.

## Files

| File                    |  Contents                                   |
|-------------------------|---------------------------------------------|
| usb_config.h            | General configuration like VendorID         |
| usb_descriptors.h       | Definition of all descriptors               |
| usb_descriptors.c       | Routines to build some descriptors          |
| usb_device.c            | Main routine for a USB device               |
| usb_device.h            | Interface for usb_device.c                  |
| usb_endpoints.h         | Definition of endpoints data structures     |
| usb_packets.h           | Definition of USB packets                   |
| usbcdc_config.c         | CDC config                                  |
| usbcdc_descriptors.h    | CDC descriptors                             |



## USB Connectors

There are different versions of connectors for host and devices.

For hosts, a type A connector is used. It is a squared connector, relatively
large. The black version is for speed up to High Speed. The blue ones, for
higher speeds.

The devices use type B connectors in a variety of forms.

| Connector   |  Description                           |
|-------------|----------------------------------------|
|  B          | Large. Still used in printers          |
|  Mini-B     | Small. Obsolete                        |
|  Micro-B    | Smaller. Generally used in cell phones |
|  Micro-AB   | For OTG Devices                        |

> The Mini-B connector is used in the Debug USB port of the STK3700 Board.

> The Micro-AB connector is used in the USB port of the STK3700 Board.

> The Micro-B connector is beeing replaced by the Type-C in cell phones.

All these connectors, are been slowly replaced by Type-C. It is faster, more
robust, support large current and it is reversible.

# Endpoints

All communications occurs from an enpoint and to an endpoint.
There is a special endpoint (#0) that is used in the configuration process.

In addition to endpoint #0, a full- or high-speed device can have up to 30
additional endpoint addresses (1–15, IN and OUT).

A low-speed device can have at most two additional endpoint addresses which can
be two IN, two OUT, or one in each direction.

In every USB low, full and high speed transaction, the host sends an addressing
triple that consists of a device address, an endpoint number, and endpoint
direction.

A data transfer uses pipes, that are an association between a device endpoint and
the controller. A pipe is created during the enumeration process.


# Enumeration process

During enumeration,

1.  A device is connected. Device in Powered state.
2.  The hub detects the speed.
3.  The device is reset. Both DM and DP low. Device in Defaul state.
4.  Using address 0, host send a Get Descriptor request.
5.  Host assigns an address using a Set Address request. Device in Address state.
6.  Hosts send a Get Descriptor to the this address. First a Device Descriptor.
7.  Then one or more Configuration Descriptors as specified in Device Descriptor.
8.  The hosts requests more descriptors. If the device does not support one, it
    must returns a STALL packet.
9.  The host requests the string descriptor 0.
10. The host requests the serial number string.
11. The host requests the product string.
12. The host request the BOS (Binary Object Store) descriptor.
13. The host can request class specific descriptors.

This is not a specific order!

> NOTE: The Configuration Descriptor must include ALL subordinates descriptors.
> If it larger than the supported size, the partial descriptor must be sent
> limited to the supported size. The hosts requests a Get Descriptor again,
> now with the specified size (it was sent in the first try).


A device moves thru different states:

* Powered
* Default
* Address:
* Configured: When the host send a Set Configuration request. The device must
  run according the configuration selected.
* Attached: When the hub is not powering the VBUS line, due to over-current or
  host request.
* Suspend: A device enter the Suspend state when there is no bus activity,
  including SOF markers for, at least, 3 ms. The power comsumption must be
  restricted.


# Transfer types

A transfer is always initiated by the host. In version 2 of USB the following
transfer types are defined.


| Transfer type | Description                            | Examples           |
|---------------|----------------------------------------|--------------------|
| CONTROL       | Configuration data                     | Configuration      |
| BULK          | Large amount of data                   | Pen drives, modems |
| INTERRUPT     | Small amount of data in a scheduled basis <br/> using polling       | Mouse , keyboard |
| ISOCHRONOUS   | Large amount of data at a defined rate <br/> but  without guarantee | Video/Audio      |

A transfer is divided into transactions and each transaction consists of one
or more packets. Each transaction begins with a Token packet. There are four
types of token packets:

* SOF (Start of Frame): Start of a 1 ms frame
* IN (Input): The transactions transfers data from device to host.
* OUT (Output): The transaction transfers data from host to device.
* SETUP: It starts a control transfer.

There are handshake packets:

* ACK: Acknowledge an error-free receipt of the data packet.
* NAK: Reports an error
* STALL: indicates that the transfer is not completed.


The maximum packet sizes (MPS) are specified by the USB standards.

|  Speed            |  Control   |  Bulk       |  Interrupt   | Isochronous    |
|-------------------|------------|-------------|--------------|----------------|
| Low Speed         |     8      |    -        |   1..8       |      -         |
| Full Speed        | 8,16,32,64 | 8,16,32,64  |   1..64      |   1..1023      |
| High Speed        |    64      |    512      |   1..512     |   1..1024      |
| Super Speed       |   512      |   1024      |   1..1024    |   1..1024      |


All packets, except the last one, must be equal to the maximum packet size
dealed in the configuration phase. When the transfer size is a multiple of the
MPS, a zero length packet (ZLP) should be sent to mark the end of transfer.

## Packet types

Each packet begins with a Packet ID (PID). The PID is a 4-bit field, that
identifies the packet and is followed by its complement. So, the PID demands
8-bits.

| PID Name | Pid Encoding | Packet type | Transfer types     | Speed      |
|----------|--------------|-------------|--------------------|------------|
| SETUP    |    1101      |   Token     | All                | All        |
| IN       |    1001      |   Token     | All                | All        |
| OUT      |    0001      |   Token     | All                | All        |
| SOF      |    0101      |   Token     | All                | All        |
| DATA0    |    0011      |   Data      | All                | All        |
| DATA1    |    1011      |   Data      | All                | All        |
| DATA2    |    0111      |   Data      | Isochronous        | High       |
| MDATA    |    1111      |   Data      | Isochronous        | High       |
| ACK      |    0010      |  Handshake  | CONTROL, BULK, INT | All        |
| NAK      |    1010      |  Handshake  | CONTROL, BULK, INT | All        |
| STALL    |    1110      |  Handshake  | CONTROL, BULK, INT | All        |
| NYETL    |    0110      |  Handshake  | CONTROL, BULK, INT | High       |
| PRE      |    1100      |  Special    | CONTROL, INT       | Full       |
| ERR      |    1100      |  Special    | All                | High       |
| SPLIT    |    1000      |  Special    | All                | High       |
| PING     |    0100      |  Special    | CONTROL, BULK      | High       |
| EXT      |    0000      |  Special    |                    | All        |


## Transfer types, transactions and phases


| Transfer type | Transactions           |      | Phases                       |
|---------------|------------------------|------|------------------------------|
| Control       | Setup stage (SETUP)    |  1   | Token, Data, Handshake       |
|   ^           | Data stage (IN or OUT) |  0+  | Token, Data, Handshake       |
|   ^           | Status stage           |  1   | Token, Data, Handshake       |
| Bulk          | IN or OUT              |  1+  | Token, Data, Handshake       |
| Interrupt     | IN or OUT              |  1+  | Token, Data, Handshake       |
| Isochronous   | IN or OUT              |  1+  | Token, Data                  |




## On The Go Dual Role Device

OTG devices can work as a host or as a device. An example is a camera. When
 connected to a PC, it will work as a device, but when connected to a printer,
 it can work as a host.

OTG needs to solve a problem at the start of a connection. Which device
powers the connection. This is done by the cable (one must be a micro-A
and the other a micro-B). The micro-A device must have the ID pin grounded.

After that, there are some protocols to negotiate, who is going to be the host
and who is going to be the device.

There are some OTG specific protocols like Host Negotiation Protocol (HNP) and
Session Request Protocol (SRP).

## USB Interface on the EFM32GG990


> The documentation about USB (Chapter 15 of the EFM32 Reference Manual) is bad.
> There are references to location. This means a 4-byte or 32-bit value. Further
> there are mentions to DWORD. They are 4-bytes values too.


The EFM32GG990 has a built in USB controller with the following features:

* Support for Full-Speed (12 Mbps)
* Support for Low-Speed (1.5 Mbps)
* On the Go capability
* Can deliver 50 mA to a device (at 3.3 V)
* On chip PHY interface
* Dedicate DMA controller
* 12 endpoints and the 0 endpoint
* Built in voltage regulator for the PHY transceiver.

> According AN0065, the maximum packet size is 64 for control OUT endpoint #0.
> The options are 8, 16, 32 and 64 bytes.

> There is support for CONTROL, ISOCHRONOUS, BULK and INTERRUPT endpoints.



There are 8 pins associate with the USB interface.

| Signal     | Pin   | Description               | Speed  |   Mode          |
|------------|-------|---------------------------|--------|-----------------|
|   DP       | GPIO  | Data plus                 |  All   | Host/Device/OTG |
|   DM       | GPIO  | Data minus                |  All   | Host/Device/OTG |
|   DMPU     | GPIO  | External pull up control  |  LS    |                 |
|   ID       | GPIO  | OTG Device type           |  All   | OTG             |
|   VBUSEN   | GPIO  | internal VBUS onto VBUS   |        | Host/OTG-A      |
|   VBUS     | VBUS  | VBus (5V  from connector) |        |                 |
|   VREGI    | VREGI | VBUS regulator input      |        |                 |
|   VREGO    | VREGO | VBUS regulator output     |        |                 |


The VBUS, VREGI and VREGO pins are dedicated, i.e., not shared with other MCU
functions.

| Pin        |  Description                                          |
|------------|-------------------------------------------------------|
| USB_VREGI  | USB Input to internal 3.3 V regulator.                |
| USB_VREGO  | USB Decoupling for internal 3.3V USB regulator  <br/> and regulator output. |
| USB_VBUS   | USB 5.0 V VBUS input.

VBUSEN signal will turn on the VBUS power when the interface as a host or as
 a OTG A Device.

> From AN0046. "A low-speed capable device is identified by a 1.5 kohm pull-up
> resistor on the D- line. The internal pull-up resistor on EFM32
> microcontrollers is approximately 2.2 kohm, so an external 4.7 kohm resistor
> must be placed in parallel to be standard compliant. This resistor should be
> connected to the USB_DMPU pin so it can be switched on and off by the
> USB PHY."



The circuit for powering the USB interface is flexible and enables, as shown
Section 15.3.2 of the RM.

* Bus-powered devices (Figure 15.2)
* Self-powered devices without power switch (Figure 15.3)
* Self-powered devices with power switch (Figure 15.4)
* OTG Dual Role Device  without regulator (Figure 15.5)
* OTG Dual Role Device with regulator (Figure 15.6)
* Host (Figure 15.7)


> The built in voltage regulator for the USB PHU is enabled by default and can
> thus be used to power the EFM32 itself. Systems not using the USB should
> disable the regulator by setting VREGDIS in USB_CTRL. (See RM 15.3.4)

There is only one interrupt allocated to the USB interface (USB_IRQHandler).
The USB_IF register contains the interrupt flags.

The USB Low Speed and High Speed are supported by the EFM32GG990 devicces.
But the STK3700 board doest no support Low Speed devices.

The DMPU and VBUSEN are controlled directly by the PHY. It is possible to
change the their polarity by setting the DMPUAP and VBUSENAP bits, respectively,
on the USB_CTRL register.

> The STK3700 Board does not support Low Speed. There should be a 4.7 K resistor
> driven by DMPU to make the impedance attend the USB protocols.
> Connection characteristics for low speed are different from the one
> delivered by the board.
>
> It may work but would be not reliable and most hosts and hubs support full
> speed.



> NOTE: The core is designed to be interrupt-driven. Polling interrupt mechanism
> is not recommended: this may result in undefined resolutions.

> NOTE: In device mode, just after Power On Reset or a Soft Reset, the
> USB_GINTSTS.SOF bit is set to 1 for debug purposes. This status must be cleared
> and can be ignored.

## Clocking the USB interface

A 48 MHz clock signal is required forthe USB interface. During suspension, a
32 KHz clock can be used.

There is a specific HFCORECLOCK_USB clock signal (enabled by USBCLK bit in the
HFCORECLKEN0 register), that can be sourced from:

* HFCLKNODIV: 48 MHz HCLK (before it is divided by the HFCLKDIV prescaler)
* LFXO: 32768 Hz external crystal oscillator (used when in the EM2 state)
* LFRCO: 32768 Hz internal oscillator (used when in the EM2 state)

The clock source is specified by the field USBCLOCKSEL in the CMD register.
The current source is given by the bits USBCLFRCOSEL, USBCLFXOSEL and
 USBCHFCLKSEL in the CMU_STATUS register.


## Registers


The USB Registers can be grouped as in the table below.

| Offset range     | Group   | Description                               |
|------------------|---------|-------------------------------------------|
| 0x00000-0x0001F  | Core    | Core registers                            |
| 0x3C000-0x3C3FF  | Global  | Global Global Registers (G*)              |
| 0x3C400-0x3C7FF  | Host    | Host Mode Registers (H*)                  |
| 0x3C800-0x3CDFF  | Device  | Device Mode Registers (D*)                |
| 0x3CE00-0x3CE04  | Power   | Power and Clock registers                 |
| 0x3D000-0x4A7FF  | FIFO    | Device EP/Host channel FIFO               |
| 0x5C000-0x57FFF  | DMA     | DMA Direct RAM (for debug)                |

These register are in the System Area and runs under the HFCORECLK_USB clock.

| Offset| Register   | RW | Description                                  |
|-------|------------|----|----------------------------------------------|
| 0x000 | USB_CTRL   | RW | System Control Register                      |
| 0x004 | USB_STATUS | R  | System Status Register                       |
| 0x008 | USB_IF     | R  | Interrupt Flag Register                      |
| 0x00C | USB_IFS    | W1 | Interrupt Flag Set Register                  |
| 0x010 | USB_IFC    | W1 | Interrupt Flag Clear Register                |
| 0x014 | USB_IEN    | RW | Interrupt Enable Register                    |
| 0x018 | USB_ROUTE  | RW | I/O Routing Register                         |

All others are part of the Core and need HFCORECLK_USBC clock.

> NOTE: There are registers that use a *x* index to access the endpoint *x*+1.
> So USB_DIEP0CTL access register related to endpoint 0, and USB_DIEP0_CTL
> access a register related to endpoint 1. CRAZY!!!!!!

| Endpoint | Register          |                                              |
|----------|-------------------|----------------------------------------------|
|   0      | USB_DIEP0CTL      | Device IN Endpoint 0 control register        |
|   ^      | USB_DIEP0INT      | Device IN Endpoint 0 interrupt register      |
|   ^      | USB_DIEP0TSIZ     | Device IN Endpoint 0 transfer size register  |
|   ^      | USB_DIEP0TXFSTS   | Device IN Endpoint 0 status register         |
|   ^      | USB_DIEP0DMAADDR  | Device IN Endpoint 0 DMA address register    |
|   1      | USB_DIEP0_CTL     | Device IN Endpoint 1 control register        |
|   ^      | USB_DIEP0_INT     | Device IN Endpoint 1 interrupt register      |
|   ^      | USB_DIEP0_TSIZ    | Device IN Endpoint 1 transfer size register  |
|   ^      | USB_DIEP0_TXFSTS  | Device IN Endpoint 1 status register         |
|   ^      | USB_DIEP0_DMAADDR | Device IN Endpoint 1 DMA address register    |
|   2      | USB_DIEP1_CTL     | Device IN Endpoint 2 control register        |
|   ^      | USB_DIEP1_INT     | Device IN Endpoint 2 interrupt register      |
|   ^      | USB_DIEP1_TSIZ    | Device IN Endpoint 2 transfer size register  |
|   ^      | USB_DIEP1_TXFSTS  | Device IN Endpoint 2 status register         |
|   ^      | USB_DIEP1_DMAADDR | Device IN Endpoint 2 DMA address register    |
|  ...     |  ....             | ......                                       |
|   6      | USB_DIEP5_CTL     | Device IN Endpoint 6 control register        |
|   ^      | USB_DIEP5_INT     | Device IN Endpoint 6 interrupt register      |
|   ^      | USB_DIEP5_TSIZ    | Device IN Endpoint 6 transfer size register  |
|   ^      | USB_DIEP5_TXFSTS  | Device IN Endpoint 6 status register         |
|   ^      | USB_DIEP5_DMAADDR | Device IN Endpoint 6 DMA address register    |
|  ---     |  ----             | ------                                       |
|   0      | USB_DOEP0CTL      | Device OUT Endpoint 0 control register       |
|   ^      | USB_DOEP0INT      | Device OUT Endpoint 0 interrupt register     |
|   ^      | USB_DOEP0TSIZ     | Device OUT Endpoint 0 transfer size register |
|   ^      | USB_DOEPTXFSTS    | Device OUT Endpoint 0 status register        |
|   ^      | USB_DOEP0DMAADDR  | Device OUT Endpoint 0 DMA address register   |
|   1      | USB_DOEP0_CTL     | Device OUT Endpoint 1 control register       |
|   ^      | USB_DOEP0_INT     | Device OUT Endpoint 1 interrupt register     |
|   ^      | USB_DOEP0_TSIZ    | Device OUT Endpoint 1 transfer size register |
|   ^      | USB_DOEP0_TXFSTS  | Device OUT Endpoint 1 status register        |
|   ^      | USB_DOEP0_DMAADDR | Device OUT Endpoint 1 DMA address register   |
|  ...     |  ....             | ......                                       |
|   6      | USB_DIEP5_CTL     | Device IN Endpoint 6 control register        |
|   ^      | USB_DIEP5_INT     | Device IN Endpoint 6 interrupt register      |
|   ^      | USB_DIEP5_TSIZ    | Device IN Endpoint 6 transfer size register  |
|   ^      | USB_DIEP5_TXFSTS  | Device IN Endpoint 6 status register         |
|   ^      | USB_DIEP5_DMAADDR | Device IN Endpoint 6 DMA address register    |
|  ---     |  ----             | ------                                       |
|   0      | USB_HC0_CHAR      | Host Channel 0 Characteristics Register
|   ^      | USB_HC0_INT       | Host Channel 0 Interrupt Register
|   ^      | USB_HC0_INTMSK    | Host Channel 0 Interrupt Mask Register
|   ^      | USB_HC0_TSIZ      | Host Channel 0 Transfer Size Register
|   ^      | USB_HC0_DMAADDR   | Host Channel 0 DMA Address Register
|  ...     |  ....             | ......                                       |
|   x      | USB_HC0_CHAR      | Host Channel 0 Characteristics Register
|   ^      | USB_HC0_INT       | Host Channel 0 Interrupt Register
|   ^      | USB_HC0_INTMSK    | Host Channel 0 Interrupt Mask Register
|   ^      | USB_HC0_TSIZ      | Host Channel 0 Transfer Size Register
|   ^      | USB_HC0_DMAADDR   | Host Channel 0 DMA Address Register
|  ...     |  ....             | ......                                       |
|   ^      | USB_HC13_CHAR     | Host Channel 13 Characteristics Register
|   ^      | USB_HC13_INT      | Host Channel 13 Interrupt Register
|   ^      | USB_HC13_INTMSK   | Host Channel 13 Interrupt Mask Register
|   ^      | USB_HC13_TSIZ     | Host Channel 13 Transfer Size Register
|   ^      | USB_HC13_DMAADDR  | Host Channel 13 DMA Address Register



There is data structures in the EMF32GG header files that make it easier
to handle these registers.

| Type               | Defined in              |
|--------------------|-------------------------|
| USB_DIEP_TypeDef   | emf32gg_usb_diep.h      |
| USB_DOEP_TypeDef   | emf32gg_usb_doep.h      |
| USB_HC_TypeDef     | emf32gg_usb_hc.h        |

USB_DIEP_TypeDef has the fields below:
         CTL;          /* Device IN Endpoint Control Register  */
         INT;          /* Device IN Endpoint Interrupt Register  */
         TSIZ;         /* Device IN Endpoint Transfer Size Register  */
         DMAADDR;      /* Device IN Endpoint DMA Address Register  */
         TXFSTS;       /* Device IN Endpoint Transmit FIFO Status Register  */

USB_DOEP_TypeDef has the fields below:
         CTL;          /* Device OUT Endpoint Control Register  */
         INT;          /* Device OUT Endpoint Interrupt Register  */
         TSIZ;         /* Device OUT Endpoint Transfer Size Register  */
         DMAADDR;      /* Device OUT Endpoint DMA Address Register  */

USB_HC_TypeDef has the field below:
         CHAR;         /* Host Channel Characteristics Register  */
         INT;          /* Host Channel Interrupt Register  */
         INTMSK;       /* Host Channel Interrupt Mask Register  */
         TSIZ;         /* Host Channel Transfer Size Register  */
         DMAADDR;      /* Host Channel DMA Address Register  */



There are corresponding fields in the USB_TypeDef structure: DIEP[6], DOEP[6]
and HC[14].

In the case of DIEP and DOEP, these symbols only can be used to access the
registers of endpoints 1 to 6. For Endpoint 0, there are  specific fields in the
USB_TypeDef structure.

   DIEP0CTL;          /**< Device IN Endpoint 0 Control Register  */
   DIEP0INT;          /**< Device IN Endpoint 0 Interrupt Register  */
   DIEP0TSIZ;         /**< Device IN Endpoint 0 Transfer Size Register  */
   DIEP0DMAADDR;      /**< Device IN Endpoint 0 DMA Address Register  */
   DIEP0TXFSTS;       /**< Device IN Endpoint 0 Transmit FIFO Status Register  */

   DOEP0CTL;          /**< Device OUT Endpoint 0 Control Register  */
   DOEP0INT;          /**< Device OUT Endpoint 0 Interrupt Register  */
   DOEP0TSIZ;         /**< Device OUT Endpoint 0 Transfer Size Register  */
   DOEP0DMAADDR;      /**< Device OUT Endpoint 0 DMA Address Register  */

> DIEP[] amd DOEP[] fields must be addressed by the index ENDPOINT-1


Additionally, the symbols below are defined in em_usbhal.h and enable the access
to these registers in a straightforward way.

   #define USB_DINEPS       ((USB_DIEP_TypeDef    *) &USB->DIEP0CTL )
   #define USB_DOUTEPS      ((USB_DOEP_TypeDef    *) &USB->DOEP0CTL )

> USB_DINEPS and uSB_DOUTEPS are addressed by ENDPOINT number


Care should be taken when accessing registers of endpoint 0 due to some
differences. One important one is that endpoint 0 only realizes control
transfers.

## Operation Mode

The USB interface has a builtin RAM used to store the packets to be sent or the
received ones. It is called FIFO RAM and it is managed by core. Data from it
can be transfered into or from external memory using:

* DMA Mode: The core fetches the data to be transmitted or updates the received
  data on the AHB
* Slave Mode: The application initiates the data transfers for data fetch and
  store.

This implementation only uses DMA Mode. It is easier to use DMA, because the
core handles the data movement between FIFO and buffers in external memory.
In DMA mode the core transfers data  to/from FIFO using

* USB_DIEPx_DMAADDR and USB_DOEPx_DMAADDR registers when in Device Mode
* USB_HCx_DMAADDR register in Host mode

In DMA mode it is possible to generate interrupt at

* Transfer-Level: an interrupt is generated when the full transfer is completed.
  In Device mode, the core handles all errors.
* Transaction-Level: an interrupt is generated when a transfer of one packet
  (maximum packet size or short packet size) is completed.

## Working as Device

The USB interface has as 2 KB builtin RAM used to implement FIFO for the
endpoints. Basically, the data is received into or transmitted from FIFO.

There are buffers in the MCU RAM and data is moved between FIFO and RAM using
DMA.

There are some difference on the handling of Control, Bulk, Interrupt and
Isochronous transfers. There are differences between the handling of periodic
and non periodic transfers.

> The transfers are defines as IN or OUT according the Host point of view.
> IN tranfers move data from device to host and OUT transfers from host to
> device.

### Receiving data from Host (OUT transfers)

When using non isochronous (control, bulk or interrupt) transfers, the generic
 way to receive data from host is (OUT transfer):

1 - Allocate a buffer in memory at a DWORD (32 bit) boundary.
2 - The enpoint must be configured by setting the USB_DOEPx_CTL register.
2 - Set USB_DOEPx_DMAADDR address register (DWORD address aligned)
3 - Set USB_DOEPx_TSIZ transfer size register. The XFERSIZE (Transfer Size
    Field and the PKTCNT (Packet Count) Field must be set.
4 - When a interrupt is generated, test if the Endpoint transfer size register


The configuration must set the following fields:

* EPENA: Write an 1 to enable endpoint.
* ETYPE: Type of transfer (CONTROL, BULK, INT or ISO)
* MPS: Maximum packet size (0..2047)

The packet count (PKTCNT) must be a multiple of the Maximum Packet Size (MPS)
 for the endpoint. Altough it is possible that MPS is not a multiple of 4, the
 use of multiple of 4 makes everything simple.

   $$ N = TransferSize/MPS $$

### Transfering data to Host

To transmit non periodic data to a host , an IN endpoint must be used. A bulk
or a control transfer can be used.

Since the data must be available when the host asks for it, the information
about the data must be set before. It will only be transmitted when the host
asks and it is in the FIFO.

1 - Allocate a buffer with a 32-bit alignement.
2 - Put the data onto it.
3 - Set ENDPOINT address register
4 - Set ENDPOINT packet count and transfer size.






## FIFO Configuration


Before its use, the FIFO RAM must be configured.

### Configuring for operation as a device

When USB is a device, there must be areas in FIFO for:

* Incoming (receive) packets: this is shared by all endpoints
* Outgoing (transmit) packets: there are many, one for each IN endpoint used.

as shown below.


      SA_RX=0000              +--------------------------+
                              |rx_fifo_size (SZ_RX)      |
      SA_TX0 = SIZ_RX         +--------------------------+
                              |tx_fifo_size #0 (SZ_TX0)  |
      SA_TX1 = SA_TX0+SZ_TX0  +--------------------------+
                              |tx_fifo_size #1 (SZ_TX1)  |
      SA_TX2 = SA_TX1+SZ_TX1  +--------------------------|
                              |    and so on.            |


The registers used for configurations are:

| Register.Field                 | Name            |  Value               |
|--------------------------------|-----------------|----------------------|
| USB_GRXFSIZ.RXFDEP             | RxFIFO Depth    | SZ_RX                |
| USB_GNPTXFSIZ.NPTXFINEPTXF0DEP | TxFIFO 0 Depth  | SZ_TX0               |
| USB_GNPTXFSIZ.NPTXFSTADDR      | Start Address   | SA_TX0               |
| USB_DIEPTXF1.INEPNTXFDEP       | TxFIFO 1 Depth  | SZ_TX1               |
| USB_DIEPTXF1.INEPNTXFSTADDR    | Start Address   | SA_TX1=SA_TX0+SZ_TX0 |
| USB_DIEPTXF2.INEPNTXFDEP       | TxFIFO 2 Depth  | SZ_TX2               |
| USB_DIEPTXF2.INEPNTXFSTADDR    | Start Address   | SA_TX2=SA_TX1+SZ_TX1 |
| USB_DIEPTXF3.INEPNTXFDEP       | TxFIFO 3 Depth  | SZ_TX3               |
| USB_DIEPTXF3.INEPNTXFSTADDR    | Start Address   | SA_TX3=SA_TX2+SZ_TX2 |
| USB_DIEPTXF4.INEPNTXFDEP       | TxFIFO 4 Depth  | SZ_TX4               |
| USB_DIEPTXF4.INEPNTXFSTADDR    | Start Address   | SA_TX4=SA_TX3+SZ_TX3 |
| USB_DIEPTXF5.INEPNTXFDEP       | TxFIFO 5 Depth  | SZ_TX5               |
| USB_DIEPTXF5.INEPNTXFSTADDR    | Start Address   | SA_TX5=SA_TX4+SZ_TX4 |
| USB_DIEPTXF6.INEPNTXFDEP       | TxFIFO 6 Depth  | SZ_TX6               |
| USB_DIEPTXF6.INEPNTXFSTADDR    | Start Address   | SA_TX6=SA_TX5+SZ_TX5 |

NOTE: Using CMSIS header, the DIEPTXFx registers are accessed by the DIEPTXFx
fields of USB structure. For example, to set the INEPNTXFDEP field, one can
use

   USB->DIEPTXF1 =  (USB->DIEPTXF1&~_USB_DIEPTXF1_INEPNTXFDEP_MASK)
                    |(SZ_TX1<<_USB_DIEPTXF1_INEPNTXFDEP_SHIFT);


To flush the FIFOs, set

   USB_GRSTCTL.TXFNUM = 0x10
   USB_GRSTCTL.TXFFLSH = 1
   USB_GRSTCTL.RXFFLSH = 1

and wait until USB_GRSTCTL.TXFFLSH=0 and USB_GRSTCTL.RXFFLSH=0.

The size of the Rx FIFO must include:

* 4*N+6 bytes:
* 1 byte for global OUT NAK
* (LPS/4)+1 bytes: for normal endpoints or ((LPS/4)+1)*2 bytes: for
  high bandwith endpoints (recommended)
* N bytes: status information

where N is the number of SETUP packest (n>=3) and MPS is the largest packet
size.

The size of a Tx FIFO must include, at least,

  LPS bytes:


### Configuring for operation as a device

*TBD*


## Initialization


Follow these steps to enable the USB (See 15.3.1)

1. Enable the clock to the system part by setting USB in CMU_HFCORECLKEN0.
2. If the internal USB regulator is bypassed (by applying 3.3V on USB_VREGI and
   USB_VREGO externally), disable the regulator by setting VREGDIS in USB_CTRL.
3. If the PHY is powered from VBUS using the internal regulator, the VREGO
   sense circuit should be enabled by setting VREGOSEN in USB_CTRL.
4. Enable the USB PHY pins by setting PHYPEN in USB_ROUTE.
5. If host or OTG dual-role device, set VBUSENAP in USB_CTRL to the desired
   value and then enable the USB_VBUSEN pin in USB_ROUTE. Set the MODE for the
   pin to PUSHPULL.
6. If low-speed device, set DMPUAP in USB_CTRL to the desired value and then
   enable the USB_DMPU pin in USB_ROUTE. Set the MODE for the pin to PUSHPULL.
7. Make sure HFXO is ready and selected. The core part requires the undivided
   HFCLK to be 48 MHz when USB is active (during suspend/session-off a 32 kHz
   clock is used)..
8. Enable the clock to the core part by setting USBC in CMU_HFCORECLKEN0.
9. Wait for the core to come out of reset. This is easiest done by polling a
core register with non-zero reset value until it reads a non-zero value. This
takes approximately 20 48-MHz cycles.
10. Start initializing the USB core as described in USB Core Description, as below.

For both, host and device, the following procedure must be acomplished.

1. Program the following fields in the Global AHB Configuration (USB_GAHBCFG)
   register.
   • DMA Mode bit
   • AHB Burst Length field • Global Interrupt Mask bit = 1
   • Non-periodic TxFIFO Empty Level (can be enabled only when the core is
   operating in Slave mode as a host.) • Periodic TxFIFO Empty Level (can be
   enabled only when the core is operating in Slave mode)
2. Program the following field in the Global Interrupt Mask (USB_GINTMSK)
   register:
   • USB_GINTMSK.RXFLVLMSK = 0
1. Program the following fields in USB_GUSBCFG register.
• HNP Capable bit
• SRP Capable bit
• External HS PHY or Internal FS Serial PHY Selection bit
• Time-Out Calibration field
• USB Turnaround Time field
4. The software must unmask the following bits in the USB_GINTMSK register.
• OTG Interrupt Mask
• Mode Mismatch Interrupt Mask
5. The software can read the USB_GINTSTS.CURMOD bit to determine whether the
   core is operating in Host or Device mode. The software the follows either the
   Section 15.4.1.1 (p. 250) or Device Initialization (p. 251) sequence.



To initialize the USB core to work as a device, the following sequence must be
performed by the application.

1. Program the following fields in USB_DCFG register.
• Device Speed
• Non-Zero-Length Status OUT Handshake
• Periodic Frame Interval
2. Program the USB_GINTMSK register to unmask the following interrupts.
• USB Reset
• Enumeration Done
• Early Suspend
• USB Suspend
3. Wait for the USB_GINTSTS.USBRST interrupt, which indicates a reset has been
   detected on the USB and lasts for about 10 ms. On receiving this interrupt,
   the application must perform the steps listed in Initialization on USB Reset
   (p. 285)
4. Wait for the USB_GINTSTS.ENUMDONE interrupt. This interrupt indicates the end
   of reset on the USB. On receiving this interrupt, the application must read
   the USB_DSTS register to determine the enumeration speed and perform the
   steps listed in Initialization on Enumeration Completion (p. 285)

At this point, the device can accept SOF packets.

#### When VBUS is on when the device is connected

If VBUS is on when the device is connected to the USB cable, there is no SRP
from the device. The device connection flow is as follows:
1. The device triggers the USB_GINTSTS.SESSREQINT [bit 30] interrupt bit.
2. When the device application detects the USB_GINTSTS.SESSREQINT interrupt, it
   programs the required bits in the USB_DCFG register.
3. When the Host drives Reset, the Device triggers USB_GINTSTS.USBRST [bit 12]
   on detecting the Reset. The host then follows the USB 2.0 Enumeration
   sequence.

#### When VBUS is off When the Device is Connected

If VBUS is off when the device is connected to the USB cable, the device
initiates SRP in OTG Revision 1.3 mode. The device connection flow is as
follows:
1. The application initiates SRP by writing the Session Request bit in the OTG
   Control and Status register. The core perform data-line pulsing followed by
   VBUS pulsing.
2. The host starts a new session by turning on VBUS, indicating SRP success. The
   core interrupts the application by setting the Session Request Success Status
   Change bit in the OTG Interrupt Status register.
3. The application reads the Session Request Success bit in the OTG Control and
   Status register and programs the required bits in USB_DCFG register.
4. When Host drives Reset, the Device triggers USB_GINTSTS.USBRST on detecting
   the Reset. The host then follows the USB 2.0 Enumeration sequence.

## Device Disconnection

The device session ends when the USB cable is disconnected or if the VBUS is
switched off by the Host.

1. When the USB cable is unplugged or when the VBUS is switched off by the Host,
   the Device core trigger USB_GINTSTS.OTGINT [bit 2] interrupt bit.
2. When the device application detects USB_GINTSTS.OTGINT interrupt, it checks
   that the USB_GOTGINT.SESENDDET (Session End Detected) bit is set to 1.

The application can perform a soft disconnect by setting the Soft disconnect bit
(SFTDISCON) in Device Control Register (USB_DCTL).

#### Soft disconnecte sequence

To do a Soft disconnect, Soft reset and USB Device Enumeration sequence, the
following sequence of actions should be done:

1. The application configures the device to send or receive transfers.
2. The application sets the Soft disconnect bit (SFTDISCON) in the Device
   Control Register (USB_DCTL).
3. The application sets the Soft Reset bit (CSFTRST) in the Reset Register
   (USB_GRSTCTL).
4. Poll the USB_GRSTCTL register until the core clears the soft reset bit, which
   ensures the soft reset is completed properly.
5. Initialize the core according to the instructions in Device Initialization
   (p. 251).


#### Suspend sequence


To do a Soft disconnect, Soft reset, USB Device Enumeration sequence, the
following sequence of operations must be done:

1. The core detects a USB suspend and generates a Suspend Detected interrupt.
2. The application sets the Stop PHY Clock bit in the Power and Clock Gating
   Control register, the core puts the PHY in suspend mode, and the PHY clock
   stops.
3. The application clears the Stop PHY Clock bit in the Power and Clock Gating
   Control register, and waits for the PHY clock to come back. The core takes
   the PHY back to normal mode, and the PHY clock comes back.
4. The application sets the Soft disconnect bit (SFTDISCON) in Device Control
   Register (USB_DCTL).
5. The application sets the Soft Reset bit (CSFTRST) in the Reset Register
   (USB_GRSTCTL).
6. Poll the USB_GRSTCTL register until the core clears the soft reset bit, which
   ensures the soft reset is completed properly.
7. Initialize the core according to the instructions in Device Initialization
   (p. 251) .




## EMF32GG940-STK3700 Board

The EFM32GG990 is directly connected to the USB Type AB connector. The USB
 powering used is the OTG Dual Role Device without regulator, with a software
  controlled switch.

There is an additional physical switch (SW700), that controls the VMCU source:

* Coin cell
* MCU USB 5V
* Debug USB 5V.

The software controlled switch is controlled by the EFM_USB_VBUSEN signal and
switches between the device or the board as power source for VBUS.

The signals used are show below.

| Signal            | Function  | Location | MCU Pin  | Connector Pin |
|-------------------|-----------|----------|----------|---------------|
| EFM_USB_ID        |  ID       |          |   PF12   |       4       |
| EFM_USB_DP        |  DP       |          |   PF11   |       3       |
| EFM_USB_DM        |  DM       |          |   PF10   |       2       |
| EFM_USB_OC_FAULT  |  GPIO     |          |   PF6    |       -       |
|                   |  DMPU     |          |   PD2    |               |
| EFM_USB_VBUSEN    |  VBUSEN   |          |   PF5    |               |
| EFM_USB_VBUS      |  VBUS     |          |  (B8)    |       1       |
| EFM_USB_VREGO     |  VREGO    |          |  (B11)   |               |
|                   |  VREGI    |          |  (B10)   |       1       |


The ID pin is used in OTG mode, to find if the interface will work as a A-Type
 or a B-Type (A-Type provides power).

The DMPU (Pullup control for DM) pin is not used in the STK3700 BOard. It is
needed to put a 4.7 Kohms resistor in paralled to the 2.2 Kohms internal
resistor and to correctly signalize the board as a device. So it is not possible
to run the board as a USB low speed device.

The table of compatible speeds for the STK3700 Board is shown below.

| Speed     |   Host   |  Device  |  On-The-Go  |
|-----------|----------|----------|-------------|
| Low Speed |   yes    |    no    |     ?       |
| Full Speed|   yes    |    yes   |     yes     |



## Annex A - Classes of devices

There are some predefined classes of USB devices. The most common are shown
below and are supported by most operating systems like Linux and Windows.

| Base <br/> Class | Descriptor <br/> Usage | Description     |
|------|-----------|------------------------------------------|
| 00h  | Device    | Interface Descriptors                    |
| 01h  | Interface | Audio                                    |
| 02h  | Both      | CDC (Communications Device Class) Control|
| 03h  | Interface | HID (Human Interface Device)             |
| 05h  | Interface | Physical                                 |
| 06h  | Interface | Image                                    |
| 07h  | Interface | Printer                                  |
| 08h  | Interface | Mass Storage                             |
| 09h  | Device    | Hub                                      |
| 0Ah  | Interface | CDC (Communications Device Class) Data   |
| 0Bh  | Interface | Smart Card                               |
| 0Dh  | Interface | Content Security                         |
| 0Eh  | Interface | Video                                    |
| 0Fh  | Interface | Personal Healthcare                      |
| 10h  | Interface | AVDC (Audio/Video Devices)               |
| 11h  | Device    | Billboard Device Class                   |
| 12h  | Interface | USB Type-C Bridge Class                  |
| DCh  | Both      | Diagnostic Device                        |
| E0h  | Interface | Wireless Controller                      |
| EFh  | Both      | Miscellaneous                            |
| FEh  | Interface | Application Specific                     |
| FFh  | Both      | Vendor Specific                          |


## Annex B - Initialization sequence in emlib


## References

1. [EMF32GG Reference    Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)
2. [EFM32GG990 Data Sheet](https://www.silabs.com/documents/public/data-sheets/efm32gg-datasheet.pdf)
3. [AP2151 - 0.5A Single Channel Current-limited Power Switch](https://www.diodes.com/assets/Datasheets/AP2141-51.pdf)
4. [AN0046: USB Hardware Design Guidelines](https://www.silabs.com/documents/public/application-notes/an0046-efm32-usb-hardware-design-guidelines.pdf)
5. [AN0065: EFM32 as a USB Device](https://www.silabs.com/documents/public/application-notes/AN0065.pdf)
6. [AN0801: EFM32 as USB Host](https://www.silabs.com/documents/public/application-notes/AN0801.pdf)
7. [USB](https://en.wikipedia.org/wiki/USB)
8. [USB Defined Class Codes](https://www.usb.org/defined-class-codes)
9. [AN0065-EFM32 as a USB Device (code)](https://www.silabs.com/documents/public/example-code/an0065-efm32-usb-device.zip)
10. [AN0801-EFM32 as a USB Host (code)](https://www.silabs.com/documents/public/example-code/an0801-efm32-usb-host.zip)
11. [GeckoSDK (code)](http://www.silabs.com/Support%20Documents/Software/Gecko_SDK.zip)




Initialization of USB Interface as a Device
===========================================


Device with the following features:

* Device Only (No OTG)
* Full Speed Only
* Bulk and Control Only
* Using DMA (easier and simpler)
* Using internal regulator as a self powered device. PHY powered by VBUS
  and MCU from other source (3V3)

VBUSEN is used to control a switch (AP2151) thru an active high EN signal.
VBUSEN is connected to PF5. When VBUSEN is high, the internal 5 V is connected
to VBUS. When EFM32 runs as a device, it must be set to low or left floating
(there is a pull down resistor).

Over Current Fault Signal (OC_FAULT) is connected to PF6 and can be used to
trigger an interrupt.

### Enable Clock

Following 15.3.1

1. Enable the USB System Clock (HFCORECLK_USB)
   CMU_HFCORECLKEN0.USB = 1

2. Enable internal regulator, i.e., the VREGO sense circuit and VBUSEN polarity
   USB_CTRL.VREGOSEN  = 1
   USB_CTRL.VBUSENAP  = 0

3. Enable the pins used by USB PHY
   USB_ROUTE.PHYPEN = 1

4. Select HFXO as MCU clock source (Verify if it is ready and selected).
   HFCORECLK must be 48 MHz. HFCORECLKDIV does not matter because USB uses an
   undivided HCORECLK
   # Enable HFXOEN
   CMU_OSCENCMD.HFXOEN = 1
   # Setup HFXOEN
   CMU_CTRL.HFXOBUFCUR = 3
   CMU_CTRL.HFXOMODE = 0
   # Set HFCORECLK_USBC source
   CMU_CMD.USBCCLKSEL = 1
   # Set Clock Source for HFCLK (CAUTION: Default is 0)
   CMU_CMD.HFCLKSEL = 2
   # Set VBUSEN Polarity ???


5. Enable the USB Core Clock.
   CMU_HFCORECLKEN0.USBC = 1

6. Wait for the core to come out of reset. This takes approximately 20 48-MHz
   cycles.
   while ( USB_IF==0 ) {}

### Core Initialization

Following 15.4.1: Programming the Core

1. Set Global AHB Configuration
   USB_GAHBCFG.DMAEN = 1
   USB_GAHBCFG.HBSTLEN = 0 (SINGLE) <---------- *TBV*
   USB_GAHBCFG.GLBLINTRMSK = 0

2. Mask Receive FIFO Non-Empty Interrupt Mask
   USB_GINTMSK.RXFLVLMSK = 0

3. Configure USB Core
   USB_GUSBCFG.HNPCAP = 0
   USB_GUSBCFG.SRPCAP = 0
   USB_GUSBCFG.TERMSELPULSE = 0
   USB_GUSBCFG.USBTRDTIM = 5

4. The following interrupts must be unmasked
   USB_GINTMSK.OTGINTMSK = 1
   USB_GINTMSK.MODEMISMSK = 1

5. Verify current mode in USB_GINTSTS.CURMOD. It must be zero.
   It should be in device mode.

6. (Optional) Force Device Mode
   USB_GUSBCFG.FORCEDEVMODE = 1
   Wait 25 ms

> In device mode, just after Power On Reset or a Soft Reset, the USB_GINTSTS.SOF
> bit is set to 1 for debug purposes. This status must be cleared and can be
> ignored.

### Device Initialization

Following 15.4.1.2 (p. 251)

1. Program the following fields in USB_DCFG register.
   # Enable Device Speed (Full)
   USB_DCFG.DEVSPD = 3
   # Set Non-Zero-Length Status OUT Handshake (?)
   USB_DCFG.NZSTSOUTHSHK = 1
   # Set Periodic Frame Interval (80%)
   USB_DCFG.NZSTSOUTHSHK = 0

2. Unmask the following interrupts.
   # USB Reset Mask
   USB_GINTMSK.  = 1
   # Enumeration Done Mask
   USB_GINTMSK.ENUMDONEMSK  = 1
   # Early Suspend Mask
   USB_GINTMSK.ERLYSUSPMSK = 1
   # USB Suspend Mask
   USB_GINTMSK.USBSUSPMSK = 1

3. Wait for the USB_GINTSTS.USBRST interrupt, which indicates a reset has been
   detected on the USB and lasts for about 10 ms. On receiving this interrupt,
   the application must perform the steps listed in Initialization on USB Reset
   (p. 285)
   Wait for 10 ms

4. Wait for the USB_GINTSTS.ENUMDONE interrupt. This interrupt indicates the end
   of reset on the USB. On receiving this interrupt, the application must read
   the USB_DSTS register to determine the enumeration speed and perform the
   steps listed in Initialization on Enumeration Completion (p. 285).

At this point, the device is ready to accept SOF packets and perform control
transfers on control endpoint.


### Initialization on USB Reset

Following 15.4.4.1.1 (p. 285)


1. Set the NAK bit for all OUT endpoints (x=1..6) [and 0?]
   USB_DOEPx_CTL.SNAK = 1

2. Unmask following interrupts
   USB_DAINTMSK.INEP0 = 1 (control 0 IN endpoint)
   USB_DAINTMSK.OUTEP0 = 1 (control 0 OUT endpoint)
   USB_DOEPMSK.SETUP = 1
   USB_DOEPMSK.XFERCOMPL = 1
   USB_DIEPMSK.XFERCOMPL = 1
   USB_DIEPMSK.TIMEOUTMSK = 1

3. To transmit or receive data, for DMA some interrupts must be masked.
   See 15.4.4.1.7 (p. 287).
   USB_GINTMSK.NPTXFEMPMSK = 0
   USB_GINTMSK.RXFLVMSK = 0

4. Set up the Data FIFO RAM for receiving control OUT data and setup data, by
   programmming the  USB_GRXFSIZ Register. Only Depth is needed because the
   address for the receive FIFO is 0. (See below for size info)
   USB_GRFSIZ.RXFDEP = RxFIFO Depth (Size)

5. Setup the Device IN Endpoint Transmit FIFO size register in order to be able
   to transmit control IN data. It depends on the FIFO number chosen (???).
   When using Endpoint 0, use USB_GNPTXFSIZ, Non-periodic Transmit FIFO Size
   Register
   USB_GNPTXFSIZ.NPTXFINEPTXF0DEP   =   EP0 TxFIFO Depth
   USB_GNPTXFSIZ.NPTXFSTADDR        =   EP0 TxFIFO Start Address

   The Start Address must be equal or greater the the RxFIFO Depth.

6. Program the following fields in the endpoint-specific registers for control
   OUT endpoint 0 to receive a SETUP packet
   USB_DOEP0TSIZ.SUPCNT = 3 (to receive up to 3 back-to-back SETUP packets)
   USB_DOEP0DMAADDR = area to store any SETUP packets received

At this point, all initialization required to receive SETUP packets is done,
except for enabling control OUT endpoint 0 in DMA mode.



### Initialization on Enumeration Completion

From 15.4.4.1.2

1. On the Enumeration Done interrupt (USB_GINTSTS.ENUMDONE), read the USB_DSTS
   register to determine the enumeration speed.
   Verify if USB_DSTS.ENUMSPD == 3

2. Set the maximum packet size (64 for FS, 8 for LS). This step configures
   control endpoint 0. The maximum packet size for a control endpoint depends on
   the enumeration speed.
   USB_DIEP0CTL.MPS = MPS

3. In DMA mode, program the USB_DOEP0CTL register to enable control OUT endpoint
   0, to receive a SETUP packet.
   USB_DOEP0CTL.EPENA = 1

At this point, the device is ready to receive SOF packets and is configured to
perform control transfers on control endpoint 0.


### Initialization on SetAddress Command

From 15.4.4.1.3, after receiving a SetAddress command in a SETUP packet.

1. Program the USB_DCFG register with the device address received in the
   SetAddress command.
   USB_DCFG.   = ADDRESS

2. Program the core to send out a status IN packet. <===================


### Initialization on SetConfiguration/SetInterface Command

From 15.4.4.1.4, after receiving a SetConfiguration or a SetInterface command.

1. When a SetConfiguration command is received, the application must program the
   endpoint registers to configure them with the characteristics of the valid
   endpoints in the new configuration.

2. When a SetInterface command is received, the application must program the
   endpoint registers of the endpoints affected by this command.

3. Some endpoints that were active in the prior configuration or alternate
   setting are not valid in the new configuration or alternate setting. These
   invalid endpoints must be deactivated.

4. For details on a particular endpoint’s activation or deactivation, see
   Endpoint Activation (p. 286) and Endpoint Deactivation (p. 286).

5. Unmask the interrupt for each active endpoint and mask the interrupts for all
   inactive endpoints in the USB_USB_DAINTMSK register.

6. Set up the Data FIFO RAM for each FIFO. See Data FIFO RAM Allocation (p. 331)
   for more detail.

7. After all required endpoints are configured, the application must program the
   core to send a status IN packet. <===================

At this point, the device core is configured to receive and transmit any type of
data packet.

### Endpoint Activation


### Endpoint Deactivation


### Device connected to host

When the device is connected to a host.

From 15.4.2.1

1. The Session Request/New Session Detected Interrupt is triggered. This is
   signaled in the USB_GINTSTS.SESSREQINT [bit 30] interrupt bit.

2. When the device application detects the USB_GINTSTS.SESSREQINT interrupt, it
   programs the required bits in the USB_DCFG register.

3. When the Host drives Reset, the Device triggers USB_GINTSTS.USBRST [bit 12]
   on detecting theReset. The host then follows the USB 2.0 Enumeration sequence.


### Device Disconnection

When the device is disconnected from the host.
The Disconnect Detected Interrupt (DISCONNINT) is triggered only for hosts.


1. When the USB cable is unplugged or when the VBUS is the Host,  the OTG
   Interrupt  is triggered. This is signaled by the USB_GINTSTS.OTGINT [bit 2]
   interrupt bit.
3. When the device application detects the interrupt, it should check that the
   SESENDDET (Session End Detected) bit in the OTG Interrupt Register
   (USB_GOTGINT) is set to 1. This bit can be set only by the core and the
   application should write 1 to clear it.

### Doing for OUT transfers


From host to device

From 15.4.2.2, "In DMA Mode, the OTG host uses the AHB master Interface for
transmit packet data fetch (AHB to USB) and receive data update (USB to AHB).
The AHB master uses the programmed DMA address (USB_HCx_DMAADDR register in host
mode and USB_DIEPx_DMAADDR/USB_DOEPx_DMAADDR register in device mode) to access
the data buffers."

From 15.4.4.2.1

### Doing IN (Non periodic Bulk and Control) IN transfers

From  15.4.4.2.3.11, the requirements are:

Before setting up an IN transfer, the application must ensure that all data to
be transmitted as part of the IN transfer is part of a single buffer, and must
program the size of that buffer and its start address (in DMA mode) to the
endpoint-specific registers.

For IN transfers, the Transfer Size field in the Endpoint Transfer Size register
denotes a payload that constitutes multiple maximum-packet-size packets and a
single short packet. This short packet is transmitted at the end of the
transfer.

To transmit a few maximum-packet-size packets and a short packet at the end of
the transfer:

  Transfer size[epnum] = n * mps[epnum] + sp
   (where n is an integer >= 0, and 0 <= sp < mps[epnum])
   If (sp > 0), then packet count[epnum] = n + 1.
   Otherwise, packet count[epnum] = n a.

To transmit a single zero-length data packet:

   Transfer size[epnum] = 0 •
   Packet count[epnum] = 1 b

To transmit a few maximum-packet-size packets and a zero-length data packet at
the end of the transfer, the application must split the transfer in two parts.
The first sends maximum-packet-size data packets and the second sends the
zero-length data packet alone.

   First transfer:
      transfer size[epnum] = n * mps[epnum];
      packet count = n;
   Second transfer:
      transfer size[epnum] = 0; packet count = 1;

In DMA mode, the core fetches an IN data packet from the memory, always
starting at a DWORD boundary. If the maximum packet size of the IN endpoint
is not a multiple of 4, the application must arrange the data in the memory
with pads inserted at the end of a maximum-packet-size packet so that a new
packet always starts on a DWORD boundary.

Once an endpoint is enabled for data transfers, the core updates the Transfer
Size register. At the end of IN transfer, which ended with a Endpoint
Disabled interrupt, the application must read the Transfer Size register to
determine how much data posted in the transmit FIFO was already sent on the USB.

Data fetched into transmit FIFO =
   Application-programmed initial transfer size – core-updated final transfer size
Data transmitted on USB =
   (application-programmed initial packet count – Core updated final packet
   count) * mps[epnum] • Data yet to be transmitted on USB =
   (Application-programmed initial transfer size – data transmitted on USB)


The steps to transmit data to the host are:

1. Program the USB_DIEPx_TSIZ register with the transfer size and corresponding
   packet count. In DMA mode, also program the USB_DIEPx_DMAADDR register.

   USB_DIEPx_TSIZ.PKTCNT   = Packet count
   USB_DIEPx_TSIZ.XFERSIZE = Transfer size
   USB_DIEPx_DMAADDR = Address of data to be transfered

2. Program the USB_DIEPx_CTL register with the endpoint characteristics and set
   the CNAK and Endpoint Enable bits.

   USB_DIEPx_CTL.MPS  = MPS
   USB_DIEPx_CTL.USB_TXFNUM =
   USB_DIEPx_CTL.EPENA = 1
   USB_DIEPx_CTL.EPTYPE = 0

   To clear
   USB_DIEPx_CTL.STALL = 1
   USB_DIEPx_CTL.CNAK  = 1
   USB_DIEPx_CTL.SNAK  = 1


3. In slave mode when transmitting non-zero length data packet, the application
   must poll the USB_DIEPx_TXFSTS register (where x is the FIFO number
   associated with that endpoint) to determine whether there is enough space in
   the data FIFO. The application can optionally use USB_DIEPx_INT.TXFEMP before
   writing the data.


### Configuring FIFO

From 15.4.7.1.


When USB is a device, there must be areas in FIFO for:

* Incoming (receive) packets: this is shared by all endpoints
* Outgoing (transmit) packets: there are many, one for each IN endpoint used.

as shown below.


      SA_RX=0000              +--------------------------+
                              |rx_fifo_size (SZ_RX)      |
      SA_TX0 = SIZ_RX         +--------------------------+
                              |tx_fifo_size #0 (SZ_TX0)  |
      SA_TX1 = SA_TX0+SZ_TX0  +--------------------------+
                              |tx_fifo_size #1 (SZ_TX1)  |
      SA_TX2 = SA_TX1+SZ_TX1  +--------------------------|
                              |    and so on.            |


The registers used for configurations are:

| Register.Field                 | Name            |  Value               |
|--------------------------------|-----------------|----------------------|
| USB_GRXFSIZ.RXFDEP             | RxFIFO Depth    | SZ_RX                |
| USB_GNPTXFSIZ.NPTXFINEPTXF0DEP | TxFIFO 0 Depth  | SZ_TX0               |
| USB_GNPTXFSIZ.NPTXFSTADDR      | Start Address   | SA_TX0=SZ_RX         |
| USB_DIEPTXF1.INEPNTXFDEP       | TxFIFO 1 Depth  | SZ_TX1               |
| USB_DIEPTXF1.INEPNTXFSTADDR    | Start Address   | SA_TX1=SA_TX0+SZ_TX0 |
| USB_DIEPTXF2.INEPNTXFDEP       | TxFIFO 2 Depth  | SZ_TX2               |
| USB_DIEPTXF2.INEPNTXFSTADDR    | Start Address   | SA_TX2=SA_TX1+SZ_TX1 |
| USB_DIEPTXF3.INEPNTXFDEP       | TxFIFO 3 Depth  | SZ_TX3               |
| USB_DIEPTXF3.INEPNTXFSTADDR    | Start Address   | SA_TX3=SA_TX2+SZ_TX2 |
| USB_DIEPTXF4.INEPNTXFDEP       | TxFIFO 4 Depth  | SZ_TX4               |
| USB_DIEPTXF4.INEPNTXFSTADDR    | Start Address   | SA_TX4=SA_TX3+SZ_TX3 |
| USB_DIEPTXF5.INEPNTXFDEP       | TxFIFO 5 Depth  | SZ_TX5               |
| USB_DIEPTXF5.INEPNTXFSTADDR    | Start Address   | SA_TX5=SA_TX4+SZ_TX4 |
| USB_DIEPTXF6.INEPNTXFDEP       | TxFIFO 6 Depth  | SZ_TX6               |
| USB_DIEPTXF6.INEPNTXFSTADDR    | Start Address   | SA_TX6=SA_TX5+SZ_TX5 |


Size of FIFO

Following 15.4.7.1

When using DMA mode, the depth must include an additional location per channel,
because the DMA Address Registers USB_DIEPx_DMAADDR/USB_DOEPx_DMAADDR for
device mode and USB_HCxx_DMAADDR for host mode are stored in FIFO.

According 15.4.4.1

RxFIFO: At a minimum, this must be equal to 1 max packet size of control
endpoint 0 + 2 DWORDs (for the status of the control OUT data packet) +
10 DWORDs (for setup packets).

TxFIFO: At a minimum, this must be equal to 1 max packet size of control
endpoint 0.


According 15.4.4.2.1.1

The application must always allocate some extra space in the Receive Data FIFO,
to be able to receive up to three SETUP packets on a control endpoint.The space
to be Reserved is (4 * n) + 6 DWORDs, where n is the number of control endpoints
supported by the device. Three DWORDs are required for the first SETUP packet, 1
DWORD is required for the Setup Stage Done DWORD, and 6 DWORDs are required to
store two extra SETUP packets among all control endpoints. • 3 DWORDs per SETUP
packet are required to store 8 bytes of SETUP data and 4 bytes of SETUP status
(Setup Packet Pattern). The core reserves this space in the receive data FIFO to
write SETUP data only, and never uses this space for data packets.

According 15.4.7.1.1.1

The Receive FIFO RAM must have:
• RAM for SETUP Packets: 4 * n + 6 locations must be Reserved in the receive
FIFO to receive up to n SETUP packets on control endpoints, where n is the
number of control endpoints the device core supports. The core does not use
these locations, which are Reserved for SETUP packets, to write any other data.
• One location for Global OUT NAK
• Status information is written to the FIFO along with each received packet.
Therefore, a minimum space of (Largest Packet Size / 4) + 1 must be allotted to
receive packets. If a high-bandwidth endpoint is enabled, or multiple
isochronous endpoints are enabled, then at least two (Largest Packet Size / 4) +
1 spaces must be allotted to receive back-to-back packets. Typically, two
(Largest Packet Size / 4) + 1 spaces are recommended so that when the previous
packet is being transferred to AHB, the USB can receive the subsequent packet.
If AHB latency is high, you must allocate enough space to receive multiple
packets. This is critical to prevent dropping any isochronous packets. • Along
with each endpoint's last packet, transfer complete status information is also
pushed to the FIFO. Typically, one location for each OUT endpoint is
recommended.

Transmit FIFO RAM Allocation must have:

The minimum RAM space required for each IN Endpoint Transmit FIFO is the maximum
packet size for that particular IN endpoint.

More space allocated in the transmit IN Endpoint FIFO results in a better
performance on the USB and can hide latencies on the AHB.

Table 15.3 from 15.4.7.1.1.1

| FIFO Name | Data RAM Size    |
|-----------|------------------|
| RxFIFO    | rx_fifo_size.    |
| TxFIFO 0  | tx_fifo_size[0]  |
| TxFIFO 1  | tx_fifo_size[1]  |
| TxFIFO 2  | tx_fifo_size[2]  |
| ...       |  ....            |
| TxFIFO i  | tx_fifo_size[i]  |
| ...       |  ....            |
| TxFIFO 6  | tx_fifo_size[6]  |

rx_fifo_size must include RAM for setup packets, OUT
endpoint control information and data OUT packets, as
mentioned earlier.


| Register       | Size Field |  Value      | Address Field  | Value       |
|----------------|------------|-------------|----------------|-------------|
| USB_GRXFSIZ    | RXFDEP     | rx_fifo_size|       -        |    -        |
| USB_GNPTXFSIZ  |            | tx_fifo_size[0]|             | rx_fifo_size|
| USB_DIEPTXF1   |
| USB_DIEPTXF2   |
|   ....         |
| USB_DIEPTXFi   |
|   ....         |
| USB_DIEPTXF6   |





### Control Packets

For slow and full speed

Token:     PID | ADDR | ENDP | CRC5             PID=IN|OUT|SETUP|SOF
Data:      PID | DATA | CRC16                   PID=DATA0|DATA1
Handshake: PID                                  PID=ACK|NAK|STALL
SOF:       PID | FRAMEN | CRC5                  PID=SOF



# From USB Device Example

From em_usbtypes.h

   #define NP_RX_QUE_DEPTH       8
   #define HP_RX_QUE_DEPTH       8
   #define MAX_XFER_LEN          524287L       /* 2^19 - 1 bytes             */
   #define MAX_PACKETS_PR_XFER   1023          /* 2^10 - 1 packets           */
   #define MAX_NUM_TX_FIFOS      6             /* In addition to EP0 Tx FIFO */
   #define MAX_NUM_IN_EPS        6             /* In addition to EP0         */
   #define MAX_NUM_OUT_EPS       6             /* In addition to EP0         */
   #define MAX_DEVICE_FIFO_SIZE_INWORDS 512U
   #define MIN_EP_FIFO_SIZE_INWORDS  16U         /* Unit is words (32bit)      */
   #define MIN_EP_FIFO_SIZE_INBYTES  64U         /* Unit is bytes (8bit)       */
   #define MAX_USB_EP_NUM      15


   typedef enum {
      D_EP_IDLE          = 0,
      D_EP_TRANSMITTING  = 1,
      D_EP_RECEIVING     = 2,
      D_EP0_IN_STATUS    = 3,
      D_EP0_OUT_STATUS   = 4
   } USBD_EpState_TypeDef;

   typedef struct {
      bool                        in;
      uint8_t                     zlp;
      uint8_t                     num;
      uint8_t                     addr;
      uint8_t                     type;
      uint8_t                     txFifoNum;
      uint8_t                     *buf;
      uint16_t                    packetSize;
      uint16_t                    mask;
      uint32_t                    remaining;
      uint32_t                    xferred;
      uint32_t                    hwXferSize;
      uint32_t                    fifoSize;
      USBD_EpState_TypeDef        state;
      USB_XferCompleteCb_TypeDef  xferCompleteCb;
   } USBD_Ep_TypeDef;


   typedef struct {
      USB_Setup_TypeDef                     *setup;
      USB_Setup_TypeDef                     setupPkt[3];
      uint8_t                               configurationValue; /* Must be DWORD aligned */
      bool                                  remoteWakeupEnabled;
      uint8_t                               numberOfStrings;
      uint8_t                               numberOfInterfaces;
      USBD_State_TypeDef                    state;
      USBD_State_TypeDef                    savedState;
      USBD_State_TypeDef                    lastState;
      const USB_DeviceDescriptor_TypeDef    *deviceDescriptor;
      const USB_ConfigurationDescriptor_TypeDef *configDescriptor;
      const void * const                    *stringDescriptors;
      const USBD_Callbacks_TypeDef          *callbacks;
      USBD_Ep_TypeDef                       ep[ NUM_EP_USED + 1 ];
      uint8_t                               inEpAddr2EpIndex[  MAX_USB_EP_NUM + 1 ];
      uint8_t                               outEpAddr2EpIndex[ MAX_USB_EP_NUM + 1 ];
      uint32_t                              ep0MpsCode;
   } USBD_Device_TypeDef;



#define SL_MIN(a, b) ((a) < (b) ? (a) : (b))

From em_core.h

   typedef uint32_t CORE_irqState_t;
   #define CORE_DECLARE_IRQ_STATE        CORE_irqState_t irqState
   #define CORE_ENTER_ATOMIC()           irqState = CORE_EnterAtomic()
   #define CORE_EXIT_ATOMIC()            CORE_ExitAtomic(irqState)
   #define CORE_YIELD_ATOMIC()           CORE_YieldAtomic(void)

In em_core.c. There are two ways to implement CORE_EnterAtomic. One uses
get_BASEPRI and other get_PRIMASK
'
   SL_WEAK CORE_irqState_t CORE_EnterAtomic(void)    {
   #if (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)
   CORE_irqState_t irqState = __get_BASEPRI();
   __set_BASEPRI(CORE_ATOMIC_BASE_PRIORITY_LEVEL << (8 - __NVIC_PRIO_BITS));
   return irqState;
   #else
   CORE_irqState_t irqState = __get_PRIMASK();
   __disable_irq();
   return irqState;
   #endif // (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)
}

   SL_WEAK void CORE_ExitAtomic(CORE_irqState_t irqState) {
   #if (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)
   __set_BASEPRI(irqState);
   #else
   if (irqState == 0) {
      __enable_irq();
   }
   #endif // (CORE_ATOMIC_METHOD == CORE_ATOMIC_METHOD_BASEPRI)
   }

From em_usbhal.h

   uint32_t USBHAL_GetCoreInts( void ) {
   uint32_t retVal;

   retVal  = USB->GINTSTS;
   retVal &= USB->GINTMSK;

   return retVal;
   }


The Init routine is huge!!!!!. It uses data from a structure to initialize
the USB as a device.

For a CDC the struture is initialized as

	static const USBD_Init_TypeDef usbInitStruct =
	{
			.deviceDescriptor    = &USBDESC_deviceDesc,
			.configDescriptor    = USBDESC_configDesc,
			.stringDescriptors   = USBDESC_strings,
			.numberOfStrings     = sizeof(USBDESC_strings)/sizeof(void*),
			.callbacks           = &callbacks,
			.bufferingMultiplier = USBDESC_bufferingMultiplier,
			.reserved            = 0
	};


   int USBD_Init( const USBD_Init_TypeDef *p ) {
      int numEps;
      USBD_Ep_TypeDef *ep;
      uint8_t txFifoNum;
      uint8_t *conf, *confEnd;
      #if defined( CMU_OSCENCMD_USHFRCOEN )
      SYSTEM_ChipRevision_TypeDef chipRev;
      #endif
      USB_EndpointDescriptor_TypeDef *epd;
      USB_InterfaceDescriptor_TypeDef *id;
      uint32_t totalRxFifoSize, totalTxFifoSize, numInEps, numOutEps;

   CORE_DECLARE_IRQ_STATE;

   // Set clock to HFXO (=48 MHz)

   // Enable 32 KHz clock  for suspended mode

   // Initialize a timer
   USBTIMER_Init();

   // Initialize a dev structure
   memset( dev, 0, sizeof( USBD_Device_TypeDef ) );

   dev->setup                = dev->setupPkt;
   dev->deviceDescriptor     = p->deviceDescriptor;
   dev->configDescriptor     = (USB_ConfigurationDescriptor_TypeDef*)
                              p->configDescriptor;
   dev->stringDescriptors    = p->stringDescriptors;
   dev->numberOfStrings      = p->numberOfStrings;
   dev->state                = USBD_STATE_LASTMARKER;
   dev->savedState           = USBD_STATE_NONE;
   dev->lastState            = USBD_STATE_NONE;
   dev->callbacks            = p->callbacks;
   dev->remoteWakeupEnabled  = false;

   /* Initialize EP0 */
   ep                 = &dev->ep[ 0 ];
   ep->in             = false;
   ep->buf            = NULL;
   ep->num            = 0;
   ep->mask           = 1;
   ep->addr           = 0;
   ep->type           = USB_EPTYPE_CTRL;
   ep->txFifoNum      = 0;
   ep->packetSize     = p->deviceDescriptor->bMaxPacketSize0;

   dev->ep0MpsCode = _USB_DOEP0CTL_MPS_64B;

   ep->remaining      = 0;
   ep->xferred        = 0;
   ep->state          = D_EP_IDLE;
   ep->xferCompleteCb = NULL;
   ep->fifoSize       = ep->packetSize / 4;

   totalTxFifoSize = ep->fifoSize * p->bufferingMultiplier[ 0 ];
   totalRxFifoSize = (ep->fifoSize + 1) * p->bufferingMultiplier[ 0 ];

   /* Parse configuration decriptor */
   numEps = 0;
   numInEps  = 0;
   numOutEps = 0;
   conf = (uint8_t*)dev->configDescriptor;
   confEnd = conf + dev->configDescriptor->wTotalLength;

   txFifoNum = 1;
   while ( conf < confEnd ) {
      ASSERT( *conf != 0 );

      if ( *(conf + 1) == USB_ENDPOINT_DESCRIPTOR ) {
         numEps++;
         epd = (USB_EndpointDescriptor_TypeDef*)conf;
         ep                 = &dev->ep[ numEps ];

         ep->in             = ( epd->bEndpointAddress & USB_SETUP_DIR_MASK ) != 0;
         ep->buf            = NULL;
         ep->addr           = epd->bEndpointAddress;
         ep->num            = ep->addr & USB_EPNUM_MASK;
         ep->mask           = 1 << ep->num;
         ep->type           = epd->bmAttributes & CONFIG_DESC_BM_TRANSFERTYPE;
         ep->packetSize     = epd->wMaxPacketSize;
         ep->remaining      = 0;
         ep->xferred        = 0;
         ep->state          = D_EP_IDLE;
         ep->xferCompleteCb = NULL;

         ASSERT( p->bufferingMultiplier[ numEps ] != 0 );

      if ( ep->in ) {
         numInEps++;
         ep->txFifoNum = txFifoNum++;
         ep->fifoSize = ( ( ep->packetSize + 3 ) / 4 )
                        * p->bufferingMultiplier[ numEps ];
         dev->inEpAddr2EpIndex[ ep->num ] = numEps;
         totalTxFifoSize += ep->fifoSize;
         if ( ep->num > MAX_NUM_IN_EPS ) {
            DEBUG_USB_API_PUTS( "\nUSBD_Init(), Illegal IN EP address" );
            EFM_ASSERT( false );
            return USB_STATUS_ILLEGAL;
         }
      } else {
         numOutEps++;
         ep->fifoSize = ( ( ( ep->packetSize + 3 ) / 4 ) + 1 )
                        * p->bufferingMultiplier[ numEps ];
         dev->outEpAddr2EpIndex[ ep->num ] = numEps;
         totalRxFifoSize += ep->fifoSize;
         if ( ep->num > MAX_NUM_OUT_EPS )
         {
            DEBUG_USB_API_PUTS( "\nUSBD_Init(), Illegal OUT EP address" );
            EFM_ASSERT( false );
            return USB_STATUS_ILLEGAL;
         }
      }
    } else if ( *(conf + 1) == USB_INTERFACE_DESCRIPTOR ) {
      id = (USB_InterfaceDescriptor_TypeDef*)conf;

      if ( id->bAlternateSetting == 0 ) {    // Only check default interfaces
         ASSERT ( dev->numberOfInterfaces == id->bInterfaceNumber );
         dev->numberOfInterfaces++;
      }
    }
    conf += *conf;
  }

   /* Rx-FIFO size: SETUP packets : 4*n + 6    n=#CTRL EP's
    *               GOTNAK        : 1
    *               Status info   : 2*n        n=#OUT EP's (EP0 included) in HW
    */
   totalRxFifoSize += 10 + 1 + ( 2 * (MAX_NUM_OUT_EPS + 1) );

   ASSERT( dev->configDescriptor->bNumInterfaces == dev->numberOfInterfaces );

   ASSERT( numEps == NUM_EP_USED );

   ASSERT( numInEps > MAX_NUM_IN_EPS );

   ASSERT( numOutEps > MAX_NUM_OUT_EPS );

   CORE_ENTER_ATOMIC();

  /* Enable USB clock */
  CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB | CMU_HFCORECLKEN0_USBC;

   CMU_ClockSelectSet( cmuClock_USBC, cmuSelect_HFCLK );

   USBHAL_DisableGlobalInt();

   ASSERT(USBDHAL_CoreInit( totalRxFifoSize, totalTxFifoSize )!=USB_STATUS_OK);
      USBDHAL_EnableUsbResetAndSuspendInt();
      USBHAL_EnableGlobalInt();
      NVIC_ClearPendingIRQ( USB_IRQn );
      NVIC_EnableIRQ( USB_IRQn );

   #if ( USB_PWRSAVE_MODE & USB_PWRSAVE_MODE_ONVBUSOFF )
      if ( USBHAL_VbusIsOn() ) {
         USBD_SetUsbState( USBD_STATE_POWERED );
      }
      else
   #endif
      {
         USBD_SetUsbState( USBD_STATE_NONE );
      }

      CORE_EXIT_ATOMIC();
      return USB_STATUS_OK;
   }

From usbdint.c


   void USB_IRQHandler( void ) {


   CORE_DECLARE_IRQ_STATE;

   CORE_ENTER_ATOMIC();

      /* Switch USBC clock from 32kHz to a 48MHz clock to be able to  */
      /* read USB peripheral registers.                               */
      /* If we woke up from EM2, HFCLK is now HFRCO.                  */

      /* Restore clock oscillators.*/

      /* Select correct USBC clock.*/

      /* Check Interrupt flags */

      /* If regulator is active */
   if ( USB->IF && ( USB->CTRL & USB_CTRL_VREGOSEN ) ) {
      /* If VBUS > 1.8 V */
      if ( USB->IF & USB_IF_VREGOSH ) {
         /* Clear it */
         USB->IFC = USB_IFC_VREGOSH;

         /* If VREGO > 1.8 V/
         if ( USB->STATUS & USB_STATUS_VREGOS ) {
            servedVbusInterrupt = true;
            DEBUG_USB_INT_LO_PUTS( "\nVboN" );

            USBD_SetUsbState( USBD_STATE_POWERED );
         }
      }

      /* If VBUS < 1.8 V */
      if ( USB->IF & USB_IF_VREGOSL ) {
         /* Clear it */
         USB->IFC = USB_IFC_VREGOSL;

         if ( ( USB->STATUS & USB_STATUS_VREGOS ) == 0 )          {
            servedVbusInterrupt = true;
            DEBUG_USB_INT_LO_PUTS( "\nVboF" );

            USBD_SetUsbState( USBD_STATE_NONE );
   #endif
         }
      }
   }

   status = USBHAL_GetCoreInts();
   /* If no more interrupts */
   if ( status == 0 ) {
      CORE_EXIT_ATOMIC();
      if ( !servedVbusInterrupt ) {
         DEBUG_USB_INT_LO_PUTS( "\nSinT" );
      }
      return;
   }


   #define HANDLE_INT( x ) if ( status & x ) { Handle_##x(); status &= ~x; }


   HANDLE_INT( USB_GINTSTS_RESETDET   )
   HANDLE_INT( USB_GINTSTS_WKUPINT    )
   HANDLE_INT( USB_GINTSTS_USBSUSP    )
   HANDLE_INT( USB_GINTSTS_SOF        )
   HANDLE_INT( USB_GINTSTS_ENUMDONE   )
   HANDLE_INT( USB_GINTSTS_USBRST     )
   HANDLE_INT( USB_GINTSTS_IEPINT     )
   HANDLE_INT( USB_GINTSTS_OEPINT     )

   CORE_EXIT_ATOMIC();

   if ( status != 0 ) {
      DEBUG_USB_INT_LO_PUTS( "\nUinT" );
      }
   }
















USBD_ArmEP0(ep) {

   if( ep->in ) { // IN
      if( ep->remaining == 0 )
         ep->zlp = 1;

      HAL_SetEp0DMAAddrIN(ep->buff)
      Size = SL_MIN(ep->remaining, ep->packetsize)
      HAL_StartEp0IN( , dev->ep0MpsCode )

   } else { // OUT
      HAL_SetEp0DMAAddrOUT(ep->buff);
      HAL_StartEp0_OUT(ep->packetsize,dev->ep0->MpsCode)

   }
}


USBD_ArmEPN(ep) {

   if( ep->in ) { // IN
      HAL_StartEp0IN( ep )
   } else { // OUT
      HAL_StartEp0_OUT(ep->packetsize,dev->ep0->MpsCode)
   }
}

USBD_ArmEp(ep) {

   if( ep->num==0 )
      USBD_ArmEp0(ep)
   else
      USBD_ArmEpN(ep)
}



USBD_Read( addr, data, len, callback ) {

   ep = GetEpFromEpAddr(addr);

   // Check for inconsistencies
   // ep must be OUT: ASSERT(ep->in == false)
  CORE_ENTER_ATOMIC();

   ep->buf      = data;
   ep->remaining= len;
   ep->xferred  = 0
   ep->state    = D_EP_RECEIVING;
   ep->xferCompleteCb = callback;

   USBD_ArmEp(ep);
  CORE_EXIT_ATOMIC();
}

USBD_Write( addr, data, len, callback ) {

   ep = GetEpFromEpAddr(addr);

   // Check for inconsistencies
   // ep must be IN: ASSERT(ep->in)

   ep->buf      = data;
   ep->remaining= len;
   ep->xferred  = 0
   ep->state    = D_EP_TRANSMITTING;
   ep->xferCompleteCb = callback;

   USBD_ArmEp(ep);
}





OBS: SysTick is an optional feature in Cortex M0/M0+.

There maximum number of exceptions depends on th Cortex model.

| Cortex    |  # IRQs    |
|-----------|------------|
|    M0     |     16     |
|    M0+    |    240     |
|    M1     |    240     |
|    M3     |    240     |
|    M4     |    240     |
|    M7     |    240     |
|    M23    |    240     |
|    M33    |    480     |

Each interrupt has a priority that is a number that  starts from 0 (highest
priority). The number of priority is defined by the manufacturer, and it is
generally 8 or 16.

> NOTE: Care must be taken, because the default priority is 0 for all
> interrupts.

void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
