Developing for the EFM32GG-STK3700 Development Board
====================================================

See LICENSE.md

Foreword
--------

This material is intended as a companion for the lectures in a course in Embedded Systems. The main objective is to present the inner works of the compilation process and introduce advanced patterns of software for embedded systems.

It assumes a basic knowledge of C. It consists of many small projects, each introducing one, and sometimes two, aspects of the development process.

A [companion text](Developing-for-the-EFM32GG-STK3700-using-gcc-and-makefiles.pdf) describes the projects and the source code.
[Another text](Installing-Tools.md) shows how to install the needed tools, how to use them and the structure of a project.

The EFM32GG-STK3700 Development Board
-------------------------------------


The EFM32 Giant Gecko is a family of Cortex M3 microcontrollers manufactured by Silicon Labs (who bought Energy Micro, the initial manufacturer). The EFM32 microcontroller family has many subfamilies with different Cortex-M architectures and features as shown below.


Family         | Core              | Features | Flash (kB) | RAM (kB)   |
---------------|-------------------|----------|------------|------------|
Zero Gecko     |   ARM Cortex-M0+  |          |   4- 32    | 2-4        |
Happy Gecko    |   ARM Cortex-M0+  | USB      |  32- 64    | 4-8        |
Tiny Gecko     |   ARM Cortex-M3   | LCD      |   4- 32    | 2-4        |
Gecko          |   ARM Cortex-M3   | LCD      |  16- 128   | 8-16       |
Jade Gecko     |   ARM Cortex-M3   |          | 128-1024   | 32-256     |
Leopard Gecko  |   ARM Cortex-M3   | USB, LCD |  64- 256   | 32         |
Giant Gecko    |   ARM Cortex-M3   | USB. LCD | 512-1024   | 128        |
Giant Gecko S1 |   ARM Cortex-M4   | USB, LCD | 2048       | 512        |
Pearl Gecko    |   ARM Cortex-M4   |          | 128-1024   | 32-256     |
Wonder Gecko   |   ARM Cortex-M4   | USB, LCD |  64- 256   | 32         |


The EMF32GGF1024 Microcontroller
--------------------------------

The microcontroller used in the STK3700 board is the EMF32GGF1024 microcontroller.
Its main features are:

| Device | Description                    |
|--------|--------------------------------|
| Flash	 | 1024	KBytes                    |
| RAM	 | 128 KBytes                     |
| Clock  | 48 MHz max.                    |
| VDD    | 1.98-3.8 V                     |
| Power  | 10 mA @ 48 MHz, 219 uA @ 1 MHz |
| Package| BGA112                         |
| GPIO	 | 86 pins                        |
| USB	 | HS/FS Host Device or OTG	      |
| DMA    | 12 channels                    |
| EBI    | External bus interface 4x256 MB|
| LCD    | up to 8 commons and 34 segments|
| USART  | 3                              |
| UART	 | 2                              |
| LEUART | 2                              |
| I2C    | 2 (10 Kbps-1 Mpbs)             |
| Timer  | 4 x 16-bit timer, 4x3 PWM      |
| LETIMER| 1 x 16-bit timer               |
| ADC	 | 8 x 12-bit, 500 Ksps           |
| DAC	 | 1 x 12-bit, 500 ksps           |
| OpAmp	 | 3                              |
| Comp   | 2 x 8 inputs capacitive sensing|



The EMF32GG-STK3700 Development Board
-------------------------------------

The EMF32GG-STK3700 is a development board featuring a EFM32GG990F1024 MCU (a Giant Gecko microcontroller) with 1 MB Flash memory and 128 kB RAM. It has also the following peripherals:

*   160 segment LCD
*   2 user buttons
*   2 user LEDs
*   1 Touch slider
*   Ambient Light Sensor and inductive-capacitive metal sensor
*   EFM32 OPAMP footprint
*   32 MB NAND flash
*   USB interface for Host/Device/OTG

It has a 20 pin expansion header, breakout pads for easy access to I/O pins, different alternatives for power sources including USB and a 0.03 F Super Capacitor for backup power domain.

