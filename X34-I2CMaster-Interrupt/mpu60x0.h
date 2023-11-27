#ifndef MPU60X0_H
#define MPU60X0_H
/**
 * @file    mpu6050.h
 * @brief   Interface for the MPU6050

 * @version 1.0.0
 * Date:    7 October 2023
 *
 */

/**
 *  @brief  Registers of the MPU60x0
 */

#define   MPU_SELF_TEST_X_GYRO           0x00
#define   MPU_SELF_TEST_Y_GYRO           0x01
#define   MPU_SELF_TEST_Z_GYRO           0x02
#define   MPU_XG_OFFSET_H                0x13
#define   MPU_XG_OFFSET_L                0x14
#define   MPU_YG_OFFSET_H                0x15
#define   MPU_YG_OFFSET_L                0x16
#define   MPU_ZG_OFFSET_H                0x17
#define   MPU_ZG_OFFSET_L                0x18
#define   MPU_SMPLRT_DIV                 0x19
#define   MPU_CONFIG                     0x1A
#define   MPU_GYRO_CONFIG                0x1B
#define   MPU_ACCEL_CONFIG               0x1C
#define   MPU_ACCEL_CONFIG_2             0x1D
#define   MPU_LP_MPU60x0_ODR             0x1E
#define   MPU_WOM_THR                    0x1F
#define   MPU_FIFO_EN                    0x23
#define   MPU_I2C_MST_CTRL               0x24
#define   MPU_I2C_SLV0_ADDR              0x25
#define   MPU_I2C_SLV0_REG               0x26
#define   MPU_I2C_SLV0_CTRL              0x27
#define   MPU_I2C_SLV1_ADDR              0x28
#define   MPU_I2C_SLV1_REG               0x29
#define   MPU_I2C_SLV1_CTRL              0x2A
#define   MPU_I2C_SLV2_ADDR              0x2B
#define   MPU_I2C_SLV2_REG               0x2C
#define   MPU_I2C_SLV2_CTRL              0x2D
#define   MPU_I2C_SLV3_ADDR              0x2E
#define   MPU_I2C_SLV3_REG               0x2F
#define   MPU_I2C_SLV3_CTRL              0x30
#define   MPU_I2C_SLV4_ADDR              0x31
#define   MPU_I2C_SLV4_REG               0x32
#define   MPU_I2C_SLV4_DO                0x33
#define   MPU_I2C_SLV4_CTRL              0x34
#define   MPU_I2C_SLV4_DI                0x35
#define   MPU_I2C_MST_STATUS             0x36
#define   MPU_INT_PIN_CFG                0x37
#define   MPU_INT_ENABLE                 0x38
#define   MPU_INT_STATUS                 0x3A
#define   MPU_ACCEL_XOUT_H               0x3B
#define   MPU_ACCEL_XOUT_L               0x3C
#define   MPU_ACCEL_YOUT_H               0x3D
#define   MPU_ACCEL_YOUT_L               0x3E
#define   MPU_ACCEL_ZOUT_H               0x3F
#define   MPU_ACCEL_ZOUT_L               0x40
#define   MPU_TEMP_OUT_H                 0x41
#define   MPU_TEMP_OUT_L                 0x42
#define   MPU_GYRO_XOUT_H                0x43
#define   MPU_GYRO_XOUT_L                0x44
#define   MPU_GYRO_YOUT_H                0x45
#define   MPU_GYRO_YOUT_L                0x46
#define   MPU_GYRO_ZOUT_H                0x47
#define   MPU_GYRO_ZOUT_L                0x48
#define   MPU_EXT_SENS_DATA_00           0x49
#define   MPU_EXT_SENS_DATA_01           0x4A
#define   MPU_EXT_SENS_DATA_02           0x4B
#define   MPU_EXT_SENS_DATA_03           0x4C
#define   MPU_EXT_SENS_DATA_04           0x4D
#define   MPU_EXT_SENS_DATA_05           0x4E
#define   MPU_EXT_SENS_DATA_06           0x4F
#define   MPU_EXT_SENS_DATA_07           0x50
#define   MPU_EXT_SENS_DATA_08           0x51
#define   MPU_EXT_SENS_DATA_09           0x52
#define   MPU_EXT_SENS_DATA_10           0x53
#define   MPU_EXT_SENS_DATA_11           0x54
#define   MPU_EXT_SENS_DATA_12           0x55
#define   MPU_EXT_SENS_DATA_13           0x56
#define   MPU_EXT_SENS_DATA_14           0x57
#define   MPU_EXT_SENS_DATA_15           0x58
#define   MPU_EXT_SENS_DATA_16           0x59
#define   MPU_EXT_SENS_DATA_17           0x5A
#define   MPU_EXT_SENS_DATA_18           0x5B
#define   MPU_EXT_SENS_DATA_19           0x5C
#define   MPU_EXT_SENS_DATA_20           0x5D
#define   MPU_EXT_SENS_DATA_21           0x5E
#define   MPU_EXT_SENS_DATA_22           0x5F
#define   MPU_EXT_SENS_DATA_23           0x60
#define   MPU_I2C_SLV0_DO                0x63
#define   MPU_I2C_SLV1_DO                0x64
#define   MPU_I2C_SLV2_DO                0x65
#define   MPU_I2C_SLV3_DO                0x66
#define   MPU_I2C_MST_DELAY_CTRL         0x67
#define   MPU_SIGNAL_PATH_RESET          0x68
#define   MPU_ACCEL_INTEL_CTRL           0x69
#define   MPU_USER_CTRL                  0x6A
#define   MPU_PWR_MGMT_1                 0x6B
#define   MPU_PWR_MGMT_2                 0x6C
#define   MPU_FIFO_COUNT_H               0x72
#define   MPU_FIFO_COUNT_L               0x73
#define   MPU_FIFO_R_W                   0x74
#define   MPU_WHO_AM_I                   0x75
#define   MPU_XA_OFFSET_H                0x77
#define   MPU_XA_OFFSET_L                0x78
#define   MPU_YA_OFFSET_H                0x7A
#define   MPU_YA_OFFSET_L                0x7B
#define   MPU_ZA_OFFSET_H                0x7D
#define   MPU_ZA_OFFSET_L                0x7E

