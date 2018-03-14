
09 - Debounce {#mainpage}
============

Actually, this is the 3rd version of Button. It implements a debounce using a interrupt based method.
It uses the HAL for LEDs (STK3700) used in the last Blink example.


# Bounce


For more information about bouncing in mechanical contacts, see:

* [Ganssle](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers),
* [Ganssle Part 1](http://www.ganssle.com/debouncing.htm), 
* [Ganssle Part 2](http://www.ganssle.com/debouncing-pt2.htm),
* [Embarcados](https://www.embarcados.com.br/leitura-de-chaves-debounce/)
* [Hackday](https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/)
* [Cleghorn](https://github.com/tcleg/Button_Debouncer)
    
The basics of debouncing is to wait until the tranistories vanished. The primitive delay routines (* volatile int b= 1000; while (b--) {} *) can only be used in very simple cases, because is monopolizes the CPU. This is the same problem, of reading multiple times.

Using interrupts, it is necessary to handle a timer interrupt, and sometimes,  the button interrupt.

