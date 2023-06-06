/** 
 * @file    clock_efm32gg.c
 * @brief   Clock management routines for for EFM32GG devices.
 *
 * @note    CMSIS-like routines
 *
 * @note    replace system_efm32-ext.c
 *
 * @note    Uses em_device.h. So it is possible to define which processor
 *          by using the compiler command line, e.g. -DEFM32GG995F1024
 *          The alternative would be the inclusion of the processor specific file
 *          directly, e.g. #include "efm32gg995f1024.h"
 *
 *          
 * @version 1.0
 * @author  Hans
 */

#include <stdint.h>
#include "em_device.h"
#include "clock_efm32gg.h"

/**
 * @function        GetProdRev
 * @brief           Inline function to get the chip's Production Revision.
 *
 * @note            From Silicon Labs system_efm32gg.c. It is a static (internal)
 *                  function there.
 */
__STATIC_INLINE uint8_t GetProdRev(void)
{
  return ((DEVINFO->PART & _DEVINFO_PART_PROD_REV_MASK)
                         >> _DEVINFO_PART_PROD_REV_SHIFT);
}

/**
 * @note            If Symbol for Crystal frequency not defined, define it.
 * @note            Just in case.
 */
#ifndef EFM32_HFXO_FREQ
#define EFM32_HFXO_FREQ (48000000UL)
#endif

/**
 * @macro   Get byte from memory
 *
 * @note    BASE must be a pointer or an integer
 */
 
#define GETBYTEFROMWORD(BASE,POS)     *((uint8_t *) (BASE) +(POS))

/**
 * @function   nearestpower2exp
 * @brief      find the nearest exponent of 2 next to a given integer
 *
 * @note       to find the power of 2, use 1<<result
 */
 
static uint32_t
nearestpower2exp(uint32_t n) {
uint32_t w = 1;
uint32_t e = 0;
uint32_t err1=n;
uint32_t err2;


    while( 1 ) {
        err2 = err1;
        if( w > n ) err1 = w - n;
        else        err1 = n - w;
        if ( err2 < err1 )
            break;
       w <<= 1;
       e++;
        if ( w == 0 )
            break;
    }
    if( e > 0 ) return e-1;
    else        return 0;
}

/**
 * @brief   Set the clock frequency and source
 *
 * @note    This function is provided to make it easier to configure clock frequency
 *          and source.
 *
 * @note    There are two main clock signals in a EMF32GG: HFPERCLK and HFCORECLK
 *          Both are derived from a HFCLK signal.
 *
 * @note    This function configures the HFCLK
 *
 * @note    This is not part of the CMSIS definition.
 *
 * @note    Take care of HCLKLE. It must be less than 16 MHz
 *
 * @note    It sets the prescaler of HFCORECLOCK and HFPERCLOCK to 1
 *
 * @param[in] source
 *   CLOCK_LFXO, CLOCK_LFRC, CLOCK_HFRCO, CLOCK_HFXO,
 * @param[in] freq
 *   LFXO frequency in Hz used for target.
 */

