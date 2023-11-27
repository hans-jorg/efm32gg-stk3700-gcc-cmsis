/** ***************************************************************************
 * @file    mpu6050.c
 * @brief   HAL for the MPU 6050 MPU
 * @version 1.0
******************************************************************************/



// Registers
/**
 *   | Hwx |  Dec |     Description             | R/W  |OBS|
 *   |-----|------|-----------------------------|------|---|
 *   |  0  |    0 | SELF_TEST_X_GYRO            | R/W  |   |
 *   |  1  |    1 | SELF_TEST_Y_GYRO            | R/W  |   |
 *   |  2  |    2 | SELF_TEST_Z_GYRO            | R/W  |   |
 *   |  13 |   19 | XG_OFFSET_H                 | R/W  |   |
 *   |  14 |   20 | XG_OFFSET_L                 | R/W  |   |
 *   |  15 |   21 | YG_OFFSET_H                 | R/W  |   |
 *   |  16 |   22 | YG_OFFSET_L                 | R/W  |   |
 *   |  17 |   23 | ZG_OFFSET_H                 | R/W  |   |
 *   |  18 |   24 | ZG_OFFSET_L                 | R/W  |   |
 *   |  19 |   25 | SMPLRT_DIV                  | R/W  |   |
 *   |  1A |   26 | CONFIG                      | R/W  |   |
 *   |  1B |   27 | GYRO_CONFIG                 | R/W  |   |
 *   |  1C |   28 | MPU60x0_CONFIG                | R/W  |   |
 *   |  1D |   29 | MPU60x0_CONFIG                | R/W  |   |
 *   |  1E |   30 | LP_MPU60x0_ODR                | R/W  |   |
 *   |  1F |   31 | WOM_THR                     | R/W  |   |
 *   |  23 |   35 | FIFO_EN                     | R/W  |   |
 *   |  24 |   36 | I2C_MST_CTRL                | R/W  |   |
 *   |  25 |   37 | I2C_SLV0_ADDR               | R/W  |   |
 *   |  26 |   38 | I2C_SLV0_REG                | R/W  |   |
 *   |  27 |   39 | I2C_SLV0_CTRL               | R/W  |   |
 *   |  28 |   40 | I2C_SLV1_ADDR               | R/W  |   |
 *   |  29 |   41 | I2C_SLV1_REG                | R/W  |   |
 *   |  2A |   42 | I2C_SLV1_CTRL               | R/W  |   |
 *   |  2B |   43 | I2C_SLV2_ADDR               | R/W  |   |
 *   |  2C |   44 | I2C_SLV2_REG                | R/W  |   |
 *   |  2D |   45 | I2C_SLV2_CTRL               | R/W  |   |
 *   |  2E |   46 | I2C_SLV3_ADDR               | R/W  |   |
 *   |  2F |   47 | I2C_SLV3_REG                | R/W  |   |
 *   |  30 |   48 | I2C_SLV3_CTRL               | R/W  |   |
 *   |  31 |   49 | I2C_SLV4_ADDR               | R/W  |   |
 *   |  32 |   50 | I2C_SLV4_REG                | R/W  |   |
 *   |  33 |   51 | I2C_SLV4_DO                 | R/W  |   |
 *   |  34 |   52 | I2C_SLV4_CTRL               | R/W  |   |
 *   |  35 |   53 | I2C_SLV4_DI                 | R    |   |
 *   |  36 |   54 | I2C_MST_STATUS              | R    |   |
 *   |  37 |   55 | INT_PIN_CFG                 | R/W  |   |
 *   |  38 |   56 | INT_ENABLE                  | R/W  |   |
 *   |  3A |   58 | INT_STATUS                  | R    |   |
 *   |  3B |   59 | MPU60x0_XOUT_H                | R    |   |
 *   |  3C |   60 | MPU60x0_XOUT_L                | R    |   |
 *   |  3D |   61 | MPU60x0_YOUT_H                | R    |   |
 *   |  3E |   62 | MPU60x0_YOUT_L                | R    |   |
 *   |  3F |   63 | MPU60x0_ZOUT_H                | R    |   |
 *   |  40 |   64 | MPU60x0_ZOUT_L                | R    |   |
 *   |  41 |   65 | TEMP_OUT_H                  | R    |   |
 *   |  42 |   66 | TEMP_OUT_L                  | R    |   |
 *   |  43 |   67 | GYRO_XOUT_H                 | R    |   |
 *   |  44 |   68 | GYRO_XOUT_L                 | R    |   |
 *   |  45 |   69 | GYRO_YOUT_H                 | R    |   |
 *   |  46 |   70 | GYRO_YOUT_L                 | R    |   |
 *   |  47 |   71 | GYRO_ZOUT_H                 | R    |   |
 *   |  48 |   72 | GYRO_ZOUT_L                 | R    |   |
 *   |  49 |   73 | EXT_SENS_DATA_00            | R    |   |
 *   |  4A |   74 | EXT_SENS_DATA_01            | R    |   |
 *   |  4B |   75 | EXT_SENS_DATA_02            | R    |   |
 *   |  4C |   76 | EXT_SENS_DATA_03            | R    |   |
 *   |  4D |   77 | EXT_SENS_DATA_04            | R    |   |
 *   |  4E |   78 | EXT_SENS_DATA_05            | R    |   |
 *   |  4F |   79 | EXT_SENS_DATA_06            | R    |   |
 *   |  50 |   80 | EXT_SENS_DATA_07            | R    |   |
 *   |  51 |   81 | EXT_SENS_DATA_08            | R    |   |
 *   |  52 |   82 | EXT_SENS_DATA_09            | R    |   |
 *   |  53 |   83 | EXT_SENS_DATA_10            | R    |   |
 *   |  54 |   84 | EXT_SENS_DATA_11            | R    |   |
 *   |  55 |   85 | EXT_SENS_DATA_12            | R    |   |
 *   |  56 |   86 | EXT_SENS_DATA_13            | R    |   |
 *   |  57 |   87 | EXT_SENS_DATA_14            | R    |   |
 *   |  58 |   88 | EXT_SENS_DATA_15            | R    |   |
 *   |  59 |   89 | EXT_SENS_DATA_16            | R    |   |
 *   |  5A |   90 | EXT_SENS_DATA_17            | R    |   |
 *   |  5B |   91 | EXT_SENS_DATA_18            | R    |   |
 *   |  5C |   92 | EXT_SENS_DATA_19            | R    |   |
 *   |  5D |   93 | EXT_SENS_DATA_20            | R    |   |
 *   |  5E |   94 | EXT_SENS_DATA_21            | R    |   |
 *   |  5F |   95 | EXT_SENS_DATA_22            | R    |   |
 *   |  60 |   96 | EXT_SENS_DATA_23            | R    |   |
 *   |  63 |   99 | I2C_SLV0_DO                 | R    |   |
 *   |  64 |  100 | I2C_SLV1_DO                 | R    |   |
 *   |  65 |  101 | I2C_SLV2_DO                 | R    |   |
 *   |  66 |  102 | I2C_SLV3_DO                 | R    |   |
 *   |  67 |  103 | I2C_MST_DELAY_CTRL          | R    |   |
 *   |  68 |  104 | SIGNAL_PATH_RESET           | R    |   |
 *   |  69 |  105 | MPU60x0_INTEL_CTRL            | R    |   |
 *   |  6A |  106 | USER_CTRL                   | R    |   |
 *   |  6B |  107 | PWR_MGMT_1                  | R    |   |
 *   |  6C |  108 | PWR_MGMT_2                  | R    |   |
 *   |  72 |  114 | FIFO_COUNT_H                | R    |   |
 *   |  73 |  115 | FIFO_COUNT_L                | R    |   |
 *   |  74 |  116 | FIFO_R_W                    | R    |   |
 *   |  75 |  117 | WHO_AM_I                    | R    |   |
 *   |  77 |  119 | XA_OFFSET_H                 | R    |   |
 *   |  78 |  120 | XA_OFFSET_L                 | R    |   |
 *   |  7A |  122 | YA_OFFSET_H                 | R    |   |
 *   |  7B |  123 | YA_OFFSET_L                 | R    |   |
 *   |  7D |  125 | ZA_OFFSET_H                 | R    |   |
 *   |  7E |  126 | ZA_OFFSET_L                 | R    |   |
 *   |   6 |    6 | XA_OFFSET_H                 | R    | * |
 *   |   7 |    7 | XA_OFFSET_L                 | R    | * |
 *   |   8 |    8 | YA_OFFSET_H                 | R    | * |
 *   |   9 |    9 | YA_OFFSET_L                 | R    | * |
 *   |   A |   10 | 0ZA_OFFSET_H                | R    | * |
 *   |   B |   11 | ZA_OFFSET_L                 | R    | * |
 *   |  6C |  108 | PWR_MGMT_2                  | R    | * |
 *
 *  1: MPU-6050 only
    2: Different layout on the MPU-6050
 */

