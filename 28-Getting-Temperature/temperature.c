
/**
 * @file    temperature.c
 * @brief   Temperature Reading for EFM32GG
 * @version 1.0
 *
 * @note    Uses ADC0 Channel 8
 */


#include <stdint.h>
#include "em_device.h"
#include "temperature.h"
#include "adc.h"


// The value can be calibrated by values obtained during production
// The values are:
//      ADC0_TEMP_0_READ_1V25       From Device Information Page (See RM 5.6 Table 5.4)
//      CAL_TEMP_0                  From Device Information Page (See RM 5.6 Table 5.4)
//      TGRAD_ADCTH                 From DS 3.10 Table 3.14
//      For EFM32GG990F1024         -1.92 mV/C or -6.32 units/C
//
static int32_t caltemp;            // Calibration Temperature
static int32_t caltempread;        // Temperature reading at 1v25
static int32_t adcthu_num = 63;
static int32_t adcthu_den = 10;

/**
 * @brief   Get calibration values from Device Information Page
 *
 * @note    Cortex M are little endian (Maybe there are exceptions!)
 */
static void GetCalibrationValues(void) {

    caltemp = (DEVINFO->CAL>>16)&0xFF;              // 0x0FE0_81B2
    caltempread = (DEVINFO->ADC0CAL2>>20)&0xFFF;    // 0x0FE0_81BE

}

/**
 * @brief   Initializes ADC and get calibration values
 */
uint32_t
Temperature_Init(uint32_t freq) {

    ADC_Init(freq);
    GetCalibrationValues();

    return 0;
}

/**
 * @brief Returns ADC reading
 *
 * @note    Must be a value between 2100 and 2600
 */

uint32_t
Temperature_GetRawReading(void) {
uint32_t v;

    v = ADC_Read(ADC_TEMP);

    return v;
}

/**
 * @brief   Returns an uncalibrated value
 *
 * @note    It is interpolated between points (2600,-40) and (2100,85)
 *
 * @note    The slope is -475/125 = - 3.8 (approx. -4)
 */

int32_t
Temperature_GetUncalibratedValue(void) {
int32_t v;

    v = (int32_t) Temperature_GetRawReading();

    v = -40 - (125*(v-2600))/475;

    return v;
}


/**
 * @brief   Returns calibrated temperature value in Celsius
 *
 * @note    It used the formula 2.82 in RM 28.3.4.2
 *
 * @note    It uses the pair (Temperature,Reading) given in the DI Page
 *
 * @note    The pair is (CAL_TEMP_0.ADC0_TEMP_0_READ_1V25)
 *
 * @note    The pair in a specific device is (20,21XX)
 *
 * @note    The slope is based on the TGRAD_ADCTH value in the datasheet
 *
 * @note    The TGRAD_ADCTH value is -1.92 mV/C = -6.3 ADCunits/C
 *
 */

int32_t
Temperature_GetCalibratedValue(void) {
int32_t v;

    v = (int32_t) Temperature_GetRawReading();

    v = caltemp + (caltempread-v)*adcthu_den/adcthu_num;

    return v;
}

