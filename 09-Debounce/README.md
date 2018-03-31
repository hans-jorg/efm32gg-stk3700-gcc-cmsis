
09 - Debounce {#mainpage}
============

Actually, this is the 3rd version of Button. It implements a debounce using a interrupt based method.
It uses the HAL for LEDs (STK3700) used in the last Blink example.


# Bounce

When a contact occurs between two metal parts, there is a cycle of contact and release pulses until in settles in the expected level. The main cause is the elastic colision between the two parts. It generally takes about 10-50 ms to get a stable read.

For more information about bouncing in mechanical contacts, see:

* [Ganssle](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers),
* [Ganssle Part 1](http://www.ganssle.com/debouncing.htm), 
* [Ganssle Part 2](http://www.ganssle.com/debouncing-pt2.htm),
* [Embarcados](https://www.embarcados.com.br/leitura-de-chaves-debounce/)
* [Hackday](https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/)
* [Cleghorn](https://github.com/tcleg/Button_Debouncer)
    
The basics of debouncing is to wait until the transitories vanished. 

The basic approaches are:

1 - Implement a state machine where an stable output is generated then the read is confirmed (repeated) after a predetermined time, 

2 - Read multiple times, and generates an output when the read is repeated (confirmed) N times, where N corresponds to the debounce time.


The primitive delay routines (* volatile int b= 1000; while (b--) {} *) can only be used in very simple cases, because is monopolizes the CPU. This is the same problem of reading multiple times.

A better approach, is to use interrupts. In this case it is necessary to handle a timer interrupt, and sometimes,  the button interrupt.