For developing, there is an Integrated Segger J-Link USB debugger/emulator with debug out functionality and an Advanced Energy Monitoring system for precise current tracking.


Basic References
----------------


The most important references are:

-   [EFM32GG Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)[1]: Manual describing all peripherals, memory map.
-   [EFM32GG-STK3700 Giant Gecko Starter Kit User's Guide](https://www.silabs.com/documents/public/user-guides/efm32gg-stk3700-ug.pdf)[2]: Information about the STK3700 Board.
-   [EFM32GG990 Datasheet](https://www.silabs.com/documents/public/data-sheets/EFM32GG990.pdf): Technical information about the EMF32GG990F1024 including electrical specifications and pinout.
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

-    01-Blink-Very-Simple: Blink LEDs with direct access to registers
-    02-Blink-Simple-GPIO-HAL-1: Blink LEDs with simple GPIO
-    02-Blink-Simple-GPIO-HAL-2: Blink LEDs with a GPIO module
-    03-Blink-Layered-LED-HAL: Blink LEDs with a LED module over a GPIO module
-    04-Blink-LED-HAL: Blink LEDs with a LED module with direct access to registers
-    05-Using-SysTick: Blink LEDs using SysTick timer
-    06-Changing-Clock-Frequency: Blink LEDs when changing CPU frequency
-    07-Using-a-State-Machine: Blink LEDs using a state machine
-    08-Software-Timers: Blink LEDs using (software) timers
-    09-Button-Polling: Polling buttons
-    10-Button-Interrupt: Buttons generate interrupt
-    11-Implementing-Debounce: Debouncing buttons
-    12-UART-Polling: Simple UART interface using polling
-    13-UART-Interrupt: Advanced UART interface using interrupts and buffers
-    \*\*X14-UART-DMA: UART using DMA
-    15-Mini-stdio: A simple and small module with some stdio routines
-    16-Using-Newlib: Using the newlib (part of ARM GNU C Compiler)
-    17-Using-a-Time-Triggered-Approach: Using Pont [PTTES](https://www.safetty.net/publications/pttes) approach
-    18-Using-a-Better-Time-Triggered-Approach: Using Pont [ERES](https://www.safetty.net/publications/the-engineering-of-reliable-embedded-systems-second-edition) approach
-    19-Using-Protothreads: Using [Dunkels](http://dunkels.com/adam/pt/) protothreads
-    20-Using-FreeRTOS: Using [FreeRTOS](https://www.freertos.org/)
-    21-Using-FreeRTOS-with-Interrupts: Using [FreeRTOS](https://www.freertos.org/) and interrupts
-    22-Using-uC-OS2: Using [uc/OS2](https://github.com/weston-embedded/uC-OS2)
-    23-Using-uC-OS2-with-Interrupts:
-    24-Using-uC-OS3:
-    25-Using-uC-OS3-with-Interrupts:
-    26-Using-the-LCD: Using the onboard LCD (with a simple module)
-    27-Implementing-a-better-Newlib: More advanced implementation of newlib interface
-    28-Getting-Temperature: Showing the temperature from internal sensor
-    29-Software-based-Quadrature-Decoding: Reading the encoder with a state machine
-    30-Hardware-based-Quadrature-Decoding: Reading the encoder using HW timer
-    31-Using-the-Slider-Control: Using the on board slider
-    32-Generating-PWM-Signal: Generating PWM signals using HW timer
-    \*\*X33-I2CMaster-Polling
-    \*\*X34-I2CMaster-Interrupt
-    \*\*X41-USB-Device-Barebone
-    \*\*X42-USB-Device-TinyUSB: USB interface using a middleware
-    \*\*X43-NAND-Flash

Notes:

1.   The hardware decoding in 29-Hardware-based-Quadrature-Decoding is
     not working well. Probably the encoder is too noisy. I will try with
     a better one.
2.    Projects marked with an asterisk are unfinished!!!
3.    Projects marked with a double asterisk are in a very initial stage
