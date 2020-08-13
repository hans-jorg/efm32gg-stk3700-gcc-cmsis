17  Using Protothreads
======================

##Multithreading with one stack

This is the 8th version of Blink. The main modification is the use of protothreads. This emulates the quasi parallel execution of tasks by implementing a cooperative multitasking kernel.

It uses a 'hack' call *Duff's Device* (see below) to emulate multitasking by repeated calls. At each call, the executes resumes at the point of the last one. Due the nature of this hack, all local variables must be static, retaining value between calls.

##Blinker Task

The code for the Blinker Task is very similar of the main code in example 03. It has a drawback. There is a glitch when the counter reaches the limit for a 32 bit unsigned integer. At 1 kHz, about 49 days.

    #include "pt.h"
    struct pt pt;
    uint32_t threshold;
    #define PT_DELAY(T) threshold = timer_counter+(T); PT_WAIT_UNTIL(pt,timer_counter>=threshold);

    ...

    PT_THREAD(Blinker(struct pt \*pt)) {

     PT_BEGIN(pt);

     while(1) {
         // Processing
         LED_Toggle(LED1);
         PT_DELAY(1000);
         LED_Toggle(LED2);
         PT_DELAY(1000);
         LED_Write(0,LED1|LED2);
         PT_DELAY(1000);
         }
         PT_END(pt);
    }

    The main routine is

    void main(void) {
    ...
    PT_INIT(pt);
    ...


##Duff's Device


The following is a valid C code and is the base of the protothread library.

    void send(char \*to, char \*from, int count) {
    int n = (count + 7) / 8;

        switch (count % 8) {
        case 0: do { *to = *from++;
        case 7: *to = *from++;
        case 6: *to = *from++;
        case 5: *to = *from++;
        case 4: *to = *from++;
        case 3: *to = *from++;
        case 2: *to = *from++;
        case 1: *to = *from++;
        	} while (--n > 0);
        }

    }
