

/**
 * @file    daconverter.c
 * @brief   DAC HAL for EFM32GG
 * @version 1.0
 */
#include <stdint.h>
#include "em_device.h"
#include "clock_efm32gg_ext.h"
#include "devinfo.h"
#include "daconverter.h"

/**
 * Configuration parameters
 */
#define DAC_MODE            (0)
#define DAC_BIASPROG        (7)


/**
 *  @brief  Pointer to call backfunction
 */
static void (*DAC_Callback)(int ch) = 0;


/**
 *  @brief  Pointer to call backfunction
 *
 *  @note   bit vector. Bit mask os (1<<channel_nr)
 */
static uint32_t enabled_chs = 0;

/**
 *  @brief IRQ Handler for DAC0
 */
void DAC0_IRQHandler(void) {

    int ch = 0;
    if( DAC0->IF&DAC_IF_CH0) ch |= DAC_CH0;
    if( DAC0->IF&DAC_IF_CH0) ch |= DAC_CH1;

    if( DAC_Callback ) DAC_Callback(ch);
    // Clear interrupt.
    DAC0->IFC = DAC_IFC_CH0|DAC_IFC_CH1;
}


/**
 *  @brief      DAC_DisableChannels
 *
 *  @param     unsingmed mapped as a bitvector. Bit 0 corresponds to CH0 and bit 1, to CH1
 */
int DAC_DisableChannels(unsigned bm) {

    if( (bm&DAC_CH0)!=0 ) {
        DAC0->CH0CTRL &= ~DAC_CH0CTRL_EN;
    }
    if( (bm&DAC_CH1)!=0 ) {
        DAC0->CH0CTRL &= ~DAC_CH1CTRL_EN;
    }
    enabled_chs = 0;

    return 0;
}


/**
 *  @brief      DAC_EnableChannels
 *
 *  @param     unsingmed mapped as a bitvector. Bit 0 corresponds to CH0 and bit 1, to CH1
 */
int DAC_EnableChannels(unsigned bm) {

    if( (bm&DAC_CH0)!=0 ) {
        DAC0->CH0CTRL |= DAC_CH0CTRL_EN;
        enabled_chs = DAC_CH0;
    }
    if( (bm&DAC_CH0)!=0 ) {
        DAC0->CH0CTRL |= DAC_CH0CTRL_EN;
        enabled_chs = DAC_CH1;
    }
    return 0;
}

/**
 *  @brief      DAC_Callback
 *
 *  @param      config:
 *  @param      samplerate: desired sample rate
 *
 *  @returns    0=OK, other value in case of error
 *
 */
