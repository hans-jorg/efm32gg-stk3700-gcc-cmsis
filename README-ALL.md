Developing for the EFM32GG-STK3700 Development Board
====================================================



MIT License
-------

Copyright (c) 2018 Hans Jorg Andreas Schneebeli

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Foreword
-------

This material is intended as a companion for the lectures in a course in Embedded Systems. The main objective is to present the inner works of the compilation process and introduce advanced patterns of software for embedded systems.

It assumes a basic knowledge of C. It consists of many small projects, each introducing one, and sometimes two, aspects of the development process.

Appendices show how to install the needed tools, how to use them and the structure of a project.

The EFM32GG-STK3700 Development Board
------------------------------


The EFM32 Giant Gecko is a family of Cortex M3 microcontrollers manufactured by Silicon Labs (who bought Energy Micro, the initial manufacturer). The EFM32 microcontroller family has many subfamilies with different Cortex-M architectures and features as shown below.


Family         | Core              | Features | Flash (kB) | RAM (kB)
---------------|-------------------|----------|------------|---------------
Zero Gecko     |   ARM Cortex-M0+  |          |   4- 32    | 2- 4
Happy Gecko    |   ARM Cortex-M0+  | USB      |  32- 64    | 4- 8
Tiny Gecko     |   ARM Cortex-M3   | LCD      |   4- 32    | 2- 4
Gecko          |   ARM Cortex-M3   | LCD      |  16- 128   | 8-16
Jade Gecko     |   ARM Cortex-M3   |          | 128-1024   | 32-256
Leopard Gecko  | ARM Cortex-M3     | USB, LCD |  64- 256   | 32
Giant Gecko    | ARM Cortex-M3     | USB. LCD | 512-1024   | 128
Giant Gecko S1 | ARM Cortex-M4     | USB, LCD | 2048       | 512
Pearl Gecko    |  ARM Cortex-M4    |          | 128-1024   | 32-256
Wonder Gecko   |  ARM Cortex-M4    | USB, LCD |  64- 256   | 32


##The EMF32GG-STK3700 Development Board


The EMF32GG-STK3700 is a development board featuring a EFM32GG990F1024 MCU (a Giant Gecko microcontroller) with 1 MB Flash memory and 128 kB RAM. It has also the following peripherals:

-   160 segment LCD
-   2 user buttons, 2 user LEDs and a touch slider
-   Ambient Light Sensor and inductive-capacitive metal sensor
-   EFM32 OPAMP footprint
-   32 MB NAND flash
-   USB interface for Host/Device/OTG

It has a 20 pin expansion header, breakout pads for easy access to I/O pins, different alternatives for power sources including USB and a 0.03 F Super Capacitor for backup power domain.

For developing, there is an Integrated Segger J-Link USB debugger/emulator with debug out functionality and an Advanced Energy Monitoring system for precise current tracking.

The development board EMF32GG-STK3700 features a EFM32GG990F1024 microcontroller from the Giant Gecko family. It is a Cortex M3 processor with the following features:


Flash (KB)  | RAM (KB)  |  GPIO  |  USB |  LCD   | USART/UART  | LEUART  | Timer/PWMRTC  |  ADC |  DAC | OpAmp
------------|-----------|--------|------|--------|-------------|---------|---------------|------|------|--------
    1024    |  128      |    87  |  Y   |  8x34  |     3/2     |    2 2  |      4/12     |  1(8)| 2(8) |  3 


##Basic References

The most important references are:

