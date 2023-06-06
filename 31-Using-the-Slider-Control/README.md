22 Touch Slider
===============

#Touch slider


The EFM32GG-STk3700 board has a 4 pad sensor, that can be read thru the ACMP module.

The signals are connected
 
 Signal     |   Port/Pin    | Analog comparator channel
------------|---------------|----------------------------
UIF_TOUCH0  |  PC8          |    ACMP1/CH0
UIF_TOUCH1  |  PC9          |    ACMP1/CH1
UIF_TOUCH2  |  PC10         |    ACMP1/CH2
UIF_TOUCH3  |  PC11         |    ACMP1/CH3

They use the alternate function 0 (AF0)

The capacitive touch slider works by sensing changes in the capacitance of the pads when touched by a
human finger. Sensing the changes in capacitance is done by setting up the touch pad as part of an RC
relaxation oscillator using the EFM32's analog comparator, and then counting the number of oscillations
during a fixed period of time

The ACMPs have a capacitive sense mode.

Without touch, it builds a RC oscillator, that runs in a certain frequency. When a touch occurs, the 
capacitance increases and the frequency decreases. Measuring these variations, it is possible
to detect a touch. The ACMP pins have a pullup resistor, that is enabled by setting 
CSRESEN in the ACMPx_INPUTSEL. The values of R can be set among four values in the CSRESSEL field.

CSRESSEL  |   Resistor (Ohm)
----------|--------------------
   00     |     39 K
   01     |     71 K
   10     |    104 K
   11     |    136 K

Additionally, the NEGSEL must be set to CAPSENSE (11), VDD must be set to 1/8 or 1/4 by setting the VDDLEVEL
to 8 or 16, respectively. 

The ACMPOUT bit of the ACMPx_STATUS register is the comparator status.

Using more sensors (and pins), it is possible to detect the movement of a finger over the pads.

The measured frequency can be in the range 10-200 KHz. To avoid a overload of the CPU, this can be done using timers. One timer counts pulses out of the comparator and other triggers a periodic interrupt. At each periodic interrupt, the counter is stored. A contact is then detected by comparing the frequencies.

Another way is to measure the period or the pulse widht, if it is correlated to the frequency. This can be done by using the timer in the input capture mode.

As the frequency measured increased when a touch occurs, the base (minimal) frequency or the base (maximal) period must be stored and used as reference.


To get it working, it is neccessary to use the Peripheral Reflex System, and configure it to feed the counter input. This is done in two steps:

1 - Configure TIMER to use a PRS channel as input, e.g., PRSCH11, by setting PRSSEL field in TIMER_CTRL register.
2 - Configure PRS to route the ACMP output to the channel by setting SOURCESEL field in the PRS_CH11_CTRL register.


## Measurement

Measure:
zero counter
start counter
wailt until measureflag set

Interrupt (periodic):
Store cnt of timer onto a variable. 
Determine the largest.



#References

[EMF32GG Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)

[EFM32GG990 Data Sheet](https://www.silabs.com/documents/public/data-sheets/efm32gg-datasheet.pdf)

[AN0020](https://www.silabs.com/documents/public/application-notes/AN0020.pdf)