uint32_t
SystemCoreClockSet(ClockSource_t source, uint32_t hclkdiv, uint32_t corediv) {
    uint32_t basefreq;
    uint32_t hclkfreq;
    uint8_t  tuning;
    uint32_t band;
    uint32_t divcode;

    // Put hclkdiv in valid range
    if ( hclkdiv > 8 ) hclkdiv = 8;
    if ( hclkdiv < 1 ) hclkdiv = 1;

    // Prepare for worst case
    ClockConfigureForFrequency(EFM32_HFXO_FREQ);

    // Set HFCLK divisor to 1
    CMU->CTRL      = (CMU->CTRL&~(_CMU_CTRL_HFCLKDIV_MASK));

    band = 0;
    tuning = 0;
    switch(source) {
    case CLOCK_HFRCO_1MHZ:
        if ( GetProdRev() >= 19 )
          basefreq = 1200000;
        else
          basefreq = 1000000;
        tuning   = GETBYTEFROMWORD(DEVINFO->HFRCOCAL0, 0);
        band     = CMU_HFRCOCTRL_BAND_1MHZ;
        hclkfreq = basefreq/hclkdiv;
        break;

    case CLOCK_HFRCO_7MHZ:
        if ( GetProdRev() >= 19 )
          basefreq = 6600000;   // 6.6 MHz
        else
          basefreq = 7000000;   // 7.0 MHz
        tuning   = GETBYTEFROMWORD(DEVINFO->HFRCOCAL0, 1);
        band     = CMU_HFRCOCTRL_BAND_7MHZ;
        hclkfreq = basefreq/hclkdiv;
        break;

    case CLOCK_HFRCO_11MHZ:
        basefreq = 11000000; // 11 MHz
        tuning   = GETBYTEFROMWORD(DEVINFO->HFRCOCAL0, 2);
        band     = CMU_HFRCOCTRL_BAND_11MHZ;
        hclkfreq = basefreq/hclkdiv;
        break;

    case CLOCK_HFRCO_14MHZ:
        basefreq = 14000000; // 14 MHz
        tuning   = GETBYTEFROMWORD(DEVINFO->HFRCOCAL0, 3);
        band     = CMU_HFRCOCTRL_BAND_14MHZ;
        hclkfreq = basefreq/hclkdiv;
        break;

    case CLOCK_HFRCO_21MHZ:
        basefreq = 21000000; // 21 MHz
        tuning   = GETBYTEFROMWORD(DEVINFO->HFRCOCAL1, 0);
        band     = CMU_HFRCOCTRL_BAND_21MHZ;
        hclkfreq = basefreq/hclkdiv;
        break;

    case CLOCK_HFRCO_28MHZ:
        basefreq = 28000000; // 28 MHz
        tuning   = GETBYTEFROMWORD(DEVINFO->HFRCOCAL1, 1);
        band     = CMU_HFRCOCTRL_BAND_28MHZ;
        hclkfreq = basefreq/hclkdiv;
        break;

    case CLOCK_LFRCO:
        basefreq = 32768; // 32768 Hz
        // If LFRCO is NOT enabled, enable it
        if ( (CMU->STATUS&CMU_STATUS_LFRCOENS) == 0 ) {
            CMU->OSCENCMD  = CMU_OSCENCMD_LFRCOEN;
        }
        hclkfreq = basefreq / hclkdiv;
        break;

    case CLOCK_LFXO:
        basefreq = 32768; // 32768 Hz
        // If LFXO is NOT enabled, enable it
        if ( (CMU->STATUS&CMU_STATUS_LFXOENS) == 0 ) {
            CMU->OSCENCMD  = CMU_OSCENCMD_LFXOEN;
        }
        hclkfreq = basefreq / hclkdiv;
        break;

    case CLOCK_HFXO:
        basefreq = EFM32_HFXO_FREQ;
        // Check if LFXO is enable. If not, enable it
        if ( (CMU->STATUS&CMU_STATUS_HFXOENS) == 0 ) {
            CMU->OSCENCMD  = CMU_OSCENCMD_HFXOEN;
            // wait until ready. should test timeout
            while( (CMU->STATUS&CMU_STATUS_HFXORDY) == 0 ) {}
        }
        hclkfreq = basefreq / hclkdiv;
        break;

    default:
        return 0;
    }

    // Adjust for frequency: Flash wait states, etc


    switch(source) {
    case CLOCK_HFRCO_1MHZ:
    case CLOCK_HFRCO_7MHZ:
    case CLOCK_HFRCO_11MHZ:
    case CLOCK_HFRCO_14MHZ:
    case CLOCK_HFRCO_21MHZ:
    case CLOCK_HFRCO_28MHZ:
        // Configure
        CMU->HFRCOCTRL = band|tuning;

        // Wait until ready. should test timeout
        while( (CMU->STATUS&CMU_STATUS_HFRCORDY) == 0 ) {}

        // Select HFRCO as source for HFCLK
        CMU->CMD = CMU_CMD_HFCLKSEL_HFRCO;
        break;

    case CLOCK_LFRCO:
        // Wait until ready. should test timeout
        while( (CMU->STATUS&CMU_STATUS_LFRCORDY) == 0 ) {}

        // Select LFRCO as source for HFCLK
        CMU->CMD = CMU_CMD_HFCLKSEL_LFRCO;
        break;

    case CLOCK_LFXO:
        // Wait until ready. should test timeout
        while( (CMU->STATUS&CMU_STATUS_LFXORDY) == 0 ) {}

        // Select LFRCO as source for HFCLK
        CMU->CMD = CMU_CMD_HFCLKSEL_LFXO;
        break;

    case CLOCK_HFXO:
        // Wait until ready. should test timeout
        while( (CMU->STATUS&CMU_STATUS_HFXORDY) == 0 ) {}

        // Select HFXO as source for HFCLK
        CMU->CMD = CMU_CMD_HFCLKSEL_HFXO;
        break;
    default:
        return 0;
    }

    /*
     * Set HFCLK divisor to give value
     */

    // HFCLK divisor encoded into a 0 to 7 range
    hclkdiv--;

    // Set divisor in CTRL register
    CMU->CTRL = (CMU->CTRL&~(_CMU_CTRL_HFCLKDIV_MASK))| (hclkdiv<<_CMU_CTRL_HFCLKDIV_SHIFT);


    /*
     * Set Core Clock (HFCORECLK) and HF Peripheral Clock (HFPERCLK) to given value
     */

    // Divisors are encoded as a power of 2 exponent

    divcode = nearestpower2exp(corediv);

    // Set divisors in HFCORECLKDIV and HFPERCLKDIV registers
    CMU->HFCORECLKDIV =  ( CMU->HFCORECLKDIV&~_CMU_HFCORECLKDIV_HFCORECLKDIV_MASK)
                        |(divcode<<_CMU_HFCORECLKDIV_HFCORECLKDIV_SHIFT);
    CMU->HFPERCLKDIV =   ( CMU->HFPERCLKDIV& ~_CMU_HFPERCLKDIV_HFPERCLKDIV_MASK)
                        |(divcode<<_CMU_HFPERCLKDIV_HFPERCLKDIV_SHIFT);

    /*
     * Update global SystemCoreClock variable
     */
    SystemCoreClockUpdate();

    /*
     * Optimize configuration (Flash wait states, etc) for set clock frequency
     */
    ClockConfigureForFrequency(SystemCoreClock);

    /*
     *
     */
    return hclkfreq/(1<<divcode);
}


