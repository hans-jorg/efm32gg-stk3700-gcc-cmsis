22 Better Newlib
===============

# Problems in the implementation

There is a violation of encapsulation rules when the clock frequency is changed. After the changing of clock frequency, many devices need to be reconfigured. This is the case of the UARTs. 

In the current implementation the UART_Init routine must be called to reconfigure the UART. But the main purpose of the newlib module is to hidden the implementation details, including the UART interface. The initialization of UART is done before the main routine starts (See _main in syscalls.c) and the necessity to call UART_Init in the main procedure leads to run errors difficult to analyze. 

The need to reconfigure devices when the clock frequency is changed is present in the drivers for other devices.

In order to avoid this problem.

1 - a callback mechanism is implemented in the clock routines. Whenever a clock changed, a registered routine is called and does the reconfiguration. There are two callbacks, one is called before the change (it permits to turn off any transmition or processing) and other after (it does the actual reconfiguration).

2 - The device, in this case, UART, routines need to register the functions. register a callback to reconfigure the baud rate according the new clock frequency.



There is a clock tree in the EFM32GG.

![image0001](file:clock-tree.png)

The following files were renamed in order to avoid confusion with files with the same names in other projects.

    clock_efm32gg_ext.c -> clock_efm32gg_ext2.c
    clock_efm32gg_ext.h -> clock_efm32gg_ext2.h
    uart.c -> uart2.c
    uart.h -> uart2.h

# Modification in the clock management routines

The function *ClockRegisterCallback* was added. It registers two functions *pre* and *post*. 
The *pre* function is called before the clock change, for example, to stop all transmission and/or 
processing. The *post* function is called after the clock change. It is used to reconfigure the 
device according the new clock configuration. Both functions are only called when a clock change 
occurs as specified by the *clock* parameter.

    ClockRegisterCallback( uint32_t clock, void (*pre)(uint32_t), void (*post)(uint32_t))

Both functions have a parameter specifying which clock change occured.

The clock is a OR of the following values: CLOCK_CHANGED_HFCLK, CLOCK_CHANGED_HFCORECLK, 
CLOCK_CHANGED_HFPERCLK, CLOCK_CHANGED_HFCORECLKLE, CLOCK_CHANGED_LFCLKA and CLOCK_CHANGED_LFCLKB.

There are other modifications:

1 - SystemCoreClockSet function was renamed to ClockSetCoreClock. So all function names in the 
*clock* module start with Clock. An alias was added as a preprocessor symbol to 
clock_efm32gg2.h to enable compatibility.

2 - The following aliases were added to clock_efm32gg_ext2.h as preprocessor symbols. The clock 
acronyms used in the reference manual and datasheet

    ClockSetHFCORECLK is an alias to ClockSetCoreClock
    ClockGetHFCORECLK is an alias to ClockGetCoreClockFrequency
    ClockGetHFPERCLK is an alias to ClockGetPeripheralClockFrequency



# Modification in the UART routines

The main modification is the addition of callback routines. The *pre* routine just turn off the 
receive and the transmition operations of the uart. The *post* routine reconfigures the baud rate 
according the new clock configuration.

Another modification is the possibily to use the same set of routines to control the two UARTS 
on the microcontroller: UART0 and UART1. This is controlled by the preprocessor USE_UART1 
parameter.

# Modification in syscalls.c

Only the mapping Serial* to UART* functions is modified, because UART* functions have a uart 
parameter. For now, all operations are mapped to UART0.

# References

[EMF32GG Reference Manual](https://www.silabs.com/documents/public/reference-manuals/EFM32GG-RM.pdf)

[EFM32GG990 Data Sheet](https://www.silabs.com/documents/public/data-sheets/efm32gg-datasheet.pdf)


