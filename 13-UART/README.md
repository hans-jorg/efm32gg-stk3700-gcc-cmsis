13  Serial Communication using interrupts
=========================================


##Interrupt based serial communication

This describes an interrupt based implementation of serial communication. The device used for serial communication using USB is the UART0 (not USART0). It uses the PE0 and PE1 pins for RX and TX, respectively, and the PF7 to enable a transceiver.

The EMF32GG990F1024 has two interrupts for the UART0: *UART0_TX_IRQn* and *UART0_RX_IRQn*. The generated interrupts calls the routines *UART0_RX_IRQHandler* and *UART0_TX_IRQHandler*.

Both use a FIFO buffer to store the characters handled. This avoid to need to wait. When the buffer is full, the extra characters are discarded. The FIFO buffer is defined in buffer.h and buffer.c.

##Enabling interrupts

	// Enable interrupts on UART
    UART0->IFC = (uint32_t) -1;
    UART0->IEN |= UART_IEN_TXC|UART_IEN_RXDATAV;

    // Enable interrupts on NVIC
    NVIC_SetPriority(UART0_RX_IRQn,RXINTLEVEL);
    NVIC_SetPriority(UART0_TX_IRQn,TXINTLEVEL);
    NVIC_ClearPendingIRQ(UART0_RX_IRQn);
    NVIC_ClearPendingIRQ(UART0_TX_IRQn);
    NVIC_EnableIRQ(UART0_RX_IRQn);
    NVIC_EnableIRQ(UART0_TX_IRQn);

    // Disable and then enable RX and TX
    UART0->CMD  = UART_CMD_TXDIS|UART_CMD_RXDIS;
    UART0->CMD  = UART_CMD_TXEN|UART_CMD_RXEN;


##Hazard conditions

When using interrupt, it is possible to access data while it is being modified. This can lead to data corruption and unexpected behaviors. In this case, the access to buffer must be done without interrupts by using the *ENTER_CRITICAL* and *EXIT_CRITICAL* macros pairs. They are just *__disable_irq()* and *__enable_irq()*, respectively.

##Transmitting a char

The interrupt routine is called when there is a modification in *TXC* flag in *STATUS* register and enable by setting the *TXC* bit in *IEN*.

    void UART0_TX_IRQHandler(void) {
    uint8_t ch;

        // if data in output buffer and transmitter idle, send it
        if( UART0->IF&UART_IF_TXC ) {
            if( UART0->STATUS&UART_STATUS_TXBL ) {
                if( !buffer_empty(outputbuffer) ) {
                    // Get from output buffer
                    ch = buffer_remove(outputbuffer);
                    UART0->TXDATA = ch;
                }
           }
           UART0->IFC = UART_IFC_TXC;
        }
    }

To transmit, just put the data in the buffer and if it was empty, raise an interrupt.

    void UART_SendChar(char ch) {
        if ( buffer_empty(outputbuffer) ) {
            while ( (UART0->STATUS&UART_STATUS_TXBL) == 0 ) {}
            UART0->TXDATA = ch;
        } else {
            ENTER_ATOMIC();
            buffer_insert(outputbuffer,ch);
            EXIT_ATOMIC();
        }
    }

##Receiving a char

The interrupt routine is straightforward.

    void UART0_RX_IRQHandler(void) {
    uint8_t ch;

    if( UART0->STATUS&UART_STATUS_RXDATAV ) {
        // Put in input buffer
        ch = UART0->RXDATA;
        (void) buffer_insert(inputbuffer,ch);
        }
    }

The API for receiving a char includes a non block *UART_GetCharNoWait* to get the char of buffer if there is one there. Otherwise returns 0.

    unsigned UART_GetCharNoWait(void) {
    int ch;

        if( buffer_empty(inputbuffer) )
            return 0;

        ENTER_ATOMIC();
        ch = buffer_remove(inputbuffer);
        EXIT_ATOMIC();
        return ch;
    }

##Notes

1.  Before developing application that uses the serial-USB bridge, it is necessary to update the firmware in the board controller.
2.  The interface appears as a */dev/ttyACMx* in Linux machines and *COMx* in Window machines.

##More information

1.  [EFM32 STK Virtual COM port](https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/2015/07/06/efm32_stk_virtualco-aT2m)[17]
2.  [Using stdio on Silicon Labs platforms](https://os.mbed.com/teams/SiliconLabs/wiki/Using-stdio-on-Silicon-Labs-platforms)[18]
3.  [AN0045](http://www.silabs.com/Support%20Documents/TechnicalDocs/AN0045.pdf)[19]