/**
 * @brief   Get Clock Configuration in a ClockConfiguration_t structure
 *
 * @note    It returns the clock source, the basefreq, the divisor for HCLK,
 *          the HCLK frequency, the core frequency., the divisor for the core
 *          frequency, the Peripheral Clock Frequency and the divisor used for
 *          peripheral frequency
 *
 * @note    It returns the base frequency
 */

uint32_t
ClockGetConfiguration(ClockConfiguration_t *p) {
uint32_t basefreq = 0;
uint32_t status;
uint32_t hclkfreq,hclkdivcode;
uint32_t source = 0;
uint32_t corefreq,coredivcode;
uint32_t perfreq,perdiv;

    status = CMU->STATUS;
    hclkdivcode    = (CMU->CTRL&~(_CMU_CTRL_HFCLKDIV_MASK))>>_CMU_CTRL_HFCLKDIV_SHIFT;

    if( status&CMU_STATUS_HFRCOSEL) {
        // HFRCO selected. See MODE field in CMU_HFRCOCTRL to see the frequency
        switch( CMU->HFRCOCTRL&_CMU_HFRCOCTRL_BAND_MASK ) {
        case _CMU_HFRCOCTRL_BAND_1MHZ:
            if ( GetProdRev() >= 19 )
                basefreq = 1200000;
            else
                basefreq = 1000000;
            source   = CLOCK_HFRCO_1MHZ;
            break;
        case _CMU_HFRCOCTRL_BAND_7MHZ:
            if ( GetProdRev() >= 19 )
                basefreq = 6600000;
            else
                basefreq = 7000000L;
            source   = CLOCK_HFRCO_7MHZ;
            break;
        case _CMU_HFRCOCTRL_BAND_11MHZ:
            basefreq = 11000000L;
            source   = CLOCK_HFRCO_11MHZ;
            break;
        case _CMU_HFRCOCTRL_BAND_14MHZ:
            basefreq = 14000000L;
            source   = CLOCK_HFRCO_14MHZ;
            break;
        case _CMU_HFRCOCTRL_BAND_21MHZ:
            basefreq = 21000000L;
            source   = CLOCK_HFRCO_21MHZ;
            break;
        case _CMU_HFRCOCTRL_BAND_28MHZ:
            basefreq = 28000000L;
            source   = CLOCK_HFRCO_28MHZ;
            break;
        }
    } else if ( status&CMU_STATUS_LFRCOSEL ) {
        basefreq = 32768UL;
        source   = CLOCK_LFRCO;
    } else if ( status&CMU_STATUS_LFXOSEL ) {
        basefreq = 32768UL;
        source   = CLOCK_LFXO;
    } else if ( status&CMU_STATUS_HFXOSEL ) {
        basefreq = EFM32_HFXO_FREQ;
        source   = CLOCK_HFXO;
    } else {
        source   = CLOCK_NONE;
    }

    hclkfreq = basefreq/(hclkdivcode+1);
    coredivcode  = (CMU->HFCORECLKDIV&_CMU_HFCORECLKDIV_HFCORECLKDIV_MASK)
                >>_CMU_HFCORECLKDIV_HFCORECLKDIV_SHIFT;
    corefreq = hclkfreq/(1U<<coredivcode);
    perdiv   = (CMU->HFPERCLKDIV&_CMU_HFPERCLKDIV_HFPERCLKDIV_MASK)
                >>_CMU_HFPERCLKDIV_HFPERCLKDIV_SHIFT;
    perfreq  = hclkfreq/(1U<<perdiv);

    if( p ) {
        p->source   = source;
        p->basefreq = basefreq;
        p->hclkdiv  = hclkdivcode+1;
        p->hclkfreq = hclkfreq;
        p->corefreq = corefreq;
        p->corediv  = 1U<<coredivcode;
        p->perfreq  = perfreq;
        p->perdiv   = 1U<<perdiv;
    }

    return basefreq;
}


