/**
 * @file    i2cmaster.c
 * @brief   I2C HAL for EFM32GG
 * @version 1.0
 *
 * @note    It uses polling
 *
 */

#include  <stdint.h>
#include "em_device.h"
#include "clock_efm32gg2.h"
#include "i2cmaster.h"


/**
 * @brief Data is transfered initially to a static are. So the original data can be free'd
 *        or deallocated. Below the default values used when the symbols I2C_INPUT_BUFFER_SIZE
 *        and I2C_OUTPUT_BUFFER_SIZE are not defined.
 *
 */
#ifndef I2C_INPUT_BUFFER_SIZE
#define I2C_INPUT_BUFFER_SIZE               (20)
#endif
#ifndef I2C_OUTPUT_BUFFER_SIZE
#define I2C_OUTPUT_BUFFER_SIZE              (20)
#endif


/**
 * @brief   Structure to store transfer information
 *
 * @note    The two additional bytes in the buffers are due to the I2C addresses.
 *          A retry option is planned when an error occurs.
 */

///@{
typedef enum {
        STATE_IDLE,
        STATE_ERROR,
        STATE_TX_SENDDATA,
        STATE_TX_STOP,
        STATE_RX_SENDADDR1,
        STATE_RX_SENDADDR2,
        STATE_RX_RECEIVEDATA,
        STATE_RX_STOP,
        STATE_TXRX_SENDADDR1,
        STATE_TXRX_SENDADDR2,
        STATE_TXRX_SENDDATA,
        STATE_TXRX_SENDLAST,
        STATE_TXRX_RECEIVEDATA,
        STATE_TXRX_STOP,
    } State_t;

typedef enum {
        OP_NONE,
        OP_SEND,
        OP_RECEIVE,
        OP_SENDRECEIVE,
        OP_TEST
    } Operation_t;

typedef enum {
        STATUS_STOPPED,
        STATUS_RUN
    } Status_t;

typedef struct {
    uint8_t                 *inpointer;
    uint8_t                 *outpointer;
    uint8_t                 *inlimit;
    uint8_t                 *outlimit;
    uint16_t                insize;
    uint16_t                outsize;
    uint8_t                 inbuffer[I2C_INPUT_BUFFER_SIZE+2];
    uint8_t                 outbuffer[I2C_OUTPUT_BUFFER_SIZE+2];
    Status_t                status;
    Operation_t             operation;
    State_t                 state;
    uint8_t                 addressbytes;
} TransferInfo;
///@}


/**
 * @brief A reasonable fast copy routine for small amounts of data
 *
 * @note  a KISS (Keep it simple) approach
 */
static void Copy(uint8_t *dest, uint8_t *source, uint16_t count ) {
const uint8_t *lim = dest+count;

    while( dest < lim ) {
        *dest++ = *source++;
    }
}


/**
 * @brief Generate address bytes from an I2C address
 *
 * @returns number of bytes needed

 * @note    The bytes are already shifted to make room for the R/W bit in the low order bit.
 *
 * @note    The first address byte for a 10-bit address is 11110XX. The
 */
static int DecomposeAddress(uint16_t address, uint8_t *pa) {

    if( address <= 0x77 ) {
        *pa = (uint8_t) address;
        return 1;
    }
    if( address <= 1023 ) {
        *pa++ = ((address&0x300)>>7)|0xF8;
        *pa   = address&0xFF;
        return 2;
    }

    return 0;
}


/**
 * @brief  Informations about I2c modules.
 *
 * @note   In the present configuration of the STK3700 board, only 2 can be used: I2C0 and I2C1
 */
static TransferInfo         transferinfo[2];


/* Forward definition of ProcessInterrupt routine */
static void ProcessInterrupt(I2C_TypeDef *i2c, TransferInfo *ti);



/** Interrupt Handlers */
///@{
void I2C0_IRQHandler(void) {
    ProcessInterrupt(I2C0,&transferinfo[0]);
}

void I2C1_IRQHandler(void) {
    ProcessInterrupt(I2C1,&transferinfo[1]);
}
///@}


/**
 * @brief   GetTransferInfo from I2C pointer
 *
 * @note    Another possibility is to use an I2C* field as a key to find the corresponding
 *          structure.
 */