-   [EFM32GG Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)[1]: Manual describing all peripherals, memory map.
-   [EFM32GG-STK3700 Giant Gecko Starter Kit User's Guide](https://www.silabs.com/documents/public/user-guides/efm32gg-stk3700-ug.pdf)[2]: Information about the STK3700 Board.
-   [EFM32GG990 Datasheet](https://www.silabs.com/documents/public/data-sheets/EFM32GG990.pdf)[3]: Technical information about the EMF32GG990F1024 including electrical specifications and pinout.
-   [EFM32 Microcontroller Family Cortex M3 Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32-Cortex-M3-RM.pdf)[4]

##Peripherals

There are a lot of peripherals in the board.

-   LEDs using pins PE2 and PE3.
-   Buttons using pins PB9 and PB10
-   LCD Multiplexed 20×8 with a 7 character alphanumeric field, a 4 digit numeric field and other symbols using pins PA0-PA11, PA15,PB0-PB6,PD9-PD12,PE4-PE7.
-   Touch Sensor using pins PC8-11.
-   Light Sensor using PD6,PC6
-   LC Sensor using PB12,PC7
-   NAND Flash using PB15, PE8-15, PC1-2, PF8-9, PD13-15
-   USB OTG using PF11-12, PF5-6

It is also possible to use the header connectors to add more peripherals.

##Connections

The EMF32GG-STK3700 board has two USB connectors: One, a Mini USB B Connector, for development and other, a Micro B-Type USB Connector, for the application.

For development, a cable (delivered) must be connected between the Mini USB connector on the board and the A-Type connector on the PC. Among the devices listed by the *lsusb* command, the following must appear.

    Bus 001 Device 019: ID 1366:0101 SEGGER J-Link PLUS

The STK3700 board has two microcontrollers: one, called target, is a EFM32GG990F1024 microcontroller, and the other, called Board Controller, implements an interface for programming and debugging.

For a Cortex M microcontroller the following programming interfaces are used:

-   SWD : 2 pinos: SWCLK, SWDIO - Serial Wire Debug.
-   JTAG : 4 pinos: TCK, TMS, TDI, TDO - Not used in this board

In the STK3700 only the SWD interface is used, and there is a connector on the SWD lines which permits the debugging of off-board microcontrollers.

Generally, in this kind of boards there is a serial interface between the Target and the Board Controller. It can be implemented using a physical channel with 2 lines or a virtual, using the SWD/JTAG channel. Both appears to the Host PC as a serial virtual port (*COMx* or */dev/ttyACMx*). In the STK3700 board the serial channel uses the UART0 unit (pins PE0 and PE1).

##Examples for the EFM32GG-STK3700 Development Board

In all examples, a direct access to registers approach was used. It means that no library besides CMSIS was used.

#*0* Using C


##Access to registers

The peripherals and many functionalities of the CPU are controlled accessing and modifying registers.

Silicon Labs as others manufactures provides a set of header with symbols that can be used to access the registers. Following CMSIS standard, the registers are grouped in C *struct*’s.

As an example, the *struct* to access the register for a UART is defined as below. The strange looking comment are part of the documentation using Doxygen.

    typedef struct {
         __IOM uint32_t CTRL; /**< Control Register */
         __IOM uint32_t FRAME; /**< USART Frame Format Register */
         __IOM uint32_t TRIGCTRL; /**< USART Trigger Control register */
         __IOM uint32_t CMD; /**< Command Register */
         __IM uint32_t STATUS; /**< USART Status Register */
         __IOM uint32_t CLKDIV; /**< Clock Control Register */
         __IM uint32_t RXDATAX; /**< RX Buffer Data Extended Register */
         __IM uint32_t RXDATA; /**< RX Buffer Data Register */
         __IM uint32_t RXDOUBLEX; /**< RX Buffer Double Data Extended Register */
         __IM uint32_t RXDOUBLE; /**< RX FIFO Double Data Register */
         __IM uint32_t RXDATAXP; /**< RX Buffer Data Extended Peek Register */
         __IM uint32_t RXDOUBLEXP; /**< RX Buffer Double Data Extended Peek Register*/
         __IOM uint32_t TXDATAX; /**< TX Buffer Data Extended Register */
         __IOM uint32_t TXDATA; /**< TX Buffer Data Register */
         __IOM uint32_t TXDOUBLEX; /**< TX Buffer Double Data Extended Register */
         __IOM uint32_t TXDOUBLE; /**< TX Buffer Double Data Register */
         __IM uint32_t IF; /**< Interrupt Flag Register */
         __IOM uint32_t IFS; /**< Interrupt Flag Set Register */
         __IOM uint32_t IFC; /**< Interrupt Flag Clear Register */
         __IOM uint32_t IEN; /**< Interrupt Enable Register */
         __IOM uint32_t IRCTRL; /**< IrDA Control Register */
         __IOM uint32_t ROUTE; /**< I/O Routing Register */
         __IOM uint32_t INPUT; /**< USART Input Register */
         __IOM uint32_t I2SCTRL; /**< I2S Control Register */
    } USART_TypeDef; /** @} */

The base addresses of the USART units are defined as

    #define USART0_BASE (0x4000C000UL) /**< USART0 base address */
    #define USART1_BASE (0x4000C400UL) /**< USART1 base address */
    #define USART2_BASE (0x4000C800UL) /**< USART2 base address */
    
And the symbols to access the units as
    
    #define USART0 ((USART_TypeDef *) USART0_BASE) /**< USART0 base pointer */
    #define USART1 ((USART_TypeDef *) USART1_BASE) /**< USART1 base pointer */
    #define USART2 ((USART_TypeDef *) USART2_BASE) /**< USART2 base pointer */

To read a register, just write

    uint32_t w = USART0->CTRL; // Read
    ...
    USART0->CTRL = w; // Write

Each register can have many fields. Some fields are just one bit long. One bit fields can be modified with one operation. For example, to set the *SYNC* bit at position 0, use one of these forms (the first is more used). An or operator must be used (Never use addition represented by a *+* because if the bit is already set, it gives wrong results).

    USART0->CTRL |= 1;
    USART0->CTRL = USART0->CTRL | 1;

To clear it, one of these forms (Do not use minus represented by a *-* )

    USART0->CTRL &= ~1;
    
    USART0->CTRL &= 0xFFFFFFFEU;
    
    USART0->CTRL = USART0->CTRL & ~1;
    USART0->CTRL = USART0->CTRL & 0xFFFFFFFEU;

The legibility can be enhanced using a macro *BIT* defined as

    USART0->CTRL |= BIT(0); // Set bit
    USART0->CTRL &= ~BIT(0); // Clear bit
    
But much better is the use of symbols defined in the header for the field.

    USART0->CTRL |= USART_CTRL_SYNC; // Set bit
    USART0->CTRL &= ~USART_CTRL_SYNC; // Clear bit
    
There are some fields, that are many bits long. The procedure to modify them is different. One must be assured that the field is all zero, before setting a new value using an or operation. To set the *OVS* field, that has 2 bits, it must be cleared first.

    uint32_t w = USART0->CTRL;
    w &= ~0x60;
    w |= 0x40;
    USART0->CTRL = w;
    
Or in just one line

    USART0->CTRL = (USART0->CTRL&~0x60)|0x40;

Or using shift operators

    USART0->CTRL = (USART0->CTRL&~(3<<5)|(2<<5);
    
Again, a better way is to use symbols defined in the header.
    
    USART0->CTRL = USART0->CTRL&~_USART_CTRL_OVS_MASK)|USART_CTRL_OVS_X8;

Attention with the underscore before the symbol for the mask. Generally symbols started by underscore are reserved for implementation and should be not used in production code. But the symbols ending in MASK, and in a minor scale, in SHIFT symbols are useful and very used. It is not necessary to use the other symbols, whose names start with underscore. Actually, it is very dangerous because their values are not in the correct position and they must be shifted to the correct position to give the correct results. This can be seen in the header, as shown below, but with the comments omitted for better readability.

    #define _USART_CTRL_OVS_SHIFT 5
    #define _USART_CTRL_OVS_MASK 0x60UL
    #define _USART_CTRL_OVS_DEFAULT 0x00000000UL
    #define _USART_CTRL_OVS_X16 0x00000000UL
    #define _USART_CTRL_OVS_X8 0x00000001UL
    #define _USART_CTRL_OVS_X6 0x00000002UL
    #define _USART_CTRL_OVS_X4 0x00000003UL
    #define USART_CTRL_OVS_DEFAULT (_USART_CTRL_OVS_DEFAULT << 5)
    #define USART_CTRL_OVS_X16 (_USART_CTRL_OVS_X16 << 5)
    #define USART_CTRL_OVS_X8 (_USART_CTRL_OVS_X8 << 5)
    #define USART_CTRL_OVS_X6 (_USART_CTRL_OVS_X6 << 5)
    #define USART_CTRL_OVS_X4 (_USART_CTRL_OVS_X4 << 5)
    
When setting a field with a user calculated value, it is a good idea to ensure that no other bit in the register undetected is modified. For example, there is a 3-bit field called HFCLKDIV, that specifies the divisor of the crystal frequency. Yo set it to *div*, the following instruction can be used.

    CMU->CTRL = (CMU->CTRL&~_CMU_CTRL_HFCLKDIV_MASK)|(div<<_CMU_CTRL_HFCLKDIV_SHIFT)&_CMU_CTRL_HFCLKDIV_MASK));

