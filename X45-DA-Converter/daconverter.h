#ifndef DACONVERTER_H
#define DACONVERTER_H
/**
 * @file    daconverter.h
 * @brief   DA HAL for EFM32GG STK3200
 * @version 1.0
 */
#include <stdint.h>

/**
 * @brief   create a bit mask with bit N set
 */
#ifndef BIT
#define BIT(N) (1U<<(N))
#endif

/**
 * @brief  Maximum sample rate
 */
#define DAC_MAXSAMPLERATE   (500000UL)

/**
 * @brief
 */
///@{
#define DAC_VMAX            (0xFFF)
#define DAC_VMIN            (0)
#define DAC_VDIFFMAX        (DAC_VMAX>>1)
#define DAC_VDIFFMIN        (-DAC_VDIFFMAX)
///@}
// General configuration
#define DAC_CONFIG_ENABLE_CH0            (1<<0)
#define DAC_CONFIG_ENABLE_CH1            (1<<1)
#define DAC_CONFIG_VREF_VDD              (1<<4)
#define DAC_CONFIG_VREF_2V5              (1<<5)
#define DAC_CONFIG_VREF_1V25             (1<<6)
#define DAC_CONFIG_SINGLE_ENDED_OUTPUT   (0)
#define DAC_CONFIG_DIFFERENTIAL_OUTPUT   (1<<9)


// Status
#define DAC_STATUS_CH0_READY       DAC_STATUS_CH0DV
#define DAC_STATUS_CH1_READY       DAC_STATUS_CH1DV

// Enable Channel
#define DAC_CH0                          (1<<0)
#define DAC_CH1                          (1<<1)


int DAC_Init(unsigned config, unsigned samplerate);
int DAC_ConfigureClock(unsigned samplerate);
int DAC_EnableChannels(unsigned bm);
int DAC_DisableChannels(unsigned bm);
unsigned DAC_Status(void);
int DAC_SetOutput(int ch, unsigned vch);
int DAC_SetCombOutput( unsigned vch0, unsigned vch1);
int DAC_SetDifferentialOutput(int v);
int DAC_SetSineOuputMode(void);
int DAC_ClearSineOuputMode(void);
int DAC_SetCallback( void(*)(int) );
int DAC_DisableIRQ(void);
int DAC_EnableIRQ(void);


#endif // DACONVERTER_H