#include <stdint.h>
#include "em_device.h"
#include "i2cmaster.h"
#include "mpu60x0.h"


struct MPU60x0_Struct {
    I2C_TypeDef             *i2c;
    uint16_t                addr;
};

typedef union {
    uint8_t         b[2];
    uint16_t        w;
} AllignedValue;




MPU60x0_Type Mpu = { 0 };

// From CMSIS
#define REVBYTES16(X)     __REVSH((X))
#define REVBYTES32(X)     __REV((X))

MPU60x0_Type *MPU60x0_Init(I2C_TypeDef *i2c, uint16_t addr) {


    return &Mpu;
}


/**
 *  @brief   Write a register
 *
 */
int MPU60x0_WriteRegister(MPU60x0_Type *mpu, uint8_t reg, uint8_t val) {

    return 0;
}


/**
 *  @brief  Write registers starting at a given value
 *
 */
int MPU60x0_WriteMultipleRegisters( MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data) {

    return 0;
}


/**
 *  @brief  Read a register
 *
 */
int MPU60x0_ReadRegister(MPU60x0_Type *mpu, uint8_t reg, uint8_t *pval) {

    return 0;
}


/**
 *  @brief  Read registers starting at a given value
 *
 *  @note   Beware of loops in the register map
 *
 */
