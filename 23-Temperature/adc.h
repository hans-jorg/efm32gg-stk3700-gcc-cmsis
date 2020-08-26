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

uint32_t ADC_Init(uint32_t freq);

uint32_t ADC_Read(uint32_t ch);
uint32_t ADC_StartReading(uint32_t ch);
uint32_t ADC_GetReading(uint32_t ch);
uint32_t ADC_ConfigChannel(uint32_t ch, uint32_t config);
///
// ADC_Config uses the same codification of the CMSIS compatible library
// See efm32gg_adc.h
// OBS: Field INPUTSEL is set by the ADC_Config routine
//      Fields PRSSEL and PRSEN not used
//
// ADC_SINGLECTRL_REP
//          0 = Single conversion mode is deactivated after one conversion
//          1 = Single conversion mode is converting continuously until SINGLESTOP is written
// ADC_SINGLECTRL_ADJ
//          0 = Results are right adjusted
//          1 = Results are left adjusted
// _ADC_SINGLECTRL_RES (Mask = _ADC_SINGLECTRL_RES_MASK)
//          0 = 12 bits (ADC_SINGLECTRL_RES_12BIT)
//          1 = 8 bits  (ADC_SINGLECTRL_RES_8BIT)
//          2 = 6 bits  (ADC_SINGLECTRL_RES_6BIT)
//          3 = Oversampling enabled  (ADC_SINGLECTRL_RES_OVS)
// ADC_SINGLECTRL_REF (Mask = _ADC_SINGLECTRL_REF_MASK)
//          0 = Internal 1.25 V reference (ADC_SINGLECTRL_REF_1V25)
//          1 = 2V5 Internal 2.5 V reference (ADC_SINGLECTRL_REF_2V5
//          2 = VDD Buffered VDD (ADC_SINGLECTRL_REF_VDD)
//          3 = 5VDIFF Internal differential 5 V reference (ADC_SINGLECTRL_REF_5VDIFF)
//          4 = EXTSINGLE Single ended external reference (ADC_SINGLECTRL_REF_EXTSINGLE)
//          5 = 2XEXTDIFF Differential external reference (ADC_SINGLECTRL_REF_2XEXTDIFF)
//          6 = 2XVDD Unbuffered 2xVDD (ADC_SINGLECTRL_REF_2XVDD)
// ADC_SINGLECTRL_AT (Mask = _ADC_SINGLECTRL_AT_MASK)
//          0 = 1 cycle acquisition time (ADC_SINGLECTRL_AT_1CYCLE)
//          1 = 2 cycles acquisition time (ADC_SINGLECTRL_AT_2CYCLES)
//          2 = 4 cycles acquisition time (ADC_SINGLECTRL_AT_4CYCLES)
//          3 = 8 cycles acquisition time (ADC_SINGLECTRL_AT_8CYCLES)
//          4 = 16 cycles acquisition time (ADC_SINGLECTRL_AT_16CYCLES)
//          5 = 32 cycles acquisition time(ADC_SINGLECTRL_AT_32CYCLES)
//          6 = 64 cycles acquisition time(ADC_SINGLECTRL_AT_64CYCLES)
//          7 = 128 cycles acquisition time(ADC_SINGLECTRL_AT_128CYCLES)
//          8 = 256 cycles acquisition time(ADC_SINGLECTRL_AT_256CYCLES)
//
#endif // ADC_H

