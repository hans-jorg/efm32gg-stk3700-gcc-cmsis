6 Changing the core clock frequency
===================================


##Processing during interrupt

This is the 6th version of Blink. It uses the same HAL for LEDs (STK3700). The main modification is the use of a non default clock frequency.

The routines to change the clock frequency are not part of CMSIS because the clock circuitry is up to the manufacturer. The routines to control clock frequency for the EFM32GG are in the *system-efm32gg-ext.c* file, with the interface defined in system-*efm32gg-ext.h*.

    +--------------------------------------------+
    |              Application                   |
    |--------------------------------------------|
    |               LED HAL                      |
    |--------------------------------------------|
    |              Hardware                      |
    +--------------------------------------------+


##Clock Circuitry

In the EFM32GG the clock source can be:

-   LFRCO: 32678 Hz internal RC oscillator
-   LFXCO: 32768 Hz crystal oscillator with an external 32768 Hz crystal
-   HFRCO Internal RC oscillator (factory calibrated) that runs at 1, 7, 11, 14 (default), 21 and 28 MHz
-   HFXCO: Crystal oscillator with an external crystal with an external crystal (48 MHz in the STK32700 Kit)

The clock source signal is then divided by the HFCLK divisor to generate the HFCLK signal. The HFCLK divisor is in the range 1 to 8.

The core clock is generated from the HFCLK by a prescaler, which is a power of 2 up to 512 (1,2,4,8,...,512). There is another prescaler to generate the peripheral clock.
                  +-----+                                         +--------------+
    HFRCO         |     |                                         |              |        HFCORECLK
    --------------|     |                              +----------| HFCORECLKDIV |----------------
    HFXCO         |     |       +-----------+          |          |              |
    --------------|     |       |           |  HFCLK   |          +--------------+
    LFRCO         | MUX |-------|  HFCLKDIV |----------|
    --------------|     |       |           |          |          +--------------+
    LFXCO         |     |       +-----------+          |          |              |        HFPERCLK
    --------------|     |                              +----------| HFPERCLKDIV  |----------------
                  |     |                                         |              |
                  +-----+                                         +--------------+


##Clock Control

The following routines enables the control of the clock for the EFM32GG and adjust accordingly the number of Flash Wait States and other configuration tidbits.

###SystemCoreClockSet

The main routine to control core clock frequency is

    uint32_t SystemCoreClockSet(ClockSource_t source, uint32_t hfclkdiv, uint32_t corediv);

The source can be:

-   CLOCK_LFXO to use the Low Frequency Crystal Oscillator: 32768 Hz
-   CLOCK_LFRCO to use the Low Frequency Internal RC Oscillator: 32768 Hz
-   CLOCK_HFRCO_1MHZ to use the Factory Calibrated High Frequency Internal RC Oscillator: 1 MHz
-   CLOCK_HFRCO_7MHZ to use the Factory Calibrated High Frequency Internal RC Oscillator: 7 or 6.6 MHz
-   CLOCK_HFRCO_11MHZ to use the Factory Calibrated High Frequency Internal RC Oscillator: 11 MHz
-   CLOCK_HFRCO_14MHZ to use the Factory Calibrated High Frequency Internal RC Oscillator: 14 MHz (default)
-   CLOCK_HFRCO_21MHZ to use the Factory Calibrated High Frequency Internal RC Oscillator: 21 MHz
-   CLOCK_HFRCO_28MHZ to use the Factory Calibrated High Frequency Internal RC Oscillator: 28 MHz
-   CLOCK_HFXO to use the High Frequency Crystall Oscillator: 48 MHz (STK3700)

This routine sets the Core and Peripheral Clock to use the same frequency.

###ClockSetHFClockDivisor

It is possible to change the HFCLK divisor using the following routine:

    void ClockSetHFClockDivisor(uint32_t hfclkdiv);

###ClockSetPrescalers

It is possible to change the core and peripheral prescalers using the following routine:

    void ClockSetPrescalers(uint32_t hfcoreclkdiv, uint32_t hfperclkdiv);
