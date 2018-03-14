
Access to registers
==================

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
    