Or using a multi step approach.

    uint32_t w = CMU->CTRL; // Get present value
    w &= ~_CMU_CTRL_HFCLKDIV_MASK; // Set field HFCLKDIV to 0
    uint32_t n = div<<_CMU_CTRL_HFCLKDIV_SHIFT; // Set field with new value
    n &= _CMU_CTRL_HFCLKDIV_MASK; // Ensure it does not extrapolate
    w |= n; // Calculate new value
    CMU->CTRL = w; // Set new value

##About CMSIS

CMSIS stands for *Cortex Microcontroller Software Interface Standard* and is a initiative from ARM to make easier to learn and port applications between ARM Cortex M processors. It defines the headers, initialization code, libraries and many other aspects related to Cortex M programming.

The CMSIS is built on many components. One of them creates a Hardware Abstraction Layer (HAL) for the ARM peripherals but not for the manufactured added ones. It standardizes the access to registers and resources of the microcontroller. This enables the developing without reliying too much on libraries provided by the manufacturer, that hampers the portability. See [CMSIS Site](https://developer.arm.com/embedded/cmsis)[6].

###CMSIS Header files

TBD!!!!

###NVIC functions

Some operations can not be done directly in C. There is a set of standard functions to execute some operations as listed below.

|     CMSIS function                  |   Description                                             |
|-------------------------------------|-----------------------------------------------------------|
| void NVIC_EnableIRQ(IRQn_Type IRQn) |   Enables an interrupt or exception.                      |
| void NVIC_DisableIRQ(IRQn_Type IRQn)	| Disables an interrupt or exception. | 
| void NVIC_SetPendingIRQ(IRQn_Type IRQn) | 	Sets the pending status of interrupt or exception to 1. |
| void NVIC_ClearPendingIRQ(IRQn_Type IRQn) | 	Clears the pending status of interrupt or exception to 0. | 
| uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) | Reads the pending status of interrupt or exception. This function returns non-zero value if the pending status is set to 1. | 
| void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) | Sets the priority of an interrupt or exception with configurable priority level to 1. |
| uint32_t NVIC_GetPriority(IRQn_Type IRQn) |  Reads the priority of an interrupt or exception with configurable priority level. This function return the current priority level. | 
| void 	NVIC_SystemReset (void)        | 	Reset the system. |



