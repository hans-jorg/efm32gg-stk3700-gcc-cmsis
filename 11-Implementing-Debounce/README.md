11  Debouncing
==============

This is the 3rd version of Button. It implements a debounce process using an interrupt based method. It uses the HAL for LEDs (STK3700) used in the last Blink example.

##Bounce

When a contact occurs between two metal parts, there is a cycle of contact and release pulses until in settles in the expected level. The main cause is the elastic collision between the two parts. It generally takes about 10-50 ms to get a stable read.

The basics of debouncing is to wait until the transitory vanished.
The basic approaches are:
    1. Implement a state machine where a stable output is generated when the read is confirmed (repeated) after a predetermined time,
    2. Read multiple times, and generates an output when the read is repeated (confirmed) N times, where N corresponds to the debounce time.

In boot approaches, it is important to use a strict timing control to enhance portability and reliability.

A sequential approach would be.

    // Wait until button is pressed
    while (Button_Read()==BUTTON_PRESSED) {}

    cnt = 0;
    do {
         Delay(T1MS);
         if ( Button_Read()!=BUTTON_PRESSED) // Not pressed
             cnt = 0;                        // Start again
         else                                // Pressed
             cnt++;                          // Count
    } while (cnt <= DEBOUNCE_TIME);

A better approach, is to use interrupts. In this case it is better to use a timer interrupt. Generally, the use of interrupt generated by the pin attached to the button is a bad idea.

A state machine diagram describes the working of the interrupt routine.

    int state = 0;      // 0 = Q0 (BUTTON RELEASED),  1 = Q1 (BUTTON PRESSED)
    int cnt;

    void Button_processing(void) {
    uint32_t b;
        b = Button_Read();
        switch(state) {
        case 0:         // Released
            if( b == PRESSED ) {
                if( cnt == 10 ) {
                    state = 1;
                } else {
                    cnt++;
                }
            } else { // Not Pressed
                cnt = 0;
            }
        case 1:         // Pressed
            if( b == PRESSED ) {
                cnt = 0;
            } else { // Not Pressed
                if( cnt == 10 ) {
                    state = 0;
                } else {
                    cnt++;
                }
            }
        }
    }

    int Button_Status(void) {

        if( state == 1 )
            return 1;
        else
            return 0;
    }

A clever approach is to use bit masks instead of counters.

In the source code (button.c and button.h) one can find the generalization of the above for more than one switch.

##More information

-   [Ganssle](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[e](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[. ](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[My favorite software debouncers](https://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers)[7]
-   [Ganssle](http://www.ganssle.com/debouncing.htm)[Debounce](http://www.ganssle.com/debouncing.htm)[ Part 1](http://www.ganssle.com/debouncing.htm)[8]
-   [Ganssle. ](http://www.ganssle.com/debouncing-pt2.htm)[Debounce](http://www.ganssle.com/debouncing.htm)[ Part 2](http://www.ganssle.com/debouncing-pt2.htm)[9]
-   [Embarcado](https://www.embarcados.com.br/leitura-de-chaves-debounce/)[s. Leitura de chaves](https://www.embarcados.com.br/leitura-de-chaves-debounce/)[10]
-   [Hackda](https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/)[y. Debounce code. one post to rule them all](https://hackaday.com/2010/11/09/debounce-code-one-post-to-rule-them-all/)[11]
-   [Cleghor](https://github.com/tcleg/Button_Debouncer)[n. Button Debouncer](https://github.com/tcleg/Button_Debouncer)[12]
