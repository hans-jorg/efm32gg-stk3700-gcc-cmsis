/**
 * @file    adc.c
 * @brief   ADC HAL for EFM32GG STK3700
 * @version 1.0
 *
 * @note    This is a HAL for the ADC Converters in the STK3700
 *
 * @note    There is one 12-bit ADC Converter that uses a Successive Approximation
 *          Register technique, reaching up to 1 Mbps at 12 bit, 1.86 Mpbs at 6 bit
 *          conversions.
 *
 * @note    It supports 6/8/12 bit resolution
 *
 * @note    It supports inputs from 8 external pins and 6 internal signals.
 *
 * @note    The clock source must be greater than 32 KHz and less than 13 MHz.
 *
 * @note    A conversion is done in two phases: Acquisition and Conversion.
 *          The total conversion time
 *
 * @note    According AN0021: "The ADC input signals are shielded fairly well against other noisy
 *          signals within the EFM32 Gecko. If high ADC accuracy is needed, it is advisable not
 *          to use any of the unused ADC input pins for noise-inducing activities, such as serial
 *          communication."
 *
 * @note    There is a warm up time after enabling or changing references. 1 us
 *
 * @note    There are many options for reference voltage
 *          * 1.25 V
 *          * 2.5  V. Note VDD > 2.5V
 *          * VDD
 *          * 5 V internal (differential) Note: VDD > 2.75V
 *          * External
 *          * Differential
 *          * Unbuffered VDDx2
 *
 * @note    The default VREF is VDD. It is safer.
 *
 * @note    Only single sample (for now)
 *
 *
 *  @author Hans
 *  @date   30/5/2020
 *
 */

#include <stdint.h>
#include "adc.h"
#include "em_device.h"
#include "clock_efm32gg_ext.h"

// Macros to access memory and registers
#define GETREG32(ADDRESS)    *( (uint32_t *) ADDRESS )
#define GETREG16(ADDRESS)    *( (uint16_t *) ADDRESS )
#define GETREG8(ADDRESS)    *( (uint8_t *) ADDRESS )

// ADC uses pins of GPIO Port D

#define ADC_GPIO    (&(GPIO->P[3]))

// Acquisition and conversion time in ADCCLK cycles
// Tconv = (Tacquisitio+N)*OSR (See RM 28.3.3)
// For VDD/3 the acquisition time is 2 us (See RM 28.3.4 and DS 3.10 Table 3.14)
#define ADC_TACQUISITION      1
#define ADC_TCONVERSION6      7
#define ADC_TCONVERSION8     11
#define ADC_TCONVERSION12    13

/**
 *  Default values for SINGLECTRL register
 */

// Default for external single ended channels (PD0:7)
#define ADC_EXTERNAL_SINGLE_DEFAULT(CH,CY)      ((CH)<<8)||((CY)<<20)           \
                                                    |ADC_SINGLECTRL_REF_VDD     \
                                                    |ADC_SINGLECTRL_RES_12BIT

// Default for external differential channels (Pairs PD0:1, PD2:3. PD4:5, PD6:7)
#define ADC_EXTERNAL_DIFF_DEFAULT(CH,CY)        ((CH)<<8)|((CY)<<20)            \
                                                    |ADC_SINGLECTRL_REF_5VDIFF  \
                                                    |ADC_SINGLECTRL_DIFF        \
                                                    |ADC_SINGLECTRL_RES_12BIT
// Default for internal single ended channels
#define ADC_INTERNAL_SINGLE_DEFAULT(CH,CY)      ((CH)<<8)|((CY)<<20)            \
                                                    |ADC_SINGLECTRL_REF_1V25    \
                                                    |ADC_SINGLECTRL_RES_12BIT