###Special functions

| CMSIS function                      | Instruction         | Description          |
|-------------------------------------|---------------|-----------------------|
| void __enable_irq(void) 	           | CPSIE I       |	 Change processor state - enable interrupts          |
| void __disable_irq(void) 	           |  CPSID I      |	 Change processor state - disable interrupts         |
| void __enable_fault_irq(void)	   |  CPSIE F      |	 Change processor state - enable fault interrupts    |
| void __disable_fault_irq(void)      |  CPSID F      |  Change processor state - disable fault interrupts   |
| void __ISB(void)	                   |  ISB          |  Instruction Synchronization Barrier                 |
| void __DSB(void)	                   |  DSB          |  Data Synchronization Barrier                        |
| void __DMB(void)	                   |  DMB          |  Data Memory Barrier                                 |
| uint32_t __REV(uint32_t int value)	|  REV          |  Reverse Byte Order in a Word                        |
| uint32_t __REV16(uint32_t int value)	|  REV16        |  Reverse Byte Order in each Half-Word                     |
| uint32_t __REVSH(uint32_t int value)	|  REVSH        |  Reverse byte order in bottom halfword and sign extend    |
| uint32_t __RBIT(uint32_t int value)	|  RBIT         |  Reverse bits                                        |
| void __SEV(void)	                    | SEV          |  Send Event                                          |
| void __WFE(void)	                    | WFE          |  Wait for Event                                      |
| void __WFI(void)	                    | W            |  Wait for Interrupt                                  |


###Functions to access processor register

