Developing for the EFM32GG-STK3700 Development Board
====================================================

See LICENSE.md

Foreword
--------

This material is intended as a companion for the lectures in a course in Embedded Systems. The main objective is to present the inner works of the compilation process and introduce advanced patterns of software for embedded systems.

It assumes a basic knowledge of C. It consists of many small projects, each introducing one, and sometimes two, aspects of the development process.

Appendices show how to install the needed tools, how to use them and the structure of a project.

The EFM32GG-STK3700 Development Board
-------------------------------------


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


The EMF32GG-STK3700 Development Board
-------------------------------------


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


Basic References
----------------


The most important references are:

-   [EFM32GG Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)[1]: Manual describing all peripherals, memory map.
-   [EFM32GG-STK3700 Giant Gecko Starter Kit User's Guide](https://www.silabs.com/documents/public/user-guides/efm32gg-stk3700-ug.pdf)[2]: Information about the STK3700 Board.
-   [EFM32GG990 Datasheet](https://www.silabs.com/documents/public/data-sheets/EFM32GG990.pdf)[3]: Technical information about the EMF32GG990F1024 including electrical specifications and pinout.
-   [EFM32 Microcontroller Family Cortex M3 Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32-Cortex-M3-RM.pdf)[4]

Peripherals
-----------

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

Connections
-----------


The EMF32GG-STK3700 board has two USB connectors: One, a Mini USB B Connector, for development and other, a Micro B-Type USB Connector, for the application.

For development, a cable (delivered) must be connected between the Mini USB connector on the board and the A-Type connector on the PC. Among the devices listed by the *lsusb* command, the following must appear.

    Bus 001 Device 019: ID 1366:0101 SEGGER J-Link PLUS

The STK3700 board has two microcontrollers: one, called target, is a EFM32GG990F1024 microcontroller, and the other, called Board Controller, implements an interface for programming and debugging.

For a Cortex M microcontroller the following programming interfaces are used:

-   SWD : 2 pinos: SWCLK, SWDIO - Serial Wire Debug.
-   JTAG : 4 pinos: TCK, TMS, TDI, TDO - Not used in this board

In the STK3700 only the SWD interface is used, and there is a connector on the SWD lines which permits the debugging of off-board microcontrollers.

Generally, in this kind of boards there is a serial interface between the Target and the Board Controller. It can be implemented using a physical channel with 2 lines or a virtual, using the SWD/JTAG channel. Both appears to the Host PC as a serial virtual port (*COMx* or */dev/ttyACMx*). In the STK3700 board the serial channel uses the UART0 unit (pins PE0 and PE1).

Examples for the EFM32GG-STK3700 Development Board
--------------------------------------------------

In all examples, a direct access to registers approach was used. It means that no library besides CMSIS was used.

-    01-Blink: Blick LEDs with direct access to registers
*    01-Blink-NO: How to not do it
-    02-Blink: Blick LEDs with a HAL for GPIO
-    03-Blink: Blink LEDs with a LED HAL above the GPIO HAL
-    04-Blink: Blink LEDs with a LED HAL above hardware
-    05-SysTick: Using SysTick to implement delays
-    06-ClockFrequency: Changing the core clock frequency
-    07-StateMachine: Processing inside the SysTick Interrupt
-    08-Timers: Timers
-    09-Button: Buttons
-    10-Button: More about buttons
-    11-Debounce: Debouncing
-    12-UART: Serial Communication using polling
-    13-UART: Serial Communication using interrupts
-    14-Ministdio: Mini Standard I/O Package
-    15-Newlib: Using Newlib
-    16-TimeTriggered: Time Triggered Systems
-    17-Protothreads: Using Protothreads
-    18-FreeRTOS: Using FreeRTOS
-    19-ucos2: Using uC/OS-II
-    20-ucos3: Using uC/OS-III
-    21-LCD: Using the LCD display
-    22-Better newlib support
-    23-Temperature: Getting the CPU temperature
-    24-Quadrature: a software based decoding
-   \*25-Quadrature-v2: a hardware based decoding.

OBS:

-    The hardware decoding in 25-Quadrature-v2 is not working well. Probably
     the encoder is too noisy. I will try with a better one.

**Those marked with an asterisk are unfinished!!!**