#define SINGLECTRL_REGTABSIZE 21
static const uint32_t default_singlectrl[SINGLECTRL_REGTABSIZE] = {
/* Single-ended signals DIFF=0 */
    /* External signals */
    /* Single Ch 0      */      ADC_EXTERNAL_SINGLE_DEFAULT(0,1),            //  0: ADC_CH0
    /* Single Ch 1      */      ADC_EXTERNAL_SINGLE_DEFAULT(1,1),           //  1:  ADC_CH1
    /* Single Ch 2      */      ADC_EXTERNAL_SINGLE_DEFAULT(2,1),           //  2:  ADC_CH2
    /* Single Ch 3      */      ADC_EXTERNAL_SINGLE_DEFAULT(3,1),           //  3:  ADC_CH3
    /* Single Ch 4      */      ADC_EXTERNAL_SINGLE_DEFAULT(4,1),           //  4:  ADC_CH4
    /* Single Ch 5      */      ADC_EXTERNAL_SINGLE_DEFAULT(5,1),           //  5:  ADC_CH5
    /* Single Ch 6      */      ADC_EXTERNAL_SINGLE_DEFAULT(6,1),           //  6:  ADC_CH6
    /* Single Ch 7      */      ADC_EXTERNAL_SINGLE_DEFAULT(7,1),           //  7:  ADC_CH7
    /* Internal signals */
    /* Single Temp      */      ADC_INTERNAL_SINGLE_DEFAULT(8,2),           //  8:  ADC_TEMP
    /* Single VDD/3     */      ADC_INTERNAL_SINGLE_DEFAULT(9,2),           //  9:  ADC_VDD_3
    /* Single VDD/2     */      ADC_INTERNAL_SINGLE_DEFAULT(10,2),          // 10:  ADC_VDD_2
    /* Single VSS       */      ADC_INTERNAL_SINGLE_DEFAULT(11,2),          // 11:  ADC_VSS
    /* Single VREF/2    */      ADC_INTERNAL_SINGLE_DEFAULT(12,2),          // 12:  ADC_VREF_2
    /* Single DAC_CH0   */      ADC_INTERNAL_SINGLE_DEFAULT(13,2),          // 13:  ADC_DAC0
    /* Single DAC_CH1   */      ADC_INTERNAL_SINGLE_DEFAULT(14,2),          // 14:  ADC_DAC1
    /* Filler           */      0x00000000,                                 // 15: Filler
/* Differential signals DIFF=1 */
    /* Diff Ch 0-1      */      ADC_EXTERNAL_DIFF_DEFAULT(0,1),             // 16:  ADC_DIFF_CH01
    /* Diff Ch 2-3      */      ADC_EXTERNAL_DIFF_DEFAULT(1,1),             // 17:  ADC_DIFF_CH23
    /* Diff Ch 4-5      */      ADC_EXTERNAL_DIFF_DEFAULT(2,1),             // 18:  ADC_DIFF_CH45
    /* Diff Ch 6-7      */      ADC_EXTERNAL_DIFF_DEFAULT(3,1),             // 19:  ADC_DIFF_CH67
    /* Diff 0           */      ADC_EXTERNAL_DIFF_DEFAULT(4,1)              // 20:  ADC_DIFF_0
};
//}

/**
 *  Values of SINGLECTRL register for each channel
 *
 */
static uint32_t singlectrl[SINGLECTRL_REGTABSIZE];

//
// Calibration values from Device Information Page
//

#define ADC0_CAL      0x0FE08040
#define ADC0_BIASPROG 0x0FE08048

static uint32_t adc_cal;            // @ ADC0_CAL
static uint32_t adc_bias;           // @ ADC0_BIASPROG

static uint32_t cal_gain_1v25;      // Gain for 1V25 Reference      @ADC0_CAL_1V25[14:8]
static uint32_t cal_offset_1v25;    // Offset for 1V25 Reference    @ADC0_CAL_1V25[6:0]
static uint32_t cal_gain_2v5;       // Gain for 2V5 Reference       @ADC0_CAL_2V5[14:8]
static uint32_t cal_offset_2v5;     // Offset for 2V5 Reference     @ADC0_CAL_2V5[6:0]
static uint32_t cal_gain_vdd;       // Gain for VDD Reference       @ADC0_CAL_VDD[14:8]
static uint32_t cal_offset_vdd;     // Offset for VDD Reference     @ADC0_CAL_VDD[6:0]
static uint32_t cal_gain_5v;        // Gain for 5V Reference        @ADC0_CAL_5V[14:8]
static uint32_t cal_offset_5v;      // Offset for 5V Reference      @ADC0_CAL_5V[6:0]
static uint32_t cal_gain_2xvdd;     // Gain for 2XVDD Reference     @ADC0_CAL_2XVDD[14:8]
static uint32_t cal_offset_2xvdd;   // Offset for 2XVDD Reference   @ADC0_CAL_2XVDD[6:0]


static void
GetCalibrationValues(void) {
uint32_t v;

    adc_cal  = GETREG32(ADC0_CAL);
    adc_bias = GETREG32(ADC0_BIASPROG);

    v = DEVINFO->ADC0CAL0;          /**< ADC0 Calibration register 0 at 0x0FE081B4 */
    cal_gain_1v25    = (v>>8)&0x7F;
    cal_offset_1v25  = (v>>0)&0x7F;
    cal_gain_2v5     = (v>>24)&0x7F;
    cal_offset_2v5   = (v>>16)&0x7F;

    v = DEVINFO->ADC0CAL1;          /**< ADC0 Calibration register 1 at 0x0FE081B8 */
    cal_gain_vdd     = (v>>8)&0x7F;
    cal_offset_vdd   = (v>>0)&0x7F;
    cal_gain_5v      = (v>>24)&0x7F;
    cal_offset_5v    = (v>>16)&0x7F;

    v = DEVINFO->ADC0CAL2;          /**< ADC0 Calibration register 1 at 0x0FE081BC */
    cal_gain_2xvdd   = (v>>8)&0x7F;
    cal_offset_2xvdd = (v>>0)&0x7F;

}
/**
 * ADC Interrupt Routine
 *
 *
 */