|    CMSIS function                     |       Description           |
|---------------------------------------|-----------------------------|
| uint32_t __get_PRIMASK (void)         |    Read PRIMASK  |
| void   __set_PRIMASK (uint32_t value) |    Write PRIMASK |
| uint32_t __get_FAULTMASK (void)       | Read Write |
| void __set_FAULTMASK (uint32_t value) | Write Write |
| uint32_t __get_BASEPRI (void)         | Read BASEPRI |
| void __set_BASEPRI (uint32_t value)   | Write BASEPRI |
| uint32_t __get_CONTROL (void)         | Read CONTROL |
| void __set_CONTROL (uint32_t value)   | Write CONTROL |
| uint32_t __get_MSP (void)             | Read MSP |
| void __set_MSP (uint32_t TopOfMainStack)  | Write MSP |
| uint32_t __get_PSP (void)             | Read PSP |
| void __set_PSP (uint32_t TopOfProcStack)  | Write PSP |


**************************************************

#*1* First Blink

This is the 1th version of Blink. It uses direct access to register of the EFM32GG990F1024 microcontroller.

It is possible to use the Gecko SDK Library, which includes a HAL Library for GPIO. For didactic reasons and to avoid the restrictions imposed by the license, the direct access to registers is used in this document.

The architecture of the software is shown below.

    +------------------------------------------------------+
    |                     Application                      |
    +------------------------------------------------------+
    |                      Hardware                        |
    +------------------------------------------------------+
    

To access the registers, it is necessary to know their addresses and fields. These information can be found the data sheet and other documents from the manufacturer. The manufacturer (Silicon Labs) provides a CMSIS compatible header files in the *platform* folder of the Gecko SDK Library.

The *platform* folder has the following sub-folders of interest: *Device* and *CMSIS*. In the *Device/SiliconLabs/EFM32GG/Include/* folder there is a header file named *emf32gg990f1024.h*, which includes the definition of all registers of the microcontroller. One has to be careful because it includes a lot of other header files (*emf32gg_*.h*). It is possible to include the *emf32gg990f1024.h* file directly in the code, like below.

    #include <emf32gg990f1024.h>

But a better alternative is to use a generic include and define which microcontroller is used as a parameter in the command line (actually a definition of a preprocessor symbol).

    #include "em_device.h"

The command line must then include the *-DEMF32GG990F1024* parameter. To use this alternative one, has to copy the *em_device.h* to the project folder and used quote marks (“”) instead of angle brackets (<>) in the include line.

Instead of using symbols like *0x2* to access the bit to control the *LED1*, it is better to use a symbol LED1 as below.

    #define LED1 0x2

To define it, a common idiom is to use a BIT macro defined as below (the parenthesis are recommended to avoid surprises).
   
    #define BIT(N) (1<<(N))

The symbols to access the LEDs in the GPIO Port E registers can then be defined as

    #define LED1 BIT(2)
    #define LED2 BIT(3)

To use the GPIO Port, where the LEDs are attached, it is necessary to:

-   Enable clock for peripherals
-   Enable clock for GPIO
-   Configure pins as outputs
-   Set them to the desired values

To enable clock for peripherals, the *HFPERCLKEN* bit in the *HFPERCLKDIV* register must be set. To enable clock for the GPIO, the *GPIO* bit of the *HFPERCLKNE0* register must be set. Both of them are done by or'ing the mask already defined in the header files to the registers.

    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN; // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO; // Enable HFPERCKL for GPIO
    
To access the register for GPIO Port E, a constant is defined, that points to the corresponding memory address.

    GPIO_P_TypeDef * const GPIOE = &(GPIO->P[4]); // GPIOE

To configure the pins as outputs one has to set the mode fields in the MODE registers. There are two MODE registers: *MODEL* to configure pins 0 to 7 and *MODEH*, for pins 8 to 15. To drive the LEDs, the fields must be set to Push-Pull configuration, but just or a binary value is not enough. The field must be cleared (set to 0) before.

    /* Configure Pins in GPIOE */
    GPIOE->MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK|_GPIO_P_MODEL_MODE3_MASK); // Clear bits
    GPIOE->MODEL |= (GPIO_P_MODEL_MODE2_PUSHPULL|GPIO_P_MODEL_MODE3_PUSHPULL); // Set bits
    
