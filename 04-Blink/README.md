4 Another Blink
===============


This is the 4th version of Blink. It uses direct access to register of the EFM32GG990F1024 microcontroller but with a simple Hardware Abstraction Layer (HAL) for the LED module. This HAL is built directly upon GPIO, without using a GPIO HAL (See version 2 of Blink).

The main objective is to control the LEDs without knowing about GPIO and little overhead. The architecture is shown below.

    +---------------------------------------------------------------------+
    |                            Application                              |
    +---------------------------------------------------------------------+
    |                               LED HAL                               |
    +---------------------------------------------------------------------+
    |                              Hardware                               |
    +---------------------------------------------------------------------+


The LED HAL is implemented in the *led.c* and *led.h* files.

The main functions for a LED are:

-   Initialize (Configure processor, gpio, etc)
-   Turn it on
-   Turn if off
-   Toggle it

The symbols to access the LEDs are defined as bit masks to easy the access the corresponding pins of the GPIO Port E.

    #define LED1 BIT(2)
    #define LED2 BIT(3)

###LED_Init

*LED_Init(leds)* initializes the GPIO for the LEDs specified by *leds*. *leds* is a bit mask, with 1 in the desired position. Bit 0 is the Least Significant Bit (LSB) of the 32 bit word.

###LED_Write

*LED_Write(off,on)* turns off the LEDs specified by *off*, and then turns on the LEDs specified by *on*. If a LED is specified in both, it is first cleared and then set.

###LED_Toggle

*LED_Toggle(leds)* inverts the status of the LEDs specified by *leds*.

###LED_On

*LED_On(leds)* turns on the LEDs specified by *leds*.

###LED_Off

*LED_Off(leds)* turns off the LEDs specified by *leds*.

###Acessing LEDs

To configure the LED pins, the instruction below can do the job.

    LED_Init(LED1|LED2);

To turn on the LED1, just code

    LED_Write(0,LED1);

To turn off the LED1, just

    LED_Write(LED1,0);

To toggle the LED1, just

    LED_Toggle(LED1);

In all cases above, it is possible to modify more than on LED. For example, to clear both LEDs,

    LED_Write(LED1|LED2,0);
