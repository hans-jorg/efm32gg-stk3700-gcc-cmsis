18  Using FreeRTOS
==================

##A free real time kernel

FreeRTOS is an open source real time preemptive kernel with a small footprint. It can be downloaded from www.freertos.org.

It is composed of a small set of C source and header files common to all platforms and another set, specific to a target. There are also different implementations of memory managers.

|  Folder                                               |         Files                                                   |
|-------------------------------------------------------|-----------------------------------------------------------------|
|  FreeRTOSv10.0.0/FreeRTOS/Source/include/             | croutine.h mpu_prototypes.h stack_macros.h deprecated_definitions.h mpu_wrappers.h StackMacros.h event_groups.h portable.h FreeRTOS.h projdefs.h stream_buffer.h list.h queue.h task.h message_buffer.h semphr.h timers.h  |
|  FreeRTOSv10.0.0/FreeRTOS/Source                      |  croutine.c tasks.c event_groups.c list.c queue.c stream_buffer.c timers.c |
|  FreeRTOSv10.0.0/FreeRTOS/Source/portable/GCC/ARM_CM3 |  port.c and portmacro.h |
|  FreeRTOSv10.0.0/FreeRTOS/Source/portable/MemMang     |  heap_1.c heap_2.c heap_3.c heap_4.c heap_5.c


FreeRTOS is highly configurable. The main configuration is done by editing the FreeRTOSConfig.h file. Several examples of it can be found in the Demo folder. A tip is to copy one for a similar architecture to the project folder. In this case, the LM3S102 from Texas is a Cortex M3 too.

The contents of the FreeRTOSConfig.h includes the following:

    #define configUSE_PREEMPTION 1
    #define configUSE_IDLE_HOOK 0
    #define configUSE_TICK_HOOK 0
    #define configCPU_CLOCK_HZ ( ( unsigned long ) 48000000 )
    #define configTICK_RATE_HZ ( ( TickType_t ) 1000 )
    #define configMINIMAL_STACK_SIZE ( ( unsigned short ) 100 )
    #define configTOTAL_HEAP_SIZE ( ( size_t ) ( 2048 ) )
    #define configMAX_TASK_NAME_LEN ( 3 )
    #define configUSE_TRACE_FACILITY 0
    #define configUSE_16_BIT_TICKS 0
    #define configIDLE_SHOULD_YIELD 0

	#define configUSE_CO_ROUTINES 0
	#define configMAX_PRIORITIES (2)
	#define configMAX_CO_ROUTINE_PRIORITIES (2)

	/* Set the following definitions to 1 to include the API function, 
    or zero to exclude the API function. 
    */

    #define INCLUDE_vTaskPrioritySet 0
    #define INCLUDE_uxTaskPriorityGet 0
    #define INCLUDE_vTaskDelete 0
    #define INCLUDE_vTaskCleanUpResources 0
    #define INCLUDE_vTaskSuspend 0
    #define INCLUDE_vTaskDelayUntil 0
    #define INCLUDE_vTaskDelay 1
    #define configKERNEL_INTERRUPT_PRIORITY 255 // configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero!!
    #define configMAX_SYSCALL_INTERRUPT_PRIORITY 191 // equivalent to 0xa0, or priority 5.

The most important parameters are:

-   *configCPU_CLOCK_HZ*
-   *configTOTAL_HEAP_SIZE*
-   *configTICK_RATE_HZ*

##Memory managers

There are five implementation of memory managers. A short description of each is shown below.


|      Implementation             | Description                                              |
|---------------------------------|----------------------------------------------------------|
|     heap_1.c                    | Does not free memory                                     |
|     heap_2.c                    | Uses a best fit allocation algorithm, but does not combine adjacent areas |
|     heap_3.c                    | Uses the malloc/free routines provided                                    |
|     heap_4.c                    | Full implementation but not deterministic                                 |
|     heap_5.c                    | Can use multiple memory pools                                             |


##Setting up a project