/*
 * @brief   Configure the other units for the desired clock frequency
 *
 * @note    See 11.3.4 Configuration For Operating Frequencies
 *
 * @note    It adjusts Flash wait states, HFCLKE and Boost
 *
 * @note
 *  Clock       Flash wait states                       HFCLKLE < 16
 *  16 MHz      WS0/WS0SCBTP/WS1/WS1SCBTP/WS2/WS2SCBTP  BOOSTUPTO32MHZ (1)
 *  32 MHz      WS1/WS1SCBTP/WS2/WS2SCBTP               BOOSTUPTO32MHZ (1)
 *  48 MHz      WS2/WS2SCBTP                            HFCORECLKLEDIV(2)  BOOSTABOVE32MHZ
 *
 *  @note   Default is BOOSTUPTO32MHZ in CMU_CTRL
 *
 * @note    You can set HFCORECLKLEDIV in CMU_HFCORECLKDIV or HFLE in CMU_CTRL
 */

uint32_t
ClockConfigureForFrequency(uint32_t freq) {
uint32_t newreadctrl;
uint32_t newctrl;

    newreadctrl  = MSC->READCTRL;
    newctrl      = CMU->CTRL;
    // Clear fields
    newreadctrl     &= ~(MSC_READCTRL_MODE_WS0|MSC_READCTRL_MODE_WS1
                    |   MSC_READCTRL_MODE_WS2
                    |   MSC_READCTRL_MODE_WS0SCBTP|MSC_READCTRL_MODE_WS1SCBTP
                    |   MSC_READCTRL_MODE_WS2SCBTP);
    newctrl         &= ~(_CMU_CTRL_HFXOBUFCUR_MASK);

    // Set fields
    if( freq <= 16000000UL ) {
        newreadctrl |= (MSC_READCTRL_MODE_WS0|MSC_READCTRL_MODE_WS1|MSC_READCTRL_MODE_WS2
                    |   MSC_READCTRL_MODE_WS0SCBTP|MSC_READCTRL_MODE_WS1SCBTP
                    |   MSC_READCTRL_MODE_WS2SCBTP);
        newctrl     |= CMU_CTRL_HFXOBUFCUR_BOOSTUPTO32MHZ;
    } else if ( freq <= 32000000UL ) {
        newreadctrl |= (MSC_READCTRL_MODE_WS1|MSC_READCTRL_MODE_WS2
                    |   MSC_READCTRL_MODE_WS1SCBTP|MSC_READCTRL_MODE_WS2SCBTP);
        newctrl     |= CMU_CTRL_HFXOBUFCUR_BOOSTUPTO32MHZ;
    } else { // Maximum is 48 MHz
        newreadctrl |= (MSC_READCTRL_MODE_WS2|MSC_READCTRL_MODE_WS2SCBTP);
        newctrl     |= CMU_CTRL_HFLE;
        newctrl     |= CMU_CTRL_HFXOBUFCUR_BOOSTABOVE32MHZ;
    }
    MSC->READCTRL = newreadctrl;
    CMU->CTRL     = newctrl;

    return freq;
}