int MPU60x0_ReadMultipleRegisters(MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data) {

    return 0;
}


/**
 *  @brief  Read acceleration information in 3-axes
 *
 */
int MPU60x0_ReadAccel(MPU60x0_Type *mpu, MPU60x0_AccelType *paccel ) {

    return 0;
}


/**
 *  @brief Read gyro information in 3-axes
 *
 */
int MPU60x0_ReadGyro(MPU60x0_Type *mpu, MPU60x0_GyroType *pgyro) {

    return 0;
}


/**
 *  @brief  Read acceleration in X-axis
 *
 */
int MPU60x0_ReadAccelX(MPU60x0_Type *mpu, uint16_t *paccel) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_ACCEL_XOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    2
                );

    while( MPU60x0_GetStatus(mpu) == 0 ) {}

    rc = I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data.b,
                    2
                );
    *paccel = REVBYTES16(data.w);
    return 0;
}


/**
 *  @brief  Read acceleration in Y-axis
 *
 */
int MPU60x0_ReadAccelY(MPU60x0_Type *mpu, uint16_t *paccel) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_ACCEL_YOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    2
                );

    while( MPU60x0_GetStatus(mpu) == 0 ) {}

    rc = I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data.b,
                    2
                );
    *paccel = REVBYTES16(data.w);
    return 0;
}


/**
 *  @brief  Read acceleration in Z-axis
 *
 */
int MPU60x0_ReadAccelZ(MPU60x0_Type *mpu, uint16_t *paccel) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_ACCEL_ZOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    2
                );

    while( MPU60x0_GetStatus(mpu) == 0 ) {}

    rc = I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data.b,
                    2
                );
    *paccel = REVBYTES16(data.w);
    return 0;
}


/**
 *  @brief   Start writing to a register
 */
int MPU60x0_StartWriteRegister(MPU60x0_Type *mpu, uint8_t reg, uint8_t val) {

    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_StartWriteMultipleRegisters( MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data) {
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */;
int MPU60x0_StartReadRegister(MPU60x0_Type *mpu, uint8_t reg) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = reg;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    1
                );
    return 0;
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */;
int MPU60x0_StartReadMultipleRegisters(MPU60x0_Type *mpu, uint8_t startreg, int count) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = startreg;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    count
                );
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */;
int MPU60x0_StartReadAccel(MPU60x0_Type *mpu) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_ACCEL_XOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    6
                );

    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_StartReadGyro(MPU60x0_Type *mpu) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_GYRO_XOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    6
                );
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_StartReadAccelX(MPU60x0_Type *mpu) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_ACCEL_XOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    2
                );

    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_StartReadAccelY(MPU60x0_Type *mpu) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_ACCEL_YOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    2
                );

    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_StartReadAccelZ(MPU60x0_Type *mpu) {
AllignedValue data;
int rc;
uint8_t outdata[1];

    outdata[0] = MPU_ACCEL_ZOUT_H;
    rc = I2CMaster_SendReceiveStart(
                    mpu->i2c,
                    mpu->addr,
                    outdata,
                    1,
                    2
                );

    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */

/* Status call */
int MPU60x0_GetStatus(MPU60x0_Type *mpu) {

    // TODO
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */

/* Get data */
int MPU60x0_GetDataRegister(MPU60x0_Type *mpu, uint8_t *data) {

    I2CMaster_SendReceiveGetData(
                    mpu -> i2c,
                    mpu->addr,
                    data,
                    1
                );
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_GetDataMultipleRegisters(MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data) {

    I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data,
                    count
                );
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_GetDataAccel(MPU60x0_Type *mpu, MPU60x0_AccelType *paccel) {
AllignedValue data[3];

    I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data[0].b,
                    6
                );
    paccel->x = REVBYTES16(data[0].w);
    paccel->y = REVBYTES16(data[1].w);
    paccel->z = REVBYTES16(data[2].w);

    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_GetDataGyro(MPU60x0_Type *mpu, MPU60x0_GyroType *pgyro) {
AllignedValue data[3];

    I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data[0].b,
                    6
                );
    pgyro->x = REVBYTES16(data[0].w);
    pgyro->y = REVBYTES16(data[1].w);
    pgyro->z = REVBYTES16(data[2].w);

    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_GetDataAccelX(MPU60x0_Type *mpu, uint16_t *paccel) {
AllignedValue data;

    I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data.b,
                    2
                );
    *paccel = REVBYTES16(data.w);
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_GetDataAccelY(MPU60x0_Type *mpu, uint16_t *paccel) {
AllignedValue data;

    I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data.b,
                    2
                );
    *paccel = REVBYTES16(data.w);
    return 0;
}


/**
 *   @brief  Start writiing to multiple registers
 */
int MPU60x0_GetDataAccelZ(MPU60x0_Type *mpu, uint16_t *paccel) {
AllignedValue data;

    I2CMaster_SendReceiveGetData(
                    mpu->i2c,
                    mpu->addr,
                    data.b,
                    2
                );
    *paccel = REVBYTES16(data.w);
    return 0;
}