int DAC_Init(unsigned config, unsigned samplerate ) {
uint32_t chs = 0;

    // At least, one channel must be enabled
    if( !(config&(DAC_CONFIG_ENABLE_CH0|DAC_CONFIG_ENABLE_CH1)) ) {
        return -1;
    }

    if( samplerate > DAC_MAXSAMPLERATE ) {
        samplerate = DAC_MAXSAMPLERATE;
    }
    /* Enable Clock for GPIO */
    CMU->HFPERCLKDIV |= CMU_HFPERCLKDIV_HFPERCLKEN;     // Enable HFPERCLK
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_DAC0;           // Enable HFPERCKL for DAC
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;           // Enable HFPERCLK for GPIO

    // Just in case, disable channels
    DAC0->CH0CTRL &= ~(DAC_CH0CTRL_EN);
    DAC0->CH1CTRL &= ~(DAC_CH1CTRL_EN);
    enabled_chs = 0;

    // Restore default for Control register
    DAC0->CTRL = _DAC_CTRL_RESETVALUE;

    // Verify which channels will be used
    if ( (config&DAC_CONFIG_ENABLE_CH0) != 0 )
        chs |= DAC_CH0;
    if ( (config&DAC_CONFIG_ENABLE_CH1) != 0 )
        chs |= DAC_CH1;

    // Differential output needs both channels
    if( config&DAC_CONFIG_DIFFERENTIAL_OUTPUT ) {
        // Both channels must be used
        if( chs != (DAC_CH0|DAC_CH1) ) {
            return -2;
        }
    }


    // Configure clock prescaler. It modifies the CTRL register
    int rc = DAC_ConfigureClock(samplerate);
    if( rc < 0 )
        return -3;

    uint32_t dacctrl = _DAC_CTRL_RESETVALUE;
    uint32_t cal     =  DI_GetValue32(DI_DAC0_CAL);      // or_DAC_CAL_RESETVALUE;
    uint32_t biasprog=  DI_GetValue32(DI_DAC0_BIASPROG); // or _DAC_BIASPROG_RESETVALUE;

    dacctrl &= ~(  _DAC_CTRL_CONVMODE_MASK
                  |_DAC_CTRL_DIFF_MASK
                  |_DAC_CTRL_OUTMODE_MASK
                  |_DAC_CTRL_REFSEL_MASK);

    // Not sure about the correctness of the DI struct in emf32gg990f1024.h

    if( config&DAC_CONFIG_VREF_VDD ) {
        dacctrl |= _DAC_CTRL_REFSEL_VDD;
        cal      =  DI_GetValue32(DI_DAC0_CAL_VDD);  // DEVINFO->DAC0CAL2;
    } else if( config&DAC_CONFIG_VREF_2V5 ) {
        dacctrl |= _DAC_CTRL_REFSEL_2V5;
        cal      =  DI_GetValue32(DI_DAC0_CAL_2V5);  // DEVINFO->DAC0CAL1;
    } else if( config&DAC_CONFIG_VREF_1V25 ) {
        dacctrl |= _DAC_CTRL_REFSEL_1V25;
        cal      =  DI_GetValue32(DI_DAC0_CAL_1V25); // DEVINFO->DAC0CAL0;
    } else {
        return -4;
    }

    if( config&DAC_CONFIG_DIFFERENTIAL_OUTPUT ) {
        dacctrl |= DAC_CTRL_DIFF;
    }

    // Set conversion mode to continuous
    dacctrl |= DAC_CTRL_CONVMODE_CONTINUOUS;

    // Set output mode. For now, only output in the standard pin
    dacctrl |= DAC_CTRL_OUTMODE_PIN;


    // set control and calibration registers
    DAC0->CTRL = dacctrl;
    DAC0->CAL  = cal;
    DAC0->BIASPROG = biasprog;

    // Enable channels
    DAC_EnableChannels(chs);

    return 0;
}


/**
 *  @brief      DAC_Status
 *
 *  @note       Get status of DA converter.
 *
 *  @note       The meaning of the returned value are 0 when busy
 *              and 1 when ready, ie, data can be written.
 */
unsigned DAC_Status(void) {
uint32_t w = DAC0->STATUS;

    uint32_t chs = 0;
    if( (w&DAC_STATUS_CH0DV) == 0 ) {
        chs |= DAC_CH0;
    }
    if( (w&DAC_STATUS_CH1DV) == 0 ) {
        chs |= DAC_CH1;
    }

    return chs;
}


/**
 *  @brief      DAC_ReconfigureClock
 *
 *  @param      samplerate
 *
 *  @returns    0=OK, other value in case of error
 *
 *  @note       Sample rate is limited to 500 Ksps, that
 *              means a clock of 1 MHz for the DAC
 */
int DAC_ConfigureClock(unsigned samplerate) {

   if( samplerate > DAC_MAXSAMPLERATE )
        return -1;

    uint32_t daclkfreq = 2*samplerate;

    uint32_t hfperclk = ClockGetPeripheralClockFrequency();

    uint32_t div = (hfperclk-daclkfreq-1)/daclkfreq;

    uint32_t presc = 0;
    while( (1<<presc) < div ) presc++;

    if( presc > 7 )
        return -2;

    DAC0->CTRL = ((DAC0->CTRL)&~(_DAC_CTRL_PRESC_MASK))
                |((presc)<_DAC_CTRL_PRESC_SHIFT);

    return 0;
}


