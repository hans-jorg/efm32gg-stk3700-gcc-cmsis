/**
 * @file    i2cmaster.c
 * @brief   I2C HAL for EFM32GG
 * @version 1.0
 *
 * @note    It uses polling
 *
 * @note    Only 7-bit addresses are supported
 */

#include  <stdint.h>
#include "em_device.h"
#include "clock_efm32gg_ext.h"
#include "i2cmaster.h"



/**
 * @brief   Get status from I2C transfer
 *
 * @note
 */
inline int I2CMaster_Status(I2C_TypeDef *i2c) {

    return i2c->STATUS;
}



/**
 * @brief   Get state from I2C transfer
 *
 * @note
 */
inline int I2CMaster_State(I2C_TypeDef *i2c) {

    return (i2c->STATE&_I2C_STATE_STATE_MASK)>>_I2C_STATE_STATE_SHIFT;
}


/**
 * @brief  Short description of function
 *
 * @note   Long description of function
 *
 * @param  Description of parameter
 *
 * @return Description of return parameters
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
int I2CMaster_Send(I2C_TypeDef *i2c, uint16_t address, uint8_t *data, uint32_t size) {
int i;
uint32_t nmb = size-1;  /* Middle bytes */
uint8_t c;
uint32_t timecnt;
const uint32_t TIMEOUT = 1000000; // TODO: Must be adjusted!!!!!

    /* Clear transmission */
    i2c->CMD = I2C_CMD_CLEARPC|I2C_CMD_CLEARTX;

    /* Send address and direction */
    c = (uint8_t) (address<<1);    // R/W* = 0 means transmit
    // Wait until TXDATA is empty. Just in case */
    timecnt = TIMEOUT;
    if( timecnt == 0 )
        goto error;
    if( i2c->STATE&I2C_STATE_NACKED )
        goto error;
    i2c->CMD = I2C_CMD_START;
    i2c->TXDATA = c;

    /* Send middle bytes */
    for(i=0;i<nmb;i++) {
        // Wait until transmission completed */
        timecnt = TIMEOUT;
        while( (timecnt--) && ((i2c->STATUS&I2C_STATUS_TXBL) == 0) ) {}
        if( timecnt == 0 )
            goto error;
        if( i2c->STATE&I2C_STATE_NACKED )
            goto error;
        i2c->TXDATA = data[i];
    }

    /* Send last byte with a STOP signal after it */
    i2c->CMD = I2C_CMD_STOP;
    i2c->TXDATA = data[i];

    /* Wait until transmission completed */
    timecnt = TIMEOUT;
    while( (timecnt--) && ((i2c->STATUS&I2C_STATUS_TXC) == 0) ) {}
    if( timecnt == 0 )
        goto error;
    if( i2c->STATE&I2C_STATE_NACKED )
        goto error;

    return 0;

    /* Error handling */
error:
    /* Abort */
    i2c->CMD = I2C_CMD_ABORT;
    /* Clear transmission */
    i2c->CMD = I2C_CMD_CLEARPC|I2C_CMD_CLEARTX;
    return -1;
}


/**
 * @brief   Receive data from slave
 *
 * @note
 */
int I2CMaster_Receive(I2C_TypeDef *i2c, uint16_t address, uint8_t *data, uint32_t maxsize) {
int cnt;
uint8_t c;
uint32_t timecnt;
const uint32_t TIMEOUT = 1000000; // TODO: Must be adjusted!!!!!

    /* Clear transmission */
    i2c->CMD = I2C_CMD_CLEARPC|I2C_CMD_CLEARTX;

    /* Send address and direction */
    c = (uint8_t) (address<<1)|1;    // R/W* = 1 means receive
    // Wait until TXDATA is empty. Just in case */
    timecnt = TIMEOUT;
    if( timecnt == 0 )
        goto error;
    if( i2c->STATE&I2C_STATE_NACKED )
        goto error;
    i2c->CMD = I2C_CMD_START;
    i2c->TXDATA = c;

    /* Receive data until the data array is full */
    cnt=0;
    while( cnt < maxsize ) {
        // Wait until transmission completed */
        timecnt = TIMEOUT;
        while( (timecnt--)
            && ((i2c->STATUS&I2C_STATUS_RXDATAV) == 0)
            && ((i2c->STATE&I2C_STATE_NACKED)) ) {}
        if( timecnt == 0 )
            goto error;
        data[cnt] = i2c->RXDATA;
        cnt++;
    }

    /* Send a byte with a STOP signal after it */
    i2c->CMD = I2C_CMD_STOP;
    i2c->TXDATA = 0;

    return cnt;

    /* Error handling */
error:
    /* Abort */
    i2c->CMD = I2C_CMD_ABORT;
    /* Clear transmission */
    i2c->CMD = I2C_CMD_CLEARPC|I2C_CMD_CLEARTX;
    return -1;
}


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

    perclk = clockconf.perfreq;
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
