8  Using Timers
===============

This is the 8th version of Blink. It uses the same HAL for LEDs (STK3700). The main modification is the use a better way to control time.


When using a periodical interrupt, there is a tendency to code a convoluted interrupt routine, because it concentrates the processing of different tasks, each running at different rates and priorities.

All Cortex M devices have a System Timer (SysTick) peripheral. It is a 24 bit counter (Beware, it has not 32 bits). It is basically controlled by two registers: SysTick Reload Value Register (RVR) and the SysTick Current Value Register (CVR). The Current Value Register counts downward until zero. Then it loads automatically the value stored in the Reload Value Register. Most implementations permit to choose different sources for the clock signal, but the default is the Core Clock.

One approach is to have different software counters. Each one associated with a function. When the a counter reaches zero, the function is called and the counter is reloaded. Otherwise the counter is decremented.

A structure called Timers_t is used to keep the information needed.
    typedef struct {
        int counter;
        int period;
        void (*function)(void);
    } Timers_t;

The timer information is stored in an array called Timers. The size is defined by the preprocessor symbol TIMERS_N, and the default value is 10. This can be redefined in the command line by the use of the -DTIMERS_N=20 parameters.

    Timer_t Timers[TIMERS_N];

A variable called Timers_cnt controls the number of counters in use.

The main routine is the Timers_dispatch, that must be called in the SysTick interrupt routine. It scans the Timers array, decrements the counter values and calls the corresponding function, when it reaches zero.

    void Timers_dispatch(void) {
    int i;

        for(i=0;i<Timers_cnt;i++) {
            if( Timers[i].counter == 0 ) {
                Timers[i].function();
                Timers[i].counter = timers[i].period;
            }
            Timers[i].counter--;
        }
    }

The interrupt routine has the following structure.

    void SysTick_Handler(void) {
    static int counter = 0; // must be static
    
        if( counter != 0 ) {
            counter--;
        } else {
            Timers_dispatch();
            counter = DIVIDER-1;
        }
    }
    
The time unit is SysTick Frequency divided by DIVIDER.

To use it, one should create the functions to be called and add it to the timers list.


    void t1(void) {
        // Called every time
    }

    void t3(void) {
        // Called every 3rd time
    }

    void t10(void) {
        // Called every 20th time
    }


    main() {
        ...
        Timers_add(t1,1);
        Timers_add(t3,3);
        Timers_add(t10,10);
        ...
        while(1) {}
    }