void
ADC0_IRQHandler(void) {


}


/**
 * ADC Initialization
 *
 * Set prescaler and configure unit according parameter
 */
uint32_t ADC_Init(uint32_t adcfreq, uint32_t config) {
const uint32_t adcwarmfreq = 1000000; // 1 MHz for 1 us period
uint32_t presc;

    // If ADC running stop it
    ADC0->CMD = ADC_CMD_SINGLESTOP | ADC_CMD_SCANSTOP;

    // Put adc frequency into limits
    if( adcfreq < ADC_FREQMIN )
        adcfreq = ADC_FREQMIN;
    else if ( adcfreq > ADC_FREQMAX )
        adcfreq = ADC_FREQMAX;

    // Initialize ADC. Calculate TIMEBASE.
    uint32_t freq = ClockGetCoreClockFrequency();

    // Calculate rounded up presc value
    presc = (freq+adcfreq-1)/adcfreq;

    if( freq/presc < ADC_FREQMIN ) {
        presc = freq/ADC_FREQMIN;
    }

    // Put presc in limits
    if( presc < 1 ) {
        presc = 1;
    } else if ( presc > 128 ) {
        presc = 128;
    }

    // Get calibration values from Device Information Page
    GetCalibrationValues();

    // Set fields according config parameter
    for(int i=0;i<SINGLECTRL_REGTABSIZE;i++) {
        singlectrl[i] = default_singlectrl[i];
    }

    // Set clock for peripherals and adc

    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;         // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_ADC0;               // Enable HFPERCKL for GPIO

    uint32_t newctrl = ADC0->CTRL;

    // Set prescaler
    newctrl = (newctrl&~_ADC_CTRL_PRESC_MASK)|((presc-1)<<_ADC_CTRL_PRESC_SHIFT);

    // The TIMEBASE must be set to the number of HFPERCLK which corresponds to
    // at least 1 μs. (See RM 28.3.3)
    uint32_t timebase = (freq+adcwarmfreq-1)/adcwarmfreq;

    if( timebase > 0x1F ) timebase = 0x1F; // maximal value

    newctrl = (newctrl&~_ADC_CTRL_TIMEBASE_MASK)|(timebase<<_ADC_CTRL_TIMEBASE_SHIFT);

    // Set ADC Control register
    ADC0->CTRL = newctrl;


    return 0;
}

/**
 *
 */
uint32_t ADC_ConfigChannel(uint32_t ch, uint32_t config) {
uint32_t shift;

    // For external signal, disable corresponding GPIO pins
    if( ch < 8 ) { // External signals
        shift = ch*4;
        CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO
        ADC_GPIO->MODEL &= ~(0xF<<shift);

    } else if ( ch > 15 ) { // External differential signals
        shift = (ch-0x10)*4;
        CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
        CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCKL for GPIO
        ADC_GPIO->MODEL &= ~(0xFF<<shift);

    }

    // Adjust inputsel field according ch parameter
    config &= ~_ADC_SINGLECTRL_INPUTSEL_MASK;

    if( ch < 16 ) {
        config |= (ch<<_ADC_SINGLECTRL_INPUTSEL_SHIFT);
    } else {
         config |= ((ch-16)<<_ADC_SINGLECTRL_INPUTSEL_SHIFT);
         config |= ADC_SINGLECTRL_DIFF;
    }

    singlectrl[ch] = config;
    return 0;
}

uint32_t ADC_Read(uint32_t ch) {
    (void) ADC_StartReading(ch);
    return ADC_GetReading(ch);
}

uint32_t ADC_StartReading(uint32_t ch) {

    // If ADC running, stop it
    ADC0->CMD = ADC_CMD_SINGLESTOP;

    // Save SINGLECTRL register
    uint32_t oldsinglectrl = ADC0->SINGLECTRL;

    // Configure desired read
    ADC0->SINGLECTRL = singlectrl[ch];

    // Verify if warmup is needed. If so, wait
    if( (ADC0->SINGLECTRL^oldsinglectrl) & _ADC_SINGLECTRL_REF_MASK ) {    // Any difference
        while( (ADC0->STATUS&ADC_STATUS_WARM) == 0 ) {}                   // Beware!! Infinite loop
    }

    // Start reading
    ADC0->CMD = ADC_CMD_SINGLESTART;
    return 0;
}


uint32_t ADC_GetReading(uint32_t ch) {

    // Wait conversion
    while( (ADC0->STATUS&ADC_STATUS_SINGLEACT) != 0 ) {}
    while( (ADC0->STATUS&ADC_STATUS_SINGLEDV) == 0 ) {}

    return ADC0->SINGLEDATA;
}