/*
 * @brief   Set the HF Clock divisor considering the limits
 *          to
 * @note    It configure first to the HFXO and then to the desired
 *          frequency
 *
 */
 
uint32_t
ClockSetHFClockDivisor(uint32_t div) {

    // Put div in valid range
    if ( div > 8 ) div = 8;
    if ( div < 1 ) div = 1;

    // Configure for worst case
    ClockConfigureForFrequency(EFM32_HFXO_FREQ);

    /* Set HFCLK divisor to give value */
    div--;
    CMU->CTRL      = (CMU->CTRL&~(_CMU_CTRL_HFCLKDIV_MASK))| (div<<_CMU_CTRL_HFCLKDIV_SHIFT);

    // Update global SystemCoreClock variable
    SystemCoreClockUpdate();

    // Optime for set clock frequency
    ClockConfigureForFrequency(SystemCoreClock);

    return SystemCoreClock;
}


/**
 * @brief       Change prescalers for Core and Peripheral Clock
 *
 * @param       corediv: prescaler for HFCORECLOCK
 * @param       perdiv:  prescaler for HFPERCLOCK
 *
 * @note        Prescalers must be a power of 2 in the range [1..512]. If it is not,
 *              it will be rounded
 *
 */
 
uint32_t
ClockSetPrescalers(uint32_t corediv, uint32_t perdiv) {
uint32_t c,p;
const uint32_t COREDIVMASK = (_CMU_HFCORECLKDIV_HFCORECLKDIV_MASK);
const uint32_t PERDIVMASK  = (_CMU_HFPERCLKDIV_HFPERCLKDIV_MASK);

    // Configure for worst case
    ClockConfigureForFrequency(EFM32_HFXO_FREQ);

    if( corediv == 0 ) corediv = 1;
    if( perdiv  == 0 ) perdiv  = 1;

    c = nearestpower2exp(corediv);
    if( c == 0 ) c = 1;
    if( c > 9  ) c = 9;

    p = nearestpower2exp(perdiv);
    if( p == 0 ) p = 1;
    if( p > 9  ) p = 9;

    // Configure for new frequency

    CMU->HFCORECLKDIV = (CMU->HFCORECLKDIV&~COREDIVMASK)|(c<<_CMU_HFCORECLKDIV_HFCORECLKDIV_SHIFT);
    CMU->HFPERCLKDIV  = (CMU->HFPERCLKDIV&~PERDIVMASK)|(p<<_CMU_HFPERCLKDIV_HFPERCLKDIV_SHIFT);

    // Update global SystemCoreClock variable
    SystemCoreClockUpdate();

    // Optime for set clock frequency
    ClockConfigureForFrequency(SystemCoreClock);
    
    return SystemCoreClock;
}

/*
 * @brief   Returns the Peripheral Clock frequency
 *
 * @note    Uses ClockGetConfiguration
 */

uint32_t
ClockGetPeripheralClockFrequency(void) {
ClockConfiguration_t clockconf;

    ClockGetConfiguration(&clockconf);

    return clockconf.perfreq;

}


/*
 * @brief   Returns the Core Clock frequency
 *
 * @note    Uses ClockGetConfiguration
 */

uint32_t
ClockGetCoreClockFrequency(void) {
ClockConfiguration_t clockconf;

    ClockGetConfiguration(&clockconf);

    return clockconf.corefreq;

}