TransferInfo *GetTransferInfo(I2C_TypeDef *unit) {

    if( unit == I2C0 ) {
        return &transferinfo[0];
    } else if (unit == I2C1 ) {
        return &transferinfo[1];
    } else {
        return 0;
    }
}


/**
 * @brief   ProcessInterrupt
 * @note    Common routine for processing interrupt from I2C
 */
static void
ProcessInterrupt(I2C_TypeDef *i2c, TransferInfo *ti) {

    // Process interrupt according state
    switch(ti->state) {
    case STATE_IDLE:
        // Nothing to do. Just clear interrupts
        i2c->IFC = _I2C_IFC_MASK;   // Clear all interrupts
        break;
    case STATE_ERROR:
        // Nothing to do. Just clear interrupts
        i2c->IFC = _I2C_IFC_MASK;   // Clear all interrupts
        break;
    // Transmitting data
    case STATE_TX_SENDDATA:
        if( i2c->IF&I2C_IF_NACK ) {
            ti->state = STATE_ERROR;
        }
        // Data was transmitted. Send next data
        if( i2c->IF&I2C_IF_TXBL ) {
            // if it is the last data, send STOP
            if( ti->outpointer == ti->outlimit ) {
                ti->state = STATE_TX_STOP;
            }
            i2c->TXDATA = *(ti->outpointer++);
        }
        i2c->IFC = _I2C_IFC_MASK;   // Clear all interrupts
        break;
    case STATE_TX_STOP:
        i2c->CMD = I2C_CMD_STOP;
        ti->state = STATE_IDLE;
        i2c->IFC = _I2C_IFC_MASK;   // Clear all interrupts
        break;
    // Receiving data
    case STATE_RX_SENDADDR1:
        // TODO
        break;
    case STATE_RX_SENDADDR2:
        // TODO
        break;
    case STATE_RX_RECEIVEDATA:
        // TODO
        break;
    case STATE_RX_STOP:
        i2c->CMD = I2C_CMD_STOP;
        ti->state = STATE_IDLE;
        i2c->IFC = _I2C_IFC_MASK;   // Clear all interrupts
        // TODO
        break;
    // Sending and then receiving data
    case STATE_TXRX_SENDADDR1:
        // TODO
        break;
    case STATE_TXRX_SENDADDR2:
        // TODO
        break;
    case STATE_TXRX_SENDDATA:
        // TODO
        break;
    case STATE_TXRX_SENDLAST:
        // TODO
        break;
    case STATE_TXRX_RECEIVEDATA:
        // TODO
        break;
    case STATE_TXRX_STOP:
        i2c->CMD = I2C_CMD_STOP;
        ti->state = STATE_IDLE;
        i2c->IFC = _I2C_IFC_MASK;   // Clear all interrupts
        // TODO
        break;
    }

    return;
}


/**
 * @brief   Initialize TransferInfo
 */
static void TransferInfoInit(TransferInfo *ti, uint16_t isize, uint16_t osize) {

    ti->inpointer    = ti->inbuffer;
    ti->outpointer   = ti->outbuffer;
    ti->inlimit      = ti->inbuffer+isize-1;
    ti->outlimit     = ti->outbuffer+osize-1;
    ti->insize       = isize;
    ti->outsize      = osize;
    ti->status       = STATUS_STOPPED;
    ti->operation    = OP_NONE;
    ti->state        = STATE_IDLE;
    ti->addressbytes = 1;
}

/**
 * @brief   Clear current transfer and reset the TransferInfo structure
 *
 * @note
 */
int I2CMaster_Clear(I2C_TypeDef *i2c) {
TransferInfo *ti;

    ti = GetTransferInfo(i2c);
    if( ti == 0 ) {
        return 0;
    }

    TransferInfoInit(ti,0,0);

    /* Clear transmission */
    i2c->CMD = I2C_CMD_CLEARPC|I2C_CMD_CLEARTX;

    //
    return 1;
}


/**
 * @brief   Get status from I2C transfer
 */