/**
 *  @brief  Registers specific to the MPU6050
 */
#define   MPU_XA_OFFSET_H_2              0x06
#define   MPU_XA_OFFSET_L_2              0x07
#define   MPU_YA_OFFSET_H_2              0x08
#define   MPU_YA_OFFSET_L_2              0x09
#define   MPU_ZA_OFFSET_H_2              0x0A
#define   MPU_ZA_OFFSET_L_2              0x0B
#define   MPU_WHO_AM_I_2                 0x70


typedef struct MPU60x0_Struct MPU60x0_Type;

typedef struct {
    uint16_t        x;
    uint16_t        y;
    uint16_t        z;
} MPU60x0_GyroType;

typedef struct {
    uint16_t        x;
    uint16_t        y;
    uint16_t        z;
} MPU60x0_AccelType;


MPU60x0_Type *MPU60x0_Init(I2C_TypeDef *i2c, uint16_t addr);

/* Blocking calls */
int MPU60x0_WriteRegister(MPU60x0_Type *mpu, uint8_t reg, uint8_t val);
int MPU60x0_WriteMultipleRegisters( MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data);
int MPU60x0_ReadRegister(MPU60x0_Type *mpu, uint8_t reg, uint8_t *pval);
int MPU60x0_ReadMultipleRegisters(MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data);
int MPU60x0_ReadAccel(MPU60x0_Type *mpu, MPU60x0_AccelType *paccel);
int MPU60x0_ReadGyro(MPU60x0_Type *mpu, MPU60x0_GyroType *pgyro);
int MPU60x0_ReadAccelX(MPU60x0_Type *mpu, uint16_t *paccel);
int MPU60x0_ReadAccelY(MPU60x0_Type *mpu, uint16_t *paccel);
int MPU60x0_ReadAccelZ(MPU60x0_Type *mpu, uint16_t *paccel);

/* Start writing/reading calls */
int MPU60x0_StartWriteRegister(MPU60x0_Type *mpu, uint8_t reg, uint8_t val);
int MPU60x0_StartWriteMultipleRegisters( MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data);
int MPU60x0_StartReadRegister(MPU60x0_Type *mpu, uint8_t reg);
int MPU60x0_StartReadMultipleRegisters(MPU60x0_Type *mpu, uint8_t startreg, int count);
int MPU60x0_StartReadAccel(MPU60x0_Type *mpu);
int MPU60x0_StartReadGyro(MPU60x0_Type *mpu);
int MPU60x0_StartReadAccelX(MPU60x0_Type *mpu);
int MPU60x0_StartReadAccelY(MPU60x0_Type *mpu);
int MPU60x0_StartReadAccelZ(MPU60x0_Type *mpu);

/* Status call */
int MPU60x0_GetStatus(MPU60x0_Type *mpu);

/* Get data */
int MPU60x0_GetDataRegister(MPU60x0_Type *mpu, uint8_t *data);
int MPU60x0_GetDataMultipleRegisters(MPU60x0_Type *mpu, uint8_t startreg, int count, uint8_t *data);
int MPU60x0_GetDataAccel(MPU60x0_Type *mpu, MPU60x0_AccelType *paccel);
int MPU60x0_GetDataGyro(MPU60x0_Type *mpu, MPU60x0_GyroType *pgyro);
int MPU60x0_GetDataAccelX(MPU60x0_Type *mpu, uint16_t *paccel);
int MPU60x0_GetDataAccelY(MPU60x0_Type *mpu, uint16_t *paccel);
int MPU60x0_GetDataAccelZ(MPU60x0_Type *mpu, uint16_t *paccel);

#endif // MPU60X0_H
