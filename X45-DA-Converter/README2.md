5 Using the DA Converter ========================


## The DA Converter on the EFM32GG990


The EFM32GG990F1024 in the STK3700 board has one Digital-Analog converter
with 2 channels when using single-ended terminals or 1 channel when
using differential signals.

It is called DAC0, it is a 12 bit Digital-to-Analog converter and can work at speeds up
to 500 Ksps.

It is possible to configure which pins are used by the DA converter. For
the EFM32GG990 the following pins are related to the DAC0:


|  Signal     | Pin(LOC)|         |         |         |         |
|-------------|---------|---------|---------|---------|---------|
| OUT0        | PB11(0) |         |         |         |         |
| OUT0ALT     | PC0(0)  | PC1(1)  | PC2(2)  | PC3(3)  | PD0(4)  |
| OUT1        | PB12(0) |         |         |         |         |
| OUT1ALT     | PD1(4)  |         |         |         |         |
| P0          | PC4(0)  |         |         |         |         |
| P1          | PD6(0)  |         |         |         |         |
| N0          | PC5(0)  |         |         |         |         |
| N1          | PD7(0)  |         |         |         |         |

in the STK3700 board the following signal are available in the Breakout
pads and Expansion Header.

| Signal   | Port pin  |    Breakout pad   | EXP Header    |
|----------|-----------|-------------------|---------------|
|  OUT0    |    PB11   |    Sup 5          |     11        |
|  OUT1    |    PB12   |    Sup 6          |     13        |
|  OUT0ALT |    PC0    |    Inf 10         |      3        |
|  OUT0ALT |    PC1    |    Inf 11         |               |
|  OUT0ALT |    PC2    |    Inf 12         |               |
|  OUT0ALT |    PC3    |                   |      5        |
|  OUT0ALT |    PD0    |    Sup 9          |      4        |
|  OUT1ALT |    PD1    |    Sup 10         |      6        |
|  P0      |    PC4    |                   |      7        |
|  P1      |    PD6    |    Sup 16         |     16        |
|  N0      |    PC5    |                   |      9        |
|  N1      |    PD7    |    Sup 17         |     15        |


>> This version of DAC HAL only uses the main outputs pins: OUT0 and OUT1


The numbers in parenthesis are the LOCATION number used to configure
the pin.

> What the hell are N0, N1, P0 and P1 pins?





There is a prescaler to divide the peripheral clock (HFPERCLK) and to
generate the clock for the DA operation (DAC_CLK). The DA frequency is
given by

f_{DAC_CLK) = f_{HFPERCLK}/2^{DA0.CTRL.PRESC)



> The DAC_CLK should be 1 MHz or less!!!!!!. So keep the PRESC at least
at 4 when > running at 48 Mhz.

About alternate pins, the Reference Manual says

> The DAC channels can also drive an alternative output network, which is
described in the Opamp > chapter in Section 30.3.1.2 (p. 735) . To enable
this network, OUTMODE must be configured to ADC > in DACn_CTRL. The actual
output network can be configred by configuring DACn_OPAxMUX registers.

It is possible to the output signal go to only to the main output,
only to the ALT output or both.

To use only the main output, set DAC0_OPA0MUX.OUTMODE to MAIN.

To use only the ALT output, set DAC0_OPA0MUX.OUTMODE to ALT.

To use output signal at both the main and the ALT outputs, set
DAC0_OPA0MUX.OUTMODE to ALL.

It is also possible to route the output signal to Analog Digital Converter
(AD), to an internal OPAMP] or to an Analog Compare Peripheral (ACMP).

Is is possible to reoute the output signal to OUTx pins by set the
corresponding bit in DACn_OPAxMUX registers. Note that the OUTPUT field
must be set to ADC in order to this routing to work.


The operation can be monitored by observing the STATUS register,
specifically, the CH0DV and CH1DV bits, that when set, indicates that
there is still data to be converted on the DATA registers.

There is an additional parameter to be configured: the BIASPROG. It
specifies the amount of current thru the voltage reference. The higher,
the better, but with increased power comsumption. From the Reference
Manual.

>The bias current settings should only be changed while both DAC channels
are disabled. The electrical > characteristics given in the datasheet
require the bias configuration to be set to the default values, > where
no other bias values are given.

The default values can be found in the DEVICE INFO area, specifically,
the the DAC0_BIASPROG subarea.  By the way, the Device Info contains
other values of interest. It is possible to use a predefined structure
DEVINFO to access part of these values.


