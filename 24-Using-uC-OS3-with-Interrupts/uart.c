/** **************************************************************************
 * @file    uart.c
 * @brief   LED HAL for EFM32GG STK
 * @version 1.0
 *
 * @note    Configure UART0 to work at 115200 bps, 8 bit, no parity, 1 stop bit
 *          no parity handshake.
 *
 *          With this configuration, it can communicate with the board controller,
 *          which relay data in and from a Virtual Communication Port created
 *          as a CDC inside the USB connection.
 *
 *          In Windows, it appears as COMx. In Linux, as /dev/ttyACMx.
 *
 *
 *****************************************************************************/

#include <stdint.h>
/*
 * Including this file, it is possible to define which processor using command line
 * E.g. -DEFM32GG995F1024
 * The alternative is to include the processor specific file directly
 * #include "efm32gg995f1024.h"
 */

#include "em_device.h"
#include "clock_efm32gg_ext.h"
#include "uart.h"
#include "buffer.h"

/**
 * @brief   Macros to enhance portability
 */
#define BIT(N) (1U<<(N))
#define ENTER_ATOMIC() __disable_irq()
#define EXIT_ATOMIC()  __enable_irq()

/**
 * @brief   Configuration
 */
/// Buffer size for input and output
#define INPUTBUFFERSIZE 100
#define OUTPUTBUFFERSIZE 100
/// Interrupt level
#define RXINTLEVEL 6
#define TXINTLEVEL 6

/// baudrate
const uint32_t BAUD = 115200;
const uint32_t OVERSAMPLING = 16;

/// GPIO Port used for RX/TX
static GPIO_P_TypeDef * const GPIOE = &(GPIO->P[4]);    // GPIOE
/// GPIO Port used for enable transceiver
static GPIO_P_TypeDef * const GPIOF = &(GPIO->P[5]);    // GPIOF



/**
 * @brief   Global variables
 *
 * @note    To avoid use of malloc, it uses a macro to define area
 */

DECLARE_BUFFER_AREA(inputbufferarea,INPUTBUFFERSIZE);
DECLARE_BUFFER_AREA(outputbufferarea,OUTPUTBUFFERSIZE);
buffer inputbuffer = 0;
buffer outputbuffer = 0;


/**
 * @brief   Resets UART
 */

void UART_Reset(void) {

    /* Make sure disabled first, before resetting other registers */
    UART0->CMD = UART_CMD_RXDIS | UART_CMD_TXDIS | UART_CMD_MASTERDIS
                | UART_CMD_RXBLOCKDIS | UART_CMD_TXTRIDIS | UART_CMD_CLEARTX
                | UART_CMD_CLEARRX;
    UART0->CTRL      = _UART_CTRL_RESETVALUE;
    UART0->FRAME     = _UART_FRAME_RESETVALUE;
    UART0->TRIGCTRL  = _UART_TRIGCTRL_RESETVALUE;
    UART0->CLKDIV    = _UART_CLKDIV_RESETVALUE;
    UART0->IEN       = _UART_IEN_RESETVALUE;
    UART0->IFC       = _UART_IFC_MASK;
    UART0->ROUTE     = _UART_ROUTE_RESETVALUE;
    UART0->IRCTRL    = _UART_IRCTRL_RESETVALUE;
    UART0->INPUT     = _UART_INPUT_RESETVALUE;

    buffer_deinit(inputbuffer);
    buffer_deinit(outputbuffer);

}


/**
 * @brief   Initializes UART
 *
 * @note    Does not enable interrupts!!!!
 */