/**
 *  @brief      DAC_EnableOutputs
 *
 *  @param      Bit mask specifying which output is to be enabled
 *
 *  @returns
 *
 */



/**
 *  @brief      DAC_SetOutput
 *
 *  @param      ch: channel number 0 or 1
 *  @param      v:  12-bit value

 *  @returns
 *
 */
int DAC_SetOutput(int ch, unsigned v) {

    v &= 0xFFF; // only 12 bits
    if( ch == 0 )
        DAC0->CH0DATA = v;
    if( ch == 1 )
        DAC0->CH1DATA = v;
    return 0;
}

/**
 *  @brief      DAC_SetOutput
 *
 *  @param      vch0: 12-bit value for channel number 0
 *  @param      vch1: 12-bit value for channel number 1

 *  @returns    always 0
 *
 */
int DAC_SetCombOutput( unsigned vch0, unsigned vch1) {

    vch0 &= 0xFFF;
    vch1 &= 0xFFF;

    DAC0->COMBDATA = (vch0<<16)|vch1;
    return 0;
}
/**
 *  @brief      DAC_SetDifferentialOutput
 *
 *  @param      v: value of the desired output
 *                 in range -2047 to +2048
 *
 *  @returns    always 0
 *
 */
int DAC_SetDifferentialOutput(int v) {

    v &= 0xFFF; // only 11 bits plus sign
    DAC0->CH0DATA = v;

    return 0;
}


/**
 *  @brief      DAC_SetSineOuput
 *
 *  @returns    always 0
 *
 */
int DAC_SetSineOuput(void) {

    DAC0->CTRL |= DAC_CTRL_SINEMODE;
    return 0;
}

/**
 *  @brief      DAC_ClearSineOuput
 *
 *  @returns    always 0
 *
 */
int DAC_ClearSineOuput(void) {

    DAC0->CTRL &= ~DAC_CTRL_SINEMODE;
    return 0;
}

/**
 *  @brief      DAC_SetCallback
 *
 *  @param      func: function to be called in case of interrupt
 *
 *  @returns    always 0
 *
 */
int DAC_SetCallback( void(*func)(int)) {

    DAC_Callback = func;
    return 0;
}


/**
 *  @brief      DAC_DisableIRQ
 *
 *  @returns    always 0
 *
 */
int DAC_DisableIRQ(void) {

    NVIC_DisableIRQ(DAC0_IRQn);
    return 0;
}


/**
 *  @brief      DAC_EnableIRQ
 *
 *  @returns    always 0
 *
 */
int DAC_EnableIRQ(void) {

    DAC0->IEN |= (DAC_IEN_CH0|DAC_IEN_CH1);
    NVIC_ClearPendingIRQ(DAC0_IRQn);
    NVIC_EnableIRQ(DAC0_IRQn);
    return 0;
}


/**
 *  @brief      DAC_DisableChannel
 *
 *  @param      ch: channel number 0 or 1
 *
 *  @returns    Always 0
 *
 */
int DAC_DisableChannel(int ch) {

    if( ch == 0 ) DAC0->CH0CTRL &= ~(DAC_CH0CTRL_EN);
    if( ch == 1 ) DAC0->CH1CTRL &= ~(DAC_CH1CTRL_EN);
    enabled_chs = 0;
    return 0;
}


/**
 *  @brief      DAC_EnableChannel
 *
 *  @param      ch: channel number 0 or 1
 *
 *  @returns    Always 0
 *
 */
int DAC_EnableChannel(int ch) {

    if( ch == 0 ) {
        DAC0->CH0CTRL |= DAC_CH0CTRL_EN;
        enabled_chs |= DAC_CH0;
    }
    if( ch == 1 ) {
        DAC0->CH1CTRL |= DAC_CH1CTRL_EN;
        enabled_chs |= DAC_CH1;
    }

    return 0;
}
