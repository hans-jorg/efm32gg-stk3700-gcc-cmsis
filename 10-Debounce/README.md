#*10* Debouncing

Actually, this is the 3rd version of Button. It implements a debounce process using an interrupt based method. It uses the HAL for LEDs (STK3700) used in the last Blink example.

##Bounce

When a contact occurs between two metal parts, there is a cycle of contact and release pulses until in settles in the expected level. The main cause is the elastic collision between the two parts. It generally takes about 10-50 ms to get a stable read.

The basics of debouncing is to wait until the transitory vanished.

The basic approaches are:

1.  Implement a state machine where a stable output is generated then the read is confirmed (repeated) after a predetermined time,
2.  Read multiple times, and generates an output when the read is repeated (confirmed) N times, where N corresponds to the debounce time.

The primitive delay routines (* volatile int b= 1000; while (b--) {} *) can only be used in very simple cases, because is monopolizes the CPU. This is the same problem of reading multiple times.

A better approach, is to use interrupts. In this case it is necessary to handle a timer interrupt, and sometimes, the button interrupt.

More information
--------------------------------------------

-   [Ganssl](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[e](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[. ](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[My favorite software debouncers](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[7]
-   [Ganssle. ](http://www.ganssle.com/debouncing.htm)[Debounce](http://www.ganssle.com/debouncing.htm)[ Part 1](http://www.ganssle.com/debouncing.htm)[8]
-   [Ganssle. ](http://www.ganssle.com/debouncing-pt2.htm)[Debounce](http://www.ganssle.com/debouncing.htm)[ Part 2](http://www.ganssle.com/debouncing-pt2.htm)[9]
-   [Embarcado](https://www.embarcados.com.br/leitura-de-chaves-debounce/)[s. Leitura de chaves](https://www.embarcados.com.br/leitura-de-chaves-debounce/)[10]
-   [Hackda](https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/)[y. Debounce code. one post to rule them all](https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/)[11]
-   [Cleghor](https://github.com/tcleg/Button_Debouncer)[n. Button Debouncer](https://github.com/tcleg/Button_Debouncer)[12]
