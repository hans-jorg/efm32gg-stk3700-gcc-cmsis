5 Using SysTick to implement delays
===================================


##The SysTick Timer

This is the 5th version of Blink. It uses the same HAL used in the last example. The main aspect is the use of a standard timer present in all Cortex M processors.

The SysTick counter is a 24-bit regressive counter, which can be clocked by the main clock or by an alternate clock source. In the EMF32 Giant Geck family, the alternate clock source is the LFBCLK.

The maximal value of the SysTick counter is 2<sup>24</sup>-1 = 16777215. Every time the SysTick counter reaches zero, the counter is loaded with a predefined value and an interrupt is generated.

The are CMSIS standard functions to control the SysTick timer. The main routine is the *SysTick_Config*. The parameter is the reload value. If the reload value, is the frequency of the clock source, an interrupt is generated every second. But this only works for clock frequencies smaller than 16 MHz (It is a 24 bit counter!!). If the value is the clock frequency divided by 1000, an interrupt is generated every 1 ms. Take care of rounding!

To get the core clock frequency, when using CMSIS one can use the *SystemCoreClock* global variable. According CMSIS standard, it must be updated every time the clock frequency is updated.

In this implementation, the global variable *TickCounter* is incremented when a SysTick interrupt is generated, i.e., every 1 ms. Since it is incremented every 1 ms, the variable overflows after 49 days. Is is possible to use a larger variable, but the operation would not be atomic, because it will need more cycles to increment.

The global variable *TickCounter* must be defined as *volatile*. This instructs the compiler to get the value of the variable from memory every time it is needed. This avoid the compiler, when trying to optimize, to keep a copy of the variable in a register. In this case, the code would not detected a change of the variable.

The routine SysTick_Handler is standardized by CMSIS. It is defined in the correct position in the interrupt vector inside the start_efm32gg,c as a weak symbol. This means that it can be redefined without generating an error and the new symbol overides the old (weak) one.

To wait for a certain number of milliseconds, one can implement a Delay routine like this.

    void Delay(uint32_t delay) {
        volatile uint32_t limit = TickCounter+delay;
        while( TickCounter < limit ) {}
    }

But this implementation has problems when the *TickCounter* reaches the maximal value (2<sup>24</sup>-1) and wraps around. A better alternative is like this.

    void Delay(uint32_t delay) {
        volatile uint32_t initialvalue = TickCounter;
        while( (TickCounter-initialvalue) < delay ) {}
    }

###Note 1 – Delay with smaller values

To implement delay with smaller values, one can use the SysTick with the maximum value and to take the difference between its value. This works for delay as high as 16777215 clock pulses. For a 48 MHz, this means approximately 3,5 seconds.

    void DelayInClock(uint32_t delay) {
        volatile uint32_t initialvalue = SysTick->VAL;
        while( ((initialvalue-SysTick->VAL)&0xFFFFFF) < delay ) {}
    }
    
    main() {
     ...
        SysTickConfig(0xFFFFFF);
     ...
    }
    
###Note 2 – Another approach

It is possible to configure the SysTick to use a reload value and just wait until it reaches zero. It is not necessary to use interrupts. Again, the maximum time is approximately 3,5 seconds.

    void DelayPulses(uint32_t delay) {
    
     SysTick->LOAD = delay;
     SysTick->VAL = 0;
     SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
     while( (SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk) == 0 ) {}
     SysTick->CTRL = 0;
    }
    
###Note 3 – Yet another approach

For very small delays, it is possible to use a cycle counter, optionally available in some ARM microcontrollers in the Data Watchpoint and Trace (DWT) module. The DWT Cycle Counter Register is a 32 bit register, that is incremented at every core clock cycle. This means, it wraps around at approximately 89 seconds.

    #define DWT_CONTROL *((volatile uint32_t *)0xE0001000)
    #define DWT_CYCCNT *((volatile uint32_t *)0xE0001004)
    #define SCB_DEMCR *((volatile uint32_t *)0xE000EDFC)
    
    static inline uint32_t getDwtCyccnt(void) {
        return DWT_CYCCNT;
    }

    static inline void resetDwtCyccnt(void) {
        DWT_CYCCNT = 0; // reset the counter
    }
    
    static inline void enableDwtCyccnt(void) {
         SCB_DEMCR = SCB_DEMCR |BIT(24); // TRCENA = 1
         DWT_CONTROL = DWT_CONTROL | BIT(0) ; // enable the counter (CYCCNTENA = 1)
         DWT_CYCCNT = 0; // reset the counter
    }

###Note 4 - The same as before but using CMSIS

This the same as before but using CMSIS. According it, a DWT struct with many fields enables the access to the Cycle Counter.

    /*
     * Delay using Data Watchpoint and Trace (DWT)
     */
    
    static inline uint32_t getDwtCyccnt2(void)    {
     return DWT->CYCCNT;
    }
    
    static inline void resetDwtCyccnt2(void)  {
     DWT->CYCCNT = 0; // reset the counter
    }
    
    static inline void enableDwtCyccnt2(void) {
     CoreDebug->DEMCR = CoreDebug->DEMCR | BIT(24); // TRCENA = 1
     DWT->CTRL = DWT->CTRL | BIT(0) ; // enable the counter (CYCCNTENA = 1)
     DWT->CYCCNT= 0; // reset the counter
    }
 
