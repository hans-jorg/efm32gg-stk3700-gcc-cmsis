#ifndef TEMPERATURE_H
#define TEMPERATURE_H
/**
 * @file    temperature.
 * @brief   Temperature Reading for EFM32GG
 * @version 1.0
 *
 * @note    Uses ADC0 Channel 8
 */

#include <stdint.h>

uint32_t Temperature_Init(uint32_t freq);
uint32_t Temperature_GetRawReading(void);
int32_t  Temperature_GetCalibratedValue(void);
int32_t  Temperature_GetUncalibratedValue(void);


#endif // TEMPERATURE_H
