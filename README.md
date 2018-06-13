
EFM32GG-STK3700 Development Board
=================================

The EFM32 Giant Gecko is a family of Cortex M3 microcontrollers manufactured by Silicon Labs (who bought Energy Micro, the initial manufacturer).

The EMF32GG-STK3700 is a development board featuring a EFM32GG990F1024 MCU with 1 MB Flash memory and 128 kB RAM. It has also the following peripherals:

* 160 segment LCD
* 2 user buttons, 2 user LEDs and a touch slider
* Ambient Light Sensor and inductive-capacitive metal sensor
* EFM32 OPAMP footprint
* 32 MB NAND flash
* USB interface for Host/Device/OTG

It has a 20 pin expansion header, breakout pads for easy access to I/O pins, different alternatives for power sources including USB and a 0.03 F Super Capacitor for backup power domain.

For developing, there is an Integrated Segger J-Link USB debugger/emulator with debug out functionality and an Advanced Energy Monitoring system for precise current tracking.


#Example Programs for the EFM32GG-STK3700 Development Board


In all examples, a direct access to registers approach was used. It means that no library besides CMSIS was used.

* 01-Blink:             Blink LEDs with direct access to registers
* 02-Blink:             Blink LEDs with a HAL for GPIO
* 03-Blink:             Blink LEDs with a HAL for LEDs above GPIO HAL
* 04-Blink:             Blink LEDs with a HAL for LEDs without a HAL for GPIO 
* 05-Blink:             Blink LEDs changing the core frequency
* 06-Blink:             Blink LEDs using SysTick
* 07-Blink\*:             Blink LEDs using Time Triggered Tasks
* 08-Button\*:            Control LEDs using buttons (without debounce)
* 09-Debounce\*:          Control LEDs using buttons (without debounce)
* 10-UART:              Uses 
* 11-UART\*: 
* 12-Ministdio:         Implements a mini stdio package (printf, gets, etc).
* 13-Newlib:            Implements full access to newlib
* 14-TimeTriggered:     Uses a Time-Triggered approach (Pont. Patterns for time-triggered embedded systems 2001)
* 15-Protothreads\*:      Uses a proto thread approach
* 16-FreeRTOS\*:          Uses a (free) preemptive real time kernel
* 17-ucos2\*:             Uses ucos version 2
* 18-ucos3\*:             Uses ucos version 3
* 
>**Those marked with an asterisk are unfinished!!!**

# Access to registers

The peripherals and many funcionalities of the CPU are controlled accessing and modifying registers.

Silicon Labs as others manufactures provides a set of header with symbols that can be used to access the registers. Following CMSIS standard, the registers are grouped in C struct. 

As an example, the struct to access the register for an UART is defined as below.

    typedef struct
    {
      __IOM uint32_t CTRL;       /**< Control Register  */
      __IOM uint32_t FRAME;      /**< USART Frame Format Register  */
      __IOM uint32_t TRIGCTRL;   /**< USART Trigger Control register  */
      __IOM uint32_t CMD;        /**< Command Register  */
      __IM uint32_t  STATUS;     /**< USART Status Register  */
      __IOM uint32_t CLKDIV;     /**< Clock Control Register  */
      __IM uint32_t  RXDATAX;    /**< RX Buffer Data Extended Register  */
      __IM uint32_t  RXDATA;     /**< RX Buffer Data Register  */
      __IM uint32_t  RXDOUBLEX;  /**< RX Buffer Double Data Extended Register  */
      __IM uint32_t  RXDOUBLE;   /**< RX FIFO Double Data Register  */
      __IM uint32_t  RXDATAXP;   /**< RX Buffer Data Extended Peek Register  */
      __IM uint32_t  RXDOUBLEXP; /**< RX Buffer Double Data Extended Peek Register  */
      __IOM uint32_t TXDATAX;    /**< TX Buffer Data Extended Register  */
      __IOM uint32_t TXDATA;     /**< TX Buffer Data Register  */
      __IOM uint32_t TXDOUBLEX;  /**< TX Buffer Double Data Extended Register  */
      __IOM uint32_t TXDOUBLE;   /**< TX Buffer Double Data Register  */
      __IM uint32_t  IF;         /**< Interrupt Flag Register  */
      __IOM uint32_t IFS;        /**< Interrupt Flag Set Register  */
      __IOM uint32_t IFC;        /**< Interrupt Flag Clear Register  */
      __IOM uint32_t IEN;        /**< Interrupt Enable Register  */
      __IOM uint32_t IRCTRL;     /**< IrDA Control Register  */
      __IOM uint32_t ROUTE;      /**< I/O Routing Register  */
      __IOM uint32_t INPUT;      /**< USART Input Register  */
      __IOM uint32_t I2SCTRL;    /**< I2S Control Register  */
    } USART_TypeDef;             /** @} */
      
The base addresses of the USART units are defined as

    #define USART0_BASE       (0x4000C000UL) /**< USART0 base address  */
    #define USART1_BASE       (0x4000C400UL) /**< USART1 base address  */
    #define USART2_BASE       (0x4000C800UL) /**< USART2 base address  */