inline int I2CMaster_Status(I2C_TypeDef *i2c) {
TransferInfo *ti;

    ti = GetTransferInfo(i2c);
    if( ti == 0 ) {
        return 0;
    }

    return (int) (ti->status);
}



/**
 * @brief   Get state from I2C transfer
 */
inline int I2CMaster_State(I2C_TypeDef *i2c) {
TransferInfo *ti;

    ti = GetTransferInfo(i2c);
    if( ti == 0 ) {
        return 0;
    }

    return (int) (ti->state);
}


/**
 * @brief  Enable clock for I2C module
 *
 * @param  i2c: Pointer to I2C module registers
 *
 * @return 0: OK. Negative: Error
 */
static int I2CMaster_EnableClock(I2C_TypeDef *i2c) {

    if( i2c == I2C0 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_I2C0;
    } else if ( i2c == I2C1 ) {
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_I2C1;
    } else {
        return -1;
    }
    return 0;
}


/**
 * @brief   Initialize I2C unit as master
 *
 * @parms  i2c:     Pointer to I2C register area as defined in Silicon Labs CMSIS header files
 * @parms  speed:   Speed in KHz (e.g. 100, 400, but can be less)
 * @parms  loc:     Location code for both (SCL and SDA) pins
 *
 *
 * | Signal     | #0  | #1  | #2  | #3  | #4  | #5  | #6  |                                        |
 * |------------|-----|-----|-----|-----|-----|-----|-----|----------------------------------------|
 * | I2C0_SCL   | PA1 | PD7 | PC7 | PD15| PC1 | PF1 | PE13| I2C0 Serial Clock Line input / output  |
 * | I2C0_SDA   | PA0 | PD6 | PC6 | PD14| PC0 | PF0 | PE12| I2C0 Serial Data input / output        |
 * | I2C1_SCL   | PC5 | PB12| PE1 |     |     |     |     | I2C1 Serial Clock Line input / output  |
 * | I2C1_SDA   | PC4 | PB11| PE0 |     |     |     |     | I2C1 Serial Data input / output        |
 *
 */
int I2CMaster_Init(I2C_TypeDef *i2c, uint32_t speed, uint8_t loc) {

    // Enable clock for I2C peripheral
    I2CMaster_EnableClock(i2c);

    /* Enable I2C_TypeDef peripheral */
    i2c->CTRL |= I2C_CTRL_EN;

    /* Configure speed for present clock frequency */
    I2CMaster_ConfigureSpeed(i2c,speed);

    /* Configure pins to be used for I2C_TypeDef */
    i2c->ROUTE = (i2c->ROUTE&~_I2C_ROUTE_LOCATION_MASK)|(loc<<_I2C_ROUTE_LOCATION_SHIFT);
    /* Enable pins */
    i2c->ROUTE |= (I2C_ROUTE_SCLPEN|I2C_ROUTE_SDAPEN);

    /* Configure as master */
    i2c->CTRL &= ~I2C_CTRL_SLAVE;

    /* Additional configuration */
    i2c->CTRL |= I2C_CTRL_AUTOSN;         /* Automatic STOP on NACK (Master only) */
    i2c->CTRL |= I2C_CTRL_AUTOACK;        /* Automatic STOP on NACK (Master only) */

    return 0;
}


/**
 * @brief   Send data to slave
 *
 * @note    Data should not be changed until transmission ends
 */
int I2CMaster_SendStart(I2C_TypeDef *i2c, uint16_t address, uint8_t *data, uint32_t size) {
int nab;                // number of address bytes
TransferInfo *ti;

    // Test state from
    if( (i2c->STATE&_I2C_STATE_STATE_MASK) != I2C_STATE_STATE_IDLE )
        return -1;

    // Test state from TransferInfo
    ti = GetTransferInfo(i2c);

    if( ti->status == STATUS_RUN )
        return -2;

    if( ti->state != STATE_IDLE && ti->state != STATE_ERROR )
        return -3;

    // Initialize TransferInfo struct
    TransferInfoInit(ti,0,0);

    nab = DecomposeAddress(address, ti->outbuffer);
    ti->outpointer   = ti->outbuffer;
    ti->outlimit     = ti->outpointer+nab+size-1;
    ti->operation    = OP_SEND;
    ti->addressbytes = nab;

    // Copy data to local buffer
    Copy(ti->outpointer+nab,data,size);


    // Wait until TXDATA is empty. Just in case */
    ti->state = STATE_TX_SENDDATA;

    // Enable interrupts
    i2c->IEN |= I2C_IEN_ACK|I2C_IEN_TXBL|I2C_IEN_NACK;

    // Send first byte (it is the address or part of it)
    i2c->CMD = I2C_CMD_START;
    i2c->TXDATA = *(ti->outpointer++);

    return 0;

}


