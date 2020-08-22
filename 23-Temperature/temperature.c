
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
static uint32_t caltemp;            // Calibration Temperature
static uint32_t caltempread;        // Temperature reading at 1v25
static uint32_t adcth  = 192;       // value*100 = 2 decimal places
#define VREF    125

// Get calibration values from Device Information Page
// Cortex M are little endian (Maybe there are exceptions!)
//
static void GetCalibrationValues(void) {

    caltemp = (DEVINFO->CAL>>16)&0xFF;              // 0x0FE0_81B2
    caltempread = (DEVINFO->ADC0CAL2>>16)&0xFFF0;   // 0x0FE0_81BE

}

///
// Initializes ADC and get calibration values
//
uint32_t
Temperature_Init(uint32_t freq) {

    ADC_Init(freq,0);
    GetCalibrationValues();

    return 0;
}

///
// Returns temperature without calibration
//
uint32_t
Temperature_GetRawValue(void) {
uint32_t v;

    v = ADC_Read(ADC_TEMP);

    return v;
}

///
// Returns calibrated temperature value in Celsius according formula 2.82 in RM 28.3.4.2
//

uint32_t
Temperature_GetCalibratedValue(void) {
uint32_t v;

    v = Temperature_GetRawValue();

    v = caltemp - (caltempread-v)*VREF/(4096*adcth);

    return v;
}

