2 Blink again
=============


This is the 2nd version of Blink. It uses direct access to register of the EFM32GG990F1024 microcontroller but with a simple Hardware Abstraction Layer (HAL) for the GPIO module.

The architecture is shown below.

    +---------------------------------------------------------------------+
    |                            Application                              |
    +---------------------------------------------------------------------+
    |                              GPIO HAL                               |
    +---------------------------------------------------------------------+
    |                              Hardware                               |
    +---------------------------------------------------------------------+


The HAL is implemented in the *gpio.c* and *gpio.h* files.

In the *gpio.h* files, a type *GPIO_t* is defined to be used to specify the GPIO port used.

To use the GPIO Port, it is necessary to:

-   Enable clock for peripherals
-   Enable clock for GPIO
-   Configure pins as outputs
-   Set them to the desired values

The *GPIO_Init* routine cares of the initialization. It uses the *GPIO_ConfigPins* routine to set the mode (input or output) for the specified pins. To specify the pin status, there are two alternatives: *GPIO_WritePins*, *GPIO_TogglePins*. To read (not implemented yet), there is *GPIO_ReadPins*.

So, it is possible to use the GPIO without detailed knowledge of the internal works. Another advantage is that this routines can be ported to other architectures, with different GPIO implementations.

###GPIO_Init

*GPIO_Init(gpio,inputs,outputs)* initialized the specified GPIO port *gpio*. The pins specified by *inputs* are configured to be input, and the pins specified by *output*, configured to Push-Pull Output mode. If a pin is specified in both, it is first configured as input. *inputs* and *outputs* are bit masks, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###GPIO_WritePins

*GPIO_WritePins(gpio,zeroes,ones)* set the pins of GPIO port *gpio*, specified by *zeroes* to zero, and the set the pins specified by *ones* to one. If a pin is specified in both, it is first cleared and then set. *zeroes* and *ones* are bit masks, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###GPIO_TogglePins

*GPIO_TogglePins(gpio,pins)* inverts the output pins of GPIO port *gpio*. The *pins* parameter is a bit mask, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###GPIO_ConfigPins

*GPIO_ConfigPins(gpio,pins,mode)* is used to configure the pins of the GPIO port specified by the *gpio* parameter to the desired *mode*. The pins to be configured are specified in the bit mask *pins* parameter, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

The parameter mode can be one of the symbols listed in the table below.

	GPIO_MODE_DISABLE
    GPIO_MODE_INPUT
    GPIO_MODE_INPUTPULL
    GPIO_MODE_INPUTPULLFILTER
    GPIO_MODE_PUSHPULL
    GPIO_MODE_PUSHPULLDRIVE
    GPIO_MODE_WIREDOR
    GPIO_MODE_WIREDORPULLDOWN
    GPIO_MODE_WIREDAND
    GPIO_MODE_WIREDANDFILTER
    GPIO_MODE_WIREDANDPULLUP
    GPIO_MODE_WIREDANDPULLUPFILTER
    GPIO_MODE_WIREDANDDRIVE
    GPIO_MODE_WIREDANDDRIVEFILTER
    GPIO_MODE_WIREDANDDRIVEPULLUP
    GPIO_MODE_WIREDANDDRIVEPULLUPFILTER


###Acessing LEDs

The symbols to access the LEDs in the GPIO Port E registers can be defined as

    #define LED1 BIT(2)
    #define LED2 BIT(3)
    
To configure the LED pins, the instruction below can do the job.

    GPIO_Init(GPIOE,0,LED1|LED2);

To turn on the LED1, just code
    
    GPIO_WritePins(GPIOE,0,LED1);

To turn off the LED1, just

    GPIO_WritePins(GPIOE,LED1,0);

To toggle the LED1, just

    GPIO_TogglePines(GPIOE,LED1);

In all cases above, it is possible to modify more than on LED. For example, to clear both LEDs,

    GPIO_WritePins(GPIOE,LED1|LED2,0);
