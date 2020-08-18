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
                CLOCK_HFXO              ///< High Frequency Crystall Oscillator: 48 MHz (STK3700)
             }  ClockSource_t;


/**
 * @brief       Clock Configuration Structure
 *
 * @note        Used by ClockGetConfiguration
 * @note        Will be used by ClockSetConfiguration
 */
typedef struct {
                ///@{
                /* Configuration info. Used by ClockSetConfiguration */
                ClockSource_t   source;           ///< HFCLK clock source
                uint32_t        basefreq;         ///< Base frequency of clock source
                uint32_t        hclkdiv;          ///< Divisor of base frequency to generate HFCLK
                uint32_t        corediv;          ///< Divisor of HFCLK to generate Core Clock
                uint32_t        perdiv;           ///< Divisor of HFCLK to generate Peripheral Clock
                ///@}
                ///@{
                /* For info only. Not used by ClockSetConfiguration  */
                uint32_t        hclkfreq;         ///<  =HFCLK/hclkdiv
                uint32_t        corefreq;         ///<  =HFCLK/hclkdiv/corediv
                uint32_t        perfreq;          ///<  =HFCLK/hclkdiv/perdiv
                ///@}
} ClockConfiguration_t;


/* CMSIS Like */
uint32_t SystemCoreClockSet(ClockSource_t source, uint32_t hclkdiv, uint32_t corediv);


uint32_t ClockGetConfiguration(ClockConfiguration_t *p);
uint32_t ClockConfigureForFrequency(uint32_t freq);
uint32_t ClockSetHFClockDivisor(uint32_t div);
uint32_t ClockGetPeripheralClockFrequency(void);
uint32_t ClockGetCoreClockFrequency(void);
uint32_t ClockSetPrescalers(uint32_t corediv, uint32_t perdiv);

#endif //SYSTEM_EFM32GG_EXT_H
