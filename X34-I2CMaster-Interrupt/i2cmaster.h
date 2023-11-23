#ifndef I2CMASTER_H
#define I2CMASTER_H
/**
 * @file    i2cmaster.c
 * @brief   I2C HAL for EFM32GG (master mode)
 *
 * | Signal     | #0  | #1  | #2  | #3  | #4  | #5  | #6  |                                        |
 * |------------|-----|-----|-----|-----|-----|-----|-----|----------------------------------------|
 * | I2C0_SCL   | PA1 | PD7 | PC7 | PD15| PC1 | PF1 | PE13| I2C0 Serial Clock Line input / output  |
 * | I2C0_SDA   | PA0 | PD6 | PC6 | PD14| PC0 | PF0 | PE12| I2C0 Serial Data input / output        |
 * | I2C1_SCL   | PC5 | PB12| PE1 |     |     |     |     | I2C1 Serial Clock Line input / output  |
 * | I2C1_SDA   | PC4 | PB11| PE0 |     |     |     |     | I2C1 Serial Data input / output        |
 *
 * @version 1.0
 * @author  Hans
 * @data    16/09/2023
 */

#include <stdint.h>

/* Pins used by I2C */
#define I2CMASTER_LOC_0          (0)
#define I2CMASTER_LOC_1          (1)
#define I2CMASTER_LOC_2          (2)
#define I2CMASTER_LOC_3          (3)
#define I2CMASTER_LOC_4          (4)
#define I2CMASTER_LOC_5          (5)
#define I2CMASTER_LOC_6          (6)

/**
 * @brief  Send routines
 */
///@{
int I2CMaster_Init(
                    I2C_TypeDef  *unit,
                    uint32_t speed,
                    uint8_t loc
                );
int I2CMaster_ConfigureSpeed(
                    I2C_TypeDef *unit,
                    uint32_t speed
                );
int I2CMaster_GetStatus(
                    I2C_TypeDef *unit
                );
int I2CMaster_GetState(
                    I2C_TypeDef *unit
                );
int I2CMaster_Clear(
                    I2C_TypeDef *unit
                );
///@}

/**
 * @brief  Send routines
 */
///@{
int I2CMaster_Send(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint8_t *data,
                    uint32_t size
                );
int I2CMaster_SendStart(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint8_t *data,
                    uint32_t size
                );
int I2CMaster_SendGetStatus(
                    I2C_TypeDef *unit
                );
///@}

/**
 * @brief  Receive routines
 */
///@{
int I2CMaster_Receive(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint8_t *data,
                    uint32_t rsize
                );
int I2CMaster_ReceiveStart(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint32_t rsize
                );
int I2CMaster_ReceiveGetData(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint8_t *data,
                    uint32_t size
                );
int I2CMaster_ReceiveGetStatus(
                    I2C_TypeDef *unit
                );
///@}

/**
 * @brief Send-Receive routines
 */
///@{
int I2CMaster_SendReceive(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint8_t *tdata,
                    uint32_t tsize,
                    uint8_t *rdata,
                    uint32_t rsize
                );
int I2CMaster_SendReceiveStart(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint8_t *tdata,
                    uint32_t tsize,
                    uint32_t rsize
                );
int I2CMaster_SendReceiveGetStatus(
                    I2C_TypeDef *unit
                );
int I2CMaster_SendReceiveGetData(
                    I2C_TypeDef *unit,
                    uint16_t address,
                    uint8_t *data,
                    uint32_t rsize
                );
///@}


#endif // I2CMASTER_H