And the symbols to access the units as 
    
    #define USART0       ((USART_TypeDef *) USART0_BASE)        /**< USART0 base pointer */
    #define USART1       ((USART_TypeDef *) USART1_BASE)        /**< USART1 base pointer */
    #define USART2       ((USART_TypeDef *) USART2_BASE)        /**< USART2 base pointer */
    
To read a register, just write

    uint32_t w = USART0->CTRL;          // Read
    ...
    USART0->CTRL = w;                   // Write
    
Each register has many fields. Some fields are just 1 bit long and there are some many bits long. The procedure to modify them are different. One bit fields can be modified with one operation. For example, to set the SYNC bit at position 0, use one of these forms (the first is more used). A or operator must be used (Never use addition represented by a *+* because if the bit is already set, it gives wrong results).

    USART0->CTRL |= 1;
    USART0->CTRL = USART0->CTRL | 1;    

To clear it, one of these forms (Do not use minus represented by a *-* )

    USART0->CTRL &= ~1;
    USART0->CTRL &= 0xFFFFFFFEU;
    USART0->CTRL = USART0->CTRL & ~1;
    USART0->CTRL = USART0->CTRL & 0xFFFFFFFEU;
    

The legibility can be enhanced using a macro BIT defined as

    USART0->CTRL |= BIT(0);                 // Set bit
    USART0->CTRL &= ~BIT(0);                // Clear bit

But much better is the use of symbols defined in the header for the field.

    USART0->CTRL |= USART_CTRL_SYNC;        // Set bit
    USART0->CTRL &= ~USART_CTRL_SYNC;       // Clear bit
    
In case of field with more than one bit, it must be assured that the field is all zero, before making a or operation. To set the OVS field, that has 2 bits, it must be cleared first.

    uint32_t w = USART0->CTRL;
    w &= ~0x60;
    w |= 0x40;
    USART0->CTRL = w;
    
Or in just one line

    USART0->CTRL = (USART0->CTRL&~0x60)|0x40;

Or using shift operators

    USART0->CTRL = (USART0->CTRL&~(3<<5)|(2<<5);

Again,a better way is to use symbols defined in the header.

    USART0->CTRL = USART0->CTRL&~_USART_CTRL_OVS_MASK)|USART_CTRL_OVS_X8;
    
Attention with the underscore before the symbol for the mask. Generally symbols started by underscore are reserved for implementation and should be not used in production code. But the symbols ending in MASK, and in a minor scale, in SHIFT symbols are useful and very used. It is not necessary to use the other symbols, whose names start with underscore. Actually, it is very dangerous because their values are not in the correct position. The must be shifted to the correct position. This can be seen in the header,

    #define _USART_CTRL_OVS_SHIFT         5                                        /**< Shift value for USART_OVS */
    #define _USART_CTRL_OVS_MASK          0x60UL                                   /**< Bit mask for USART_OVS */
    #define _USART_CTRL_OVS_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for USART_CTRL */
    #define _USART_CTRL_OVS_X16           0x00000000UL                             /**< Mode X16 for USART_CTRL */
    #define _USART_CTRL_OVS_X8            0x00000001UL                             /**< Mode X8 for USART_CTRL */
    #define _USART_CTRL_OVS_X6            0x00000002UL                             /**< Mode X6 for USART_CTRL */
    #define _USART_CTRL_OVS_X4            0x00000003UL                             /**< Mode X4 for USART_CTRL */
    #define USART_CTRL_OVS_DEFAULT        (_USART_CTRL_OVS_DEFAULT << 5)           /**< Shifted mode DEFAULT for USART_CTRL */
    #define USART_CTRL_OVS_X16            (_USART_CTRL_OVS_X16 << 5)               /**< Shifted mode X16 for USART_CTRL */
    #define USART_CTRL_OVS_X8             (_USART_CTRL_OVS_X8 << 5)                /**< Shifted mode X8 for USART_CTRL */
    #define USART_CTRL_OVS_X6             (_USART_CTRL_OVS_X6 << 5)                /**< Shifted mode X6 for USART_CTRL */
    #define USART_CTRL_OVS_X4             (_USART_CTRL_OVS_X4 << 5)                /**< Shifted mode X4 for USART_CTRL */
    

When setting a field with an user calculated value, it is a good idea to ensure that no other bit in the register undecteted is modified. For example, there is a 3-bit field called HFCLKDIV, that specifies the divisor of the crystal frequency. TO set it to div, the following instruction can be used.

    CMU->CTRL = (CMU->CTRL&~_CMU_CTRL_HFCLKDIV_MASK)|(div<<_CMU_CTRL_HFCLKDIV_SHIFT)&_CMU_CTRL_HFCLKDIV_MASK));

Or using a multi step approach.

    uint32_t w = CMU->CTRL;                         // Get present value
    w &= ~_CMU_CTRL_HFCLKDIV_MASK;                  // Set field HFCLKDIV to 0
    uint32_t n = div<<_CMU_CTRL_HFCLKDIV_SHIFT;     // Set field with new value
    n &= _CMU_CTRL_HFCLKDIV_MASK;                   // Ensure it does not extrapolate
    w |= n;                                         // Calculate new value
    CMU->CTRL = w;                                  // Set new value

# CMSIS


## NVIC functions

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



## Special functions 
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


## Register access


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



