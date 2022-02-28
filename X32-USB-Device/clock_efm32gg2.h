#ifndef  SYSTEM_EFM32GG_EXT_H
#define  SYSTEM_EFM32GG_EXT_H
/** ***************************************************************************
 * @file     system_efm32gg-ext.h
 * @brief    Additional CMSIS-like routines for Cortex-M3 System Layer for
 *           EFM32GG devices.
 * @version  1.0
 * @author   Hans
 *
 *****************************************************************************/


/**
 * @brief       Clock Source
 *
 * @note        Used by SystemCoreClockSet and ClockGetConfiguration
 */
typedef enum {  CLOCK_NONE=0,
                CLOCK_LFXO,             ///< Low Frequency Crystal Oscillator: 32768 Hz
                CLOCK_LFRCO,            ///< Low Frequency Internal RC Oscillator: 32768 Hz
                CLOCK_HFRCO_1MHZ,       ///< High Frequency Internal RC Oscillator: 1 MHz
                CLOCK_HFRCO_7MHZ,       ///< High Frequency Internal RC Oscillator: 7 or 6.6 MHz
                CLOCK_HFRCO_11MHZ,      ///< High Frequency Internal RC Oscillator: 11 MHz
                CLOCK_HFRCO_14MHZ,      ///< High Frequency Internal RC Oscillator: 14 MHz (default)
                CLOCK_HFRCO_21MHZ,      ///< High Frequency Internal RC Oscillator: 21 MHz
                CLOCK_HFRCO_28MHZ,      ///< High Frequency Internal RC Oscillator: 28 MHz
                CLOCK_HFXO,             ///< High Frequency Crystall Oscillator: 48 MHz (STK3700)
                // Below only for Low Frequency Clock A and B
                CLOCK_ULFRCO,            ///< Ultra Low Frequency
                CLOCK_HFCORECLOCK_2      ///< HF Core Clock divided by 2
             }  ClockSource_t;


#define CLOCK_CHANGED_HFCLK         0x00001
#define CLOCK_CHANGED_HFCORECLK     0x00002
#define CLOCK_CHANGED_HFPERCLK      0x00004
#define CLOCK_CHANGED_HFCORECLKLE   0x00008
#define CLOCK_CHANGED_LFCLKA        0x00010
#define CLOCK_CHANGED_LFCLKB        0x00020

/**
 * @brief       Clock Configuration Structure
 *
 * @note        Used by ClockGetConfiguration
 * @note        Will be used by ClockSetConfiguration
 */
typedef struct {
                ///@{
                /* Configuration info. Used by ClockSetConfiguration */
                ClockSource_t   source;             ///< HFCLK clock source
                uint32_t        basefreq;           ///< Base frequency of core clock source
                uint32_t        hclkdiv;            ///< Divisor of base frequency to generate HFCLK
                uint32_t        hfcoreclkdiv;       ///< Divisor of HFCLK to generate Core Clock
                uint32_t        hfperclkdiv;        ///< Divisor of HFCLK to generate Peripheral Clock
                uint32_t        hfperclkdivcode;    ///< Encoding of Peripheral Clock divisor
                uint32_t        hfcoreclkdivcode;   ///< Encoding of Core Clock divisor
                uint32_t        hfcoreclocklediv;   ///< Div = 2 or 4
                ///@}
                ///@{
                /* For info only. Not used by (future) ClockSetConfiguration  */
                uint32_t        hclkfreq;           ///@<  =HFCLK/hclkdiv
                uint32_t        hfcoreclkfreq;      ///@<  =HFCLK/hclkdiv/corediv
                uint32_t        hfperclkfreq;       ///@<  =HFCLK/hclkdiv/perdiv
                uint32_t        hfcoreclklefreq;    ///@<
                ///@}
} ClockConfiguration_t;


// Set core clock frequency
uint32_t ClockSetCoreClock(ClockSource_t source, uint32_t hclkdiv, uint32_t corediv);

// Register a function to be called when the clock changes
int      ClockRegisterCallback( uint32_t clock, void (*pre)(uint32_t), void (*post)(uint32_t));

// Configure System (*Wait states, etc) for a specified clock frequency
uint32_t ClockConfigureSystemForClockFrequency(uint32_t freq);

// Configure LFCLK{A,B}
uint32_t ClockSetLFCLKA(ClockSource_t source);
uint32_t ClockSetLFCLKB(ClockSource_t source);

// Configure prescalers (0 value means do not change it))
uint32_t ClockSetPrescalers(uint32_t hclkdiv, uint32_t corediv, uint32_t perdiv, uint32_t coreclklediv);

// Get the clock configuration in a struct
uint32_t ClockGetConfiguration(ClockConfiguration_t *p);

// Get Peripheral and Core Clock frequency (GetCoreClockFrequency()==SystemCoreClock)
uint32_t ClockGetPeripheralClockFrequency(void);
uint32_t ClockGetCoreClockFrequency(void); // Should return the same as SystemCoreCLock


/** @brief Aliases using names used in Reference Manual */
//{
#define ClockSetHFCORECLK ClockSetCoreClock
#define ClockGetHFCORECLK ClockGetCoreClockFrequency
#define ClockGetHFPERCLK  ClockGetPeripheralClockFrequency
//}

/** @brief Alias for compatibility with older version of this module */
#define SystemCoreClockSet ClockSetCoreClock


#endif //SYSTEM_EFM32GG_EXT_H