-   Setup a project folder
-   Configure FreeRTOS using the file FreeRTOSConfig.h in the project folder
-   Add the FreeRTOS source files in FreeRTOS/Source to project. It is not advisable to copy them to project folder.
-   Add the port.c in FreeRTOS/source/portable/GCC/ARM_CM3 to project.
-   Add the heap_X.c file to project (X can be 1,2,3,4 or 5, see above).
-   Add the following folders to the include search path: FreeRTOS/source/include, FreeRTOS/source/portable/GCC/ARM_CM3

When using CMSIS, it is necessary to add these lines to the FreeRTOSConfig.h file.

    #define vPortSVCHandler SVC_Handler
    #define xPortPendSVHandler PendSV_Handler
    #define xPortSysTickHandler SysTick_Handler

##Modifications to Makefile

1.  Insert the following lines at the beginning

         #
         # FreeRTOS Configuration
         #
         # FreeRTOS Dir
         FREERTOSDIR=../../FreeRTOSv10.0.0
         # Virtual path
         VPATH=$(FREERTOSDIR)/FreeRTOS/Source $(FREERTOSDIR)/FreeRTOS/Source/portable/GCC/ARM_CM3
         # FreeRTOS Include Path
         FREERTOSINCPATH=$(FREERTOSDIR)/FreeRTOS/Source/include  $(FREERTOSDIR)/FreeRTOS/Source/portable/GCC/ARM_CM3/
         # FreeRTOS Source Files
         FREERTOSFILES=croutine.c tasks.c event_groups.c list.c queue.c stream_buffer.c timers.c portable/GCC/ARM_CM3/port.c
		FREERTOSSRCFILES= $(addprefix $(FREERTOSDIR)/FreeRTOS/Source/,$(FREERTOSFILES))

1.  Modify the line

        OBJFILES=$(addprefix ${OBJDIR}/,$(SRCFILES:.c=.o))
        OBJFILES=$(addprefix ${OBJDIR}/,$(notdir $(SRCFILES:.c=.o) $(FREERTOSSRCFILES:.c=.o)))

1.  Modify the line

    	INCLUDEPATH= ${CMSISDEVINCDIR} ${CMSISINCDIR}

    to

	    INCLUDEPATH= . $(FREERTOSINCPATH) ${CMSISDEVINCDIR} ${CMSISINCDIR}

##Code


Before starting a small observation about symbol names in FreeRTOS. An old naming scheme, usual in the 90s, called Hungarian Notation, is still used in FreeRTOS. The idea is to incorporate some information in the name of a funcion or a variable. For example, function, whose name started with v returns no value, i.e., return void.

The main program must have the following structure:

1.  Include FreeRTOS header files

    #include "FreeRTOS.h" /* Must come first. */
    #include "task.h" /* RTOS task related API prototypes. */
    #include "queue.h" /* RTOS queue related API prototypes. */
    #include "timers.h" /* Software timer related API prototypes. */
    #include "semphr.h" /* Semaphore related API prototypes. */

1.  Include project header files, including manufacturer supplied files
2.  Define constants and parameters
3.  Implement a setup hardware function (initialization)
4.  Optionally, implement tasks, but this can be done in other source files.
5.  Implement the main as bellow

        int main(void) {
            // Setup hardware
            // Create queues and semaphores
            // Create tasks
            // Start FreeRTOS
            vTaskStartScheduler();
            // Just in case
            while (1) {}
        }

1.  Tasks are functions with the *void task(void \*param)* prototype and have the following structure

        void Task(void \*param) {
        // local variables

            // initialization
            // infinite loop

            while(1) { // Could be for(;;) {
                // actions
                // wait for something. vTaskDelay, vQueueReceive, xSemaphoreGet, etc.
            }
            // never reached
        }

*param* can be used to share code. For example, the same routine can be used for different UARTs or ADCs.

The *wait for something* is essential. Without it, tasks with lower priority would not run.

##More information

[FreeRTOS](https://www.freertos.org/)[22]
[FreeRTOS on Cortex M3/4](https://www.freertos.org/RTOS-Cortex-M3-M4.html)[23]