void UART_Init(void) {
uint32_t bauddiv;

    /* Enable Clock for GPIO and UART */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCLK for GPIO


    // Configure PE0 (TX)
    GPIOE->MODEL &= ~_GPIO_P_MODEL_MODE0_MASK;          // Clear field
    GPIOE->MODEL |= GPIO_P_MODEL_MODE0_PUSHPULL;        // Set field
    GPIOE->DOUT  |= BIT(0);

    // Configure PE1 (RX)
    GPIOE->MODEL &= ~_GPIO_P_MODEL_MODE1_MASK;          // Clear field
    GPIOE->MODEL |= GPIO_P_MODEL_MODE1_INPUT;           // Set field
    GPIOE->DOUT  |= BIT(1);

    // Enable clock for UART0
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_UART0;          // Enable HFPERCLK for UART0

    /* Reset UART */
    UART_Reset();

    // 8 bits, no parity, 1 stop bit
    UART0->FRAME &= ~( _UART_FRAME_STOPBITS_MASK
                      |_UART_FRAME_PARITY_MASK
                      |_UART_FRAME_DATABITS_MASK );                 // Clear field

    UART0->FRAME |=   UART_FRAME_STOPBITS_ONE
                    | UART_FRAME_PARITY_NONE
                    | UART_FRAME_DATABITS_EIGHT;                    // Set field

    // Asynchronous with 16x oversampling
    UART0->CTRL  = _UART_CTRL_RESETVALUE|UART_CTRL_OVS_X16;         // Set field

    // Baud rate
    bauddiv = (ClockGetPeripheralClockFrequency()*4)/(OVERSAMPLING*BAUD)-4;
    UART0->CLKDIV = bauddiv<<_UART_CLKDIV_DIV_SHIFT;


    // Configure PF7 (Enable Transceiver)
    GPIOF->MODEL &= ~_GPIO_P_MODEL_MODE7_MASK;          // Clear field
    GPIOF->MODEL |= GPIO_P_MODEL_MODE7_PUSHPULL;        // Set field
    GPIOF->DOUT  |= BIT(7);

    // Set which location to be used
    UART0->ROUTE = UART_ROUTE_LOCATION_LOC1 | UART_ROUTE_RXPEN | UART_ROUTE_TXPEN;

    // Initializes buffers
    inputbuffer  = buffer_init(inputbufferarea,INPUTBUFFERSIZE);
    outputbuffer = buffer_init(outputbufferarea,OUTPUTBUFFERSIZE);

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
}


/**
 * @brief   UART Interrupt routine for receiving data
 *
 * @note    Receives and put it in buffer
 */

void UART0_RX_IRQHandler(void) {
uint8_t ch;

    if ( UART0->IF&(UART_IF_RXDATAV|UART_IF_RXFULL) ) {
        // Put in input buffer
        ch = UART0->RXDATA;
        (void) buffer_insert(inputbuffer,ch);
    }
}


/**
 * @brief   UART Interrupt routine for transmitting data
 *
 * @note    If there is data to transmit, send it
 * @note    UART_SendChar generates this interrupt
 */

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

/**
 * @brief   Get status of UART
 *
 * @note    Could be inline in uart.h
 */

unsigned UART_GetStatus(void) {
uint32_t w;

    w = UART0->STATUS;
    return w;
}

/**
 * @brief   Send a char
 *
 * @note    Generates an interrupt to send char
 */

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

/**
 * @brief   Send a string
 *
 * @note    Could be inline in uart.h
 */

void UART_SendString(char *s) {

    while(*s) UART_SendChar(*s++);

}

/**
 * @brief   Get a char from UART without waiting
 *
 * @note    Does not block. Returns 0 when there is none
 */

unsigned UART_GetCharNoWait(void) {
int ch;

    if( buffer_empty(inputbuffer) )
        return 0;

    ENTER_ATOMIC();
    ch = buffer_remove(inputbuffer);
    EXIT_ATOMIC();
    return ch;
}

/**
 * @brief   Get a char from UART
 *
 * @note    Does block!!!!!
 */

unsigned UART_GetChar(void) {
int ch;

    while( buffer_empty(inputbuffer) ) {}

    ENTER_ATOMIC();
    ch = buffer_remove(inputbuffer);
    EXIT_ATOMIC();
    return ch;
}

/**
 * @brief   Disable interrupts
 *
 */

void UART_EnableInterrupts(uint32_t m) {

    if( m&UART_TXINT ) UART0->IEN |= UART_IEN_TXC;
    if( m&UART_RXINT ) UART0->IEN |= UART_IEN_RXDATAV;
}

/**
 * @brief   Disable interrupts
 *
 */

void UART_DisableInterrupts(uint32_t m) {

    if( m&UART_TXINT ) UART0->IEN &= ~UART_IEN_TXC;
    if( m&UART_RXINT ) UART0->IEN &= ~UART_IEN_RXDATAV;
}

/**
 * @brief   Output char using polling
 *
 * @note    Does block!!!!!
 * @note    Trasmitter interrupts must be disabled
 */

void UART_PutCharPolling(char ch) {

    while ( (UART0->STATUS&UART_STATUS_TXBL) == 0 ) {}
    UART0->TXDATA = ch;

}


/**
 * @brief   Flush buffer
 *
 * @note    Does block!!!!!
 */

int UART_Flush(void) {
int cnt;
int ch;

    // Clear input buffer
    buffer_clear(inputbuffer);

    // Clear output buffer
    cnt = 0;
    // Disable interrupts
    UART0->IEN &= ~(UART_IEN_TXC|UART_IEN_RXDATAV);
    while( ! buffer_empty(outputbuffer) ) {
        // Wait until UART TX is free
        ch = buffer_remove(outputbuffer);
        UART_PutCharPolling(ch);
        cnt++;
    }
    // Reenable interrupts
    UART0->IEN |= UART_IEN_TXC|UART_IEN_RXDATAV;
    return cnt;
}

