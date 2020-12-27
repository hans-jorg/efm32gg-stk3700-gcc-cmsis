22 PWM Generation
=================

# PWM Generation


The EFM32GG-STk3700 board has a 4 timer, that can be used to generate PWM signals.
Each timer has three channels, 0, 1 and 2. Each channel's output can be connected to
an external pin as shown in the table below.

| Timer  | Channel |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
|--------|---------|----|----|----|----|----|----|----|----|
|TIMER0  | CC0     |PA0 |PA0 |PF6 |PD1 |PA0 |PF0 |    |    |
|TIMER0  | CC1     |PA1 |PA1 |PF7 |PD2 |PC0 |PF1 |    |    |
|TIMER0  | CC2     |PA2 |PA2 |PF8 |PD3 |PC1 |PF2 |    |    |
|TIMER1  | CC0     |    |PE10|PB0 |PB7 |PD6 |    |    |    |
|TIMER1  | CC1     |    |PE11|PB1 |PB8 |PD7 |    |    |    |
|TIMER1  | CC2     |    |PE12|PB2 |PB11|    |    |    |    |
|TIMER2  | CC0     |PA8 |PA12|PC8 |    |    |    |    |    |
|TIMER2  | CC1     |PA9 |PA13|PC9 |    |    |    |    |    |
|TIMER2  | CC2     |PA10|PA14|PC10|    |    |    |    |    |
|TIMER3  | CC0     |PE14|PE0 |    |    |    |    |    |    |
|TIMER3  | CC1     |PE15|PE1 |    |    |    |    |    |    |
|TIMER3  | CC2     |PA15|PE2 |    |    |    |    |    |    |


#References

[EMF32GG Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)

[EFM32GG990 Data Sheet](https://www.silabs.com/documents/public/data-sheets/efm32gg-datasheet.pdf)

[AN0021](https://www.silabs.com/documents/public/application-notes/AN0021.pdf)

