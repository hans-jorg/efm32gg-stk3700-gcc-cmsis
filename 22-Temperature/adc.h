/** ***************************************************************************
 * @file    adc.h
 * @brief   ADC HAL for EFM32GG STK3200
 * @version 1.0
******************************************************************************/
#ifndef ADC_H
#define ADC_H

#include <stdint.h>


#define ADC_BIT(N) ((1UL)<<(N))

#define ADC_POSVREF             (0)
#define ADC_POSBITS             (4)

#define ADC_FREQMAX             13000000
#define ADC_FREQMIN             32000

// Channel encoding for ADC_Read, ADC_StartReading, ADC_GetReading and ADC_ConfigChannel
#define ADC_CH0          0
#define ADC_CH1          1
#define ADC_CH2          2
#define ADC_CH3          3
#define ADC_CH4          4
#define ADC_CH5          5
#define ADC_CH6          6
#define ADC_CH7          7
#define ADC_TEMP         8
#define ADC_VDD_3        9
#define ADC_VDD_2       10
#define ADC_VSS         11
#define ADC_VREF_2      12
#define ADC_DAC0        13
#define ADC_DAC1        14
//Filler                15
#define ADC_DIFF_CH01   16
#define ADC_DIFF_CH23   17
#define ADC_DIFF_CH45   18
#define ADC_DIFF_CH67   19
#define ADC_DIFF_0      20

// Configuration parameters for ADC_Init
#define ADC_USE_BYPASS      ADC_BIT(0)
#define ADC_USE_DECAP       ADC_BIT(1)
#define ADC_USE_RCFILT      ADC_BIT(2)
#define ADC_WARMUP_NORMAL   ADC_BIT(3)
#define ADC_WARMUP_ALWAYS   ADC_BIT(4)

uint32_t ADC_Init(uint32_t pre, uint32_t config);

uint32_t ADC_Read(uint32_t ch);

uint32_t ADC_StartReading(uint32_t ch);
uint32_t ADC_GetReading(uint32_t ch);

#endif // ADC_H

