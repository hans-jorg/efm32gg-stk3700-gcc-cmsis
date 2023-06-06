31 PWM Generation
=================

# PWM Generation


The EFM32GG-STk3700 board has 4 timers, that can be used to generate PWM signals.
Each timer has three channels, numbered 0, 1 and 2. Each channel output can be connected to
an external pin as shown in the table below according the LOCATION field in ROUTE register.
It is not possible to route the channel outputs independently. The LOCATION field specify the
route used by the timer unit.

| Timer  | Channel | 0    | 1    | 2    | 3    | 4   | 5   | 6   | 7   |
| ------ | ------- | ---- | ---- | ---- | ---- | --- | --- | --- | --- |
| TIMER0 | CC0     | PA0  | PA0  | PF6  | PD1  | PA0 | PF0 |     |     |
| TIMER0 | CC1     | PA1  | PA1  | PF7  | PD2  | PC0 | PF1 |     |     |
| TIMER0 | CC2     | PA2  | PA2  | PF8  | PD3  | PC1 | PF2 |     |     |
| TIMER1 | CC0     |      | PE10 | PB0  | PB7  | PD6 |     |     |     |
| TIMER1 | CC1     |      | PE11 | PB1  | PB8  | PD7 |     |     |     |
| TIMER1 | CC2     |      | PE12 | PB2  | PB11 |     |     |     |     |
| TIMER2 | CC0     | PA8  | PA12 | PC8  |      |     |     |     |     |
| TIMER2 | CC1     | PA9  | PA13 | PC9  |      |     |     |     |     |
| TIMER2 | CC2     | PA10 | PA14 | PC10 |      |     |     |     |     |
| TIMER3 | CC0     | PE14 | PE0  |      |      |     |     |     |     |
| TIMER3 | CC1     | PE15 | PE1  |      |      |     |     |     |     |
| TIMER3 | CC2     | PA15 | PE2  |      |      |     |     |     |     |


From the Reference Manual (Section 32.3.4 Alternate Functions)

> Alternate functions are connections to pins from Timers, USARTs etc. These modules contain route
> registers, where the pin connections are enabled. In addition, these registers contain a location
> bit field, which configures which pins the outputs of that module will be connected to if they are
> enabled. If an alternate signal output is enabled for a pin and output is enabled for the pin, the
> alternate function’s output data and output enable signals override the data output and output
> enable signals from the GPIO. However, the pin configuration stays as set in GPIO_Px_MODEL,
> GPIO_Px_MODEH and GPIO_Px_DOUT registers. I.e. the pin configuration must be set to output enable
> in GPIO for a peripheral to be able to use the pin as an output. <br />It is possible, but not
> recommended to select two or more peripherals as output on the same pin. These signals will then
> be OR'ed together. However, TIMER CCx and CDTIx outputs, which are routed as alternate functions,
> have priority, and will never be OR'ed with other alternate functions. The reader is referred to
> the pin map section of the device datasheet for more information on the possible locations
> of each alternate function and any priority settings.

From Section 20.3.6 GPIO Input/Output

> The TIMn_CCx inputs/outputs and TIM0_CDTIx outputs are accessible as alternate functions through
> GPIO. Each pin connection can be enabled/disabled separately by setting the corresponding CCxPEN
> or CDTIxPEN bits in TIMERn_ROUTE. The LOCATION bits in the same register can be used to move
> all enabled pins to alternate pins.


##ROUTE Register


References
----------

[EMF32GG Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)

[EFM32GG990 Data Sheet](https://www.silabs.com/documents/public/data-sheets/efm32gg-datasheet.pdf)

[AN0021](https://www.silabs.com/documents/public/application-notes/AN0021.pdf)