|  Address   | Symbol        |  Field    | Contents               |
|------------|---------------|-----------|------------------------|
| 0x0FE08050 | DAC0_CAL      |           | Register reset value.  |
| 0x0FE08058 | DAC0_BIASPROG |           | Register reset value.  |
| 0x0FE081C8 | DAC0_CAL_1V25 | DAC0CAL0  | [22:16]: Gain for 1V25 reference, <br/>[13:8]: Channel 1 offset for 1V25 reference, <br/>[5:0]: Channel 0 offset for 1V25 reference. |
| 0x0FE081CC | DAC0_CAL_2V5  | DAC0CAL1  | [22:16]: Gain for 2V5 reference, <br/>[13:8]: Channel 1 offset for 2V5 reference, <br/>[5:0]: Channel 0 offset for 2V5 reference.  |
| 0x0FE081D0 | DAC0_CAL_VDD  | DAC0CAL2  | [22:16]: Reserved (gain for this reference cannot be calibrated), <br/>[13:8]: Channel 1 offset for VDD reference, <br/>[5:0]: Channel 0 offset for VDD reference. |

The values not accessed through the provided DEVINFO structure should
be accessed using symbols defined by the user/programmer.

     #define  DEVINFO_DAC0_CAL       (*((uint32_t *) 0x0FE08050UL) )
     #define  DEVINFO_DAC0_BIASPROG  (*((uint32_t *) 0x0FE08058UL) )


## The DA Converter on the EFM32-STK3700

The STK3700 Board has the following signals available on the EXT
expansion header.


| Signal      | MCU pin   | EXT pin   | Shared with                  |
|-------------|-----------|-----------|------------------------------|
| DAC0_CH0    |  PB11     |  11       |  OPAMP_OUT0, TIMER1_CC2      |
| DAC0_CH1    |  PB12     |  13       |  OPAMP_OUT1                  |


> The DA pins are also used by other peripherals. Observe that the
notation EXPn, > where n* is the header pin number, is used instead of
EXP_HEADERm used in > the schematics.




| MCU pin | DA signal | Board   | Shared with                                 |
|---------|-----------|---------|---------------------------------------------|
|  PB11   | OUT0      | EXP11   | OPAMP_OUT0, TIM1_CC2, LETIM0_OUT0, I2C1_SDA |
|  PB12   | OUT1      | EXP13   | OPAMP_OUT1, LETIM0_OUT1, I2C1_SCL           |
|  PC0    | OUT0ALT   | EXP3    | OPAMP_OUT0ALT, ACMP0_CH0, EBI_A23, TIM0_CC1 |
|         |           |         | PCNT0_S0IN, US0_TX, US1_TX, I2C0_SDA        |
|         |           |         | LES_CH0, PRS_CH2                            |
|  PC1    | OUT0ALT   | NAND_ALE| OPAMP_OUT0ALT, ACMP0_CH0, EBI_A24           |
|         |           |         | TIM0_CC2, PCNT0_S1IN, US0_RX, US1_RX        |
|         |           |         | I2C0_SCL, LES_CH1,PRS_CH3                   |
|  PC2    | OUT0ALT   | NAND_CLE| OPAMP_OUT0ALT, ACMP0_CH2, EBI_A25,          |
|         |           |         | TIM0_CDTI0, US2_TX, LES_CH2                 |
|  PC3    | OUT0ALT   | EXP5    | OPAMP_OUT0ALT, ACMP0_CH3, EBI_NANDREn       |
|         |           |         | TIM0_CDTI1, US2_RX, LES_CH3                 |
|  PC4    | P0        | EXP7    | OPAMP_P0, ACMP0_CH4, EBI_A26, TIM0_CDTI2    |
|         |           |         | LETIM0_OUT0, PCNT1_S0IN, US2_CLK            |
|         |           |         | I2C1_SDA, LES_CH4                           |
|  PD0    | OUT0ALT   | EXP4    | ADC0_CH0, OPAMP_OUT0ALT, OPAMP_OUT2         |
|         |           |         | PCNT2_S0IN, US1_TX                          |
|  PD1    | OUT1ALT   | EXP6    | ADC0_CH1,OPTAMP_OUT1ALT, TIM0_CC1           |
|         |           |         | PCNT2_S1IN, US1_RX, DBG_SWO                 |
|  PD6    | P1        | EXP16*  | ADC0_CH6, OPAMP_P1, TIM1_CC0, LETIM0_OUT0   |
|         |           |         | PCNT0_S0IN, US1_RX, I2C0_SDA, LES_ALTEX0    |
|         |           |         | ACMP0_O, ETM_TD0                            |

> * PD6 is also used to excite the photo-transistor


## References

1 [ENF32GG Reference Manual]
2 [EFM32GG990 Datasheet]
3 [Application Note]