/**
 * @brief   Polling like send routine
 *
 * @note
 */
int I2CMaster_Send(I2C_TypeDef *i2c, uint16_t address, uint8_t *data, uint32_t size) {
int rc;
TransferInfo *ti;

    ti = GetTransferInfo(i2c);


    rc = I2CMaster_SendStart(i2c,address,data,size);

    while (ti->state==STATE_TX_SENDDATA) {}

    return 0;
}


/**
 * @brief
 *
 * @note
 */
int I2CMaster_SendGetStatus(I2C_TypeDef *i2c) {
TransferInfo *ti;

    // Test state from TransferInfo
    ti = GetTransferInfo(i2c);

    return ti->state == STATE_IDLE;
}



/**
 * @brief   Receive data from slave
 *
 * @note
 */
int I2CMaster_ReceiveStart(I2C_TypeDef *i2c, uint16_t address, uint32_t size) {
int nab;                // number of address bytes
TransferInfo *ti;

    // Test state from
    if( (i2c->STATE&_I2C_STATE_STATE_MASK) != I2C_STATE_STATE_IDLE )
        return -1;

    // Test state from TransferInfo
    ti = GetTransferInfo(i2c);

    if( ti->status == STATUS_RUN )
        return -2;

    if( ti->state != STATE_IDLE && ti->state != STATE_ERROR )
        return -3;

    // Initialize TransferInfo struct
    TransferInfoInit(ti,0,0);

    nab = DecomposeAddress(address, ti->outbuffer);
    ti->outpointer   = ti->outbuffer;
    ti->outlimit     = ti->outpointer+nab+size-1;
    ti->operation    = OP_RECEIVE;
    ti->addressbytes = nab;


    // Wait until TXDATA is empty. Just in case */
    ti->state = STATE_RX_RECEIVEDATA;

    // Enable interrupts
    i2c->IEN |= I2C_IEN_ACK|I2C_IEN_RXDATAV|I2C_IEN_NACK|I2C_IEN_MSTOP|I2C_IEN_START;

    // Send first byte (it is the address or part of it)
    i2c->CMD = I2C_CMD_START;
    i2c->TXDATA = *(ti->outpointer++);

    return 0;

}


/**
 * @brief
 *
 * @note
 */

/**
 * @brief   Configure I2C unit according to the actual clock
 *
 * @note    f_SCL  = 1/(T_low+T_high)
 *
 * @note    The formulas below does not work for N less than 3
 *
 *          T_high = (N_high x (CLKDIV+1))/f_HFPERCLK
 *          T_low  = (N_low x (CLKDIV+1))/f_HFPERCLK
 *
 * @note
 *          Clock Low/High Ratio
 *
 *          STANDARD                      4:4
 *          ASYMMETRIC                    6:3
 *          FAST                         11:6
 *
 */
int I2CMaster_ConfigureSpeed(I2C_TypeDef *i2c, uint32_t speed) {
uint32_t div;
uint32_t perclk;
unsigned t2,th,tl;
uint32_t nh,nl;
ClockConfiguration_t clockconf;

    ClockGetConfiguration(&clockconf);

    perclk = clockconf.hfperclkfreq;
    div    = clockconf.hclkdiv;

    t2 = 1000*perclk/speed;

    tl = (t2+1)/2;              // Must be adjusted for non symmetrical clock signal
    th = t2-tl;

    // TODO: Calculate N_high and N_low
    nh = 0;
    nl = 0;

    i2c->CLKDIV = (nh<<_I2C_CLKDIV_DIV_SHIFT)&_I2C_CLKDIV_DIV_MASK;

    return 0;
}