Finally, to set the desired value, one can or a value with a bit 1 in the desired position and all other bits set to 0.

    GPIOE->DOUT |= LED1;

To clear it, one must AND a value with a bit 0 in the desired position and all other bit set to 1

    GPIOE->DOUT &= ~LED1;

To toggle a bin, one can XOR a value with a bit 1 in the desired position (and other bits set to 0).

    GPIOE->DOUT ^= LED1;

***************************************

*2* Blink again
------------------------------


This is the 2nd version of Blink. It uses direct access to register of the EFM32GG990F1024 microcontroller but with a simple Hardware Abstraction Layer (HAL) for the GPIO module.

The architecture is shown below.

    +---------------------------------------------------------------------+
    |                            Application                              |
    +---------------------------------------------------------------------+
    |                              GPIO HAL                               |
    +---------------------------------------------------------------------+
    |                              Hardware                               |
    +---------------------------------------------------------------------+


The HAL is implemented in the *gpio.c* and *gpio.h* files.

In the *gpio.h* files, a type *GPIO_t* is defined to be used to specify the GPIO port used.

To use the GPIO Port, it is necessary to:

-   Enable clock for peripherals
-   Enable clock for GPIO
-   Configure pins as outputs
-   Set them to the desired values

The *GPIO_Init* routine cares of the initialization. It uses the *GPIO_ConfigPins* routine to set the mode (input or output) for the specified pins. To specify the pin status, there are two alternatives: *GPIO_WritePins*, *GPIO_TogglePins*. To read (not implemented yet), there is *GPIO_ReadPins*.

So, it is possible to use the GPIO without detailed knowledge of the internal works. Another advantage is that this routines can be ported to other architectures, with different GPIO implementations.

###GPIO_Init

*GPIO_Init(gpio,inputs,outputs)* initialized the specified GPIO port *gpio*. The pins specified by *inputs* are configured to be input, and the pins specified by *output*, configured to Push-Pull Output mode. If a pin is specified in both, it is first configured as input. *inputs* and *outputs* are bit masks, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###GPIO_WritePins

*GPIO_WritePins(gpio,zeroes,ones)* set the pins of GPIO port *gpio*, specified by *zeroes* to zero, and the set the pins specified by *ones* to one. If a pin is specified in both, it is first cleared and then set. *zeroes* and *ones* are bit masks, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###GPIO_TogglePins

*GPIO_TogglePins(gpio,pins)* inverts the output pins of GPIO port *gpio*. The *pins* parameter is a bit mask, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###GPIO_ConfigPins

*GPIO_ConfigPins(gpio,pins,mode)* is used to configure the pins of the GPIO port specified by the *gpio* parameter to the desired *mode*. The pins to be configured are specified in the bit mask *pins* parameter, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

The parameter mode can be one of the symbols listed in the table below.

	GPIO_MODE_DISABLE
    GPIO_MODE_INPUT
    GPIO_MODE_INPUTPULL
    GPIO_MODE_INPUTPULLFILTER
    GPIO_MODE_PUSHPULL
    GPIO_MODE_PUSHPULLDRIVE
    GPIO_MODE_WIREDOR
    GPIO_MODE_WIREDORPULLDOWN
    GPIO_MODE_WIREDAND
    GPIO_MODE_WIREDANDFILTER
    GPIO_MODE_WIREDANDPULLUP
    GPIO_MODE_WIREDANDPULLUPFILTER
    GPIO_MODE_WIREDANDDRIVE
    GPIO_MODE_WIREDANDDRIVEFILTER
    GPIO_MODE_WIREDANDDRIVEPULLUP
    GPIO_MODE_WIREDANDDRIVEPULLUPFILTER


###Acessing LEDs

The symbols to access the LEDs in the GPIO Port E registers can be defined as

    #define LED1 BIT(2)
    #define LED2 BIT(3)
    
To configure the LED pins, the instruction below can do the job.

    GPIO_Init(GPIOE,0,LED1|LED2);

To turn on the LED1, just code
    
    GPIO_WritePins(GPIOE,0,LED1);

To turn off the LED1, just

    GPIO_WritePins(GPIOE,LED1,0);

To toggle the LED1, just

    GPIO_TogglePines(GPIOE,LED1);

