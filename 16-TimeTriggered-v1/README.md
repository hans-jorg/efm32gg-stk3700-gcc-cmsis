16  Time Triggered Systems
==========================

##A time triggered kernel
 
This is the 7th version of Blink. The main modification is the use of a time triggered scheduler. In the last example, a periodical interrupt (SysTick) was used to implement a simple state machine.


             -----------               -----------               -----------
        ---->| state=0 |-------------->| state=1 |-------------->| state=2 |----------
        |    -----------               -----------               -----------         |
        |                                                                            |
        ------------------------------------------------------------------------------


##Standard approach

In the last example a counter was used to divide further the interrupts, because the divider must be less then 2^24 (=16777216) and because other tasks can have different periods.

    #define DIVIDER 1000
    ...
    void SysTick_Handler(void) {
    static int counter = 0; // must be static
    static int8_t state = 0; // must be static
    
        if( counter != 0 ) {
    	    counter--;
        } else {
        // Processing
            switch(state) {
            case 0:
                LED_Toggle(LED1);
                state = 1;
            break;
            case 1:
                LED_Toggle(LED2);
                state = 2;
            break;
            case 2:
                LED_Write(0,LED1|LED2);
                state = 0;
            break;
            }
        counter = DIVIDER-1;
    }

Dividing the interrupts is generally repeated for other tasks with different periods. An important point, is that doing the processing in the interrupt is a bad idea, because during the processing the interrupts are disabled. This approach is called back-foreground, with the interrupt routines as foreground tasks and the main routine as the background test and should only be used in very simple cases.

##Better alternative

A better alternative is the interrupt processing routine to signalize that a task must be run and the task runs outside the interrupt routine.

    #define DIVIDER 1000
    ...
    int run = 0;
    ...

	void SysTick_Handler(void) {
    static int counter = 0; // must be static
    static int8_t state = 0; // must be static

         if( counter != 0 ) {
	         counter--;
         } else {
	         run++;
	         counter = DIVIDER-1;
         }

        ...

     #define DIVIDER 1000

        ...

    void Blinker(void) {
    static int8_t state = 0; // must be static

         switch(state) {
         case 0:
	         LED_Toggle(LED1);
	         state = 1;
	         break;
         case 1:
    	     LED_Toggle(LED2);
      	    state = 2;
         break;
         case 2:
	         LED_Write(0,LED1|LED2);
	         state = 0;
	         break;
         }

    }

    ...

    int main(void) {

     ...

    	while(1) {
			if( run ) {
				Blinker();
				run--;
			}
		}
    }

##Time Triggered Scheduler

The approach shown before is the basis of a time triggered task scheduler according [Pontt](https://www.safetty.net/products/publications/pttes)[21].
A list of tasks, including information about period and the counter value is maintained through an API. A task is then started (by calling the corresponding function) when the period is over as indicated by the run variable.

    typedef struct {
         void (*function)(void); ///< pointer to function with task code
         INT counter; ///< counter (in ticks). when 0, task should be run
         INT period; ///< period (in ticks). when 0, task in run once
         INT run; ///< run counter. when above 1, task is delayed
    } Task_t;

The scheduler is initialized through Task_Init. Tasks are inserted using the Task_Add routine. During the interrupt, it is only necessary to update the list by calling Task_Update. And in the main loop, a Task_Dispatch must be called.

    void SysTick_Handler(void) {
        Task_Update();
    }

    void Blinker(void) {
    static int8_t state = 0; // must be static
        switch(state) {
        case 0:
			LED_Toggle(LED1);
			state = 1;
			break;
		case 1:
			LED_Toggle(LED2);
			state = 2;
			break;
		case 2:
			LED_Write(0,LED1|LED2);
			State = 0;
			break;
		}
	}
	...
    int main(void) {
    ...
    	Task_Init();

		Task_Add(Blinker,1000,0);
		SysTick_Config(SystemCoreClock/DIVIDER);

     	/* Blink loop */

		while (1) {
		     Task_Dispatch();
		}
    }