In all cases above, it is possible to modify more than on LED. For example, to clear both LEDs,

    GPIO_WritePins(GPIOE,LED1|LED2,0);

**************************************************

#*3* Blink revisited

This is the 3rd version of Blink. It uses direct access to registers of the EFM32GG990F1024 microcontroller but with a simple Hardware Abstraction Layer (HAL) for the LED module. This HAL is built upon the GPIO HAL (See last example).
The main objective of this HAL is to permit the control the LEDs without knowing anything about GPIO.

The architecture is shown below.

    +---------------------------------------------------------------------+
    |                            Application                              |
    +---------------------------------------------------------------------+
    |                               LED HAL                               |
    +---------------------------------------------------------------------
    |                              GPIO HAL                               |
    +---------------------------------------------------------------------+
    |                              Hardware                               |
    +---------------------------------------------------------------------+

The LED HAL is implemented in the *led.c* and *led.h* files. The GPIO HAL is implemented in *gpio.c* and *gpio.h* as before.

The main functions for a LED are:

-   Initialize (Configure processor, gpio, etc)
-   Turn it on
-   Turn if off
-   Toggle it

The symbols to access the LEDs are defined as bit masks to easy the access the corresponding pins of the GPIO Port E.

    #define LED1 BIT(2)
    #define LED2 BIT(3)

###LED_Init

*LED_Init(leds)* initializes the GPIO for the leds specified by *leds*. *leds* is a bit mask, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###LED_Write

*LED_Write(off,on)* turns off the LEDs specified by *off*, and then turns on the LEDs specified by *on*. If a LED is specified in both, it is first cleared and then set.

###LED_Toggle

*LED_Toggle(leds)* inverts the status of the LEDs specified by *leds*.

###LED_On

*LED_On(leds)* turns on the LEDs specified by *leds*.

###LED_Off

*LED_Off(leds)* turns off the LEDs specified by *leds*.

###Acessing LEDs

To configure the LED pins, the instruction below can do the job.

    LED_Init(LED1|LED2);

To turn on the LED1, just code

    LED_Write(0,LED1);

To turn off the LED1, just

    LED_Write(LED1,0);

To toggle the LED1, just

    LED_Toggle(LED1);

In all cases above, it is possible to modify more than on LED. For example, to clear both LEDs,

    LED_Write(LED1|LED2,0);

**************************************************

#*4* Another Blink


This is the 4th version of Blink. It uses direct access to register of the EFM32GG990F1024 microcontroller but with a simple Hardware Abstraction Layer (HAL) for the LED module. This HAL is built directly upon GPIO, without using a GPIO HAL (See version 2 of Blink).

The main objective is to control the LEDs without knowing about GPIO and little overhead. The architecture is shown below.

    +---------------------------------------------------------------------+
    |                            Application                              |
    +---------------------------------------------------------------------+
    |                               LED HAL                               |
    +---------------------------------------------------------------------+
    |                              Hardware                               |
    +---------------------------------------------------------------------+


The LED HAL is implemented in the *led.c* and *led.h* files.

The main functions for a LED are:

-   Initialize (Configure processor, gpio, etc)
-   Turn it on
-   Turn if off
-   Toggle it

The symbols to access the LEDs are defined as bit masks to easy the access the corresponding pins of the GPIO Port E.

    #define LED1 BIT(2)
    #define LED2 BIT(3)

###LED_Init

*LED_Init(leds)* initializes the GPIO for the LEDs specified by *leds*. *leds* is a bit mask, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###LED_Write

*LED_Write(off,on)* turns off the LEDs specified by *off*, and then turns on the LEDs specified by *on*. If a LED is specified in both, it is first cleared and then set.

###LED_Toggle

*LED_Toggle(leds)* inverts the status of the LEDs specified by *leds*.

###LED_On

*LED_On(leds)* turns on the LEDs specified by *leds*.

###LED_Off

*LED_Off(leds)* turns off the LEDs specified by *leds*.

###Acessing LEDs

To configure the LED pins, the instruction below can do the job.

    LED_Init(LED1|LED2);

To turn on the LED1, just code

    LED_Write(0,LED1);

To turn off the LED1, just

    LED_Write(LED1,0);

To toggle the LED1, just

    LED_Toggle(LED1);
