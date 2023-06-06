20  Using uC/OS-III
===================

##A newer version of uC/OS

uC/OS-iii is a proprietary real time preemptive kernel with a small footprint. It can be downloaded from www.micrium.com for non commercial purposes. If it is used in a commercial product, there are license fees.

The main features of uC/OS III are:

-   Preemptive
-   Coded in (mainly) C
-   Portable
-   Fixed priority scheduling
-   Unlimited number of tasks
-   Semaphores, Mailboxes, Queues, Timers and other mechanisms only limited by RAM
-   Small fooprint, but each task must have a stack

It was programmed mainly in C, with a small part, dependent on the compiler and processor, in C and Assembly. This enables the port of this kernel to other processors and compilers.

To port the uc/os to another system, the following requirements must be attended (See uc/os-iii book, page 350):

1.  The processor has a C compiler that generates reentrant code.
2.  Interrupts can be disabled and enabled from C.
3.  The processor supports interrupts and can provide an interrupt that occurs at regular intervals (typically between 10 and 100Hz).
4.  The processor supports a hardware stack that can accommodate a fair amount of data (possibly many kilobytes).
5.  The processor has instructions to load and store the stack pointer and other CPU registers, either on the stack or in memory.

It depends on two other packages: os/lib and os/cpu.

##Download

It can be downloaded from Micrium website (register required). ~~But the most visible version available does not include the required packages and lacks the port folder entirely. A better idea is to download it from the [download center](https://www.micrium.com/downloadcenter/)[31] or more specifically, the [Cor](https://www.micrium.com/downloadcenter/download-results/?searchterm=pa-cortex-m3&supported=true)[tex M3 port page](https://www.micrium.com/downloadcenter/download-results/?searchterm=pa-cortex-m3&supported=true)[32], one with the same or similar architecture. In this case, Texas Instruments LM3S9B92 is a good approximation, because it is a Cortex M3 too.~~

Another possibility is to download the [Simplicity Studio](https://www.silabs.com/products/development-tools/software/simplicity-studio)[33]. It includes a full uc/os iii for EFM32 devices.

But Silicon Labs open all code of uC/OS-III and related after the acquisition of Micrium. They can now be found in github.

The uC/OS iii depends on the additional packages.

-   -   uC/OS-III: the uC/OS-III
-   uC/LIB: non standard functions and macros to test characters, generate random numbers, manage memory, manipulate string, etc.
-   uC/CPU: manager (some) timers and clock frequency. Specifies some parameters like stack growth.
-   uC/CSP: (optionally) chip support funcions like GPIO pins, etc.


All ot them can be found on github.

Package	|	Repository
--------|-------------
uC-OS3  | https://github.com/Micrium/uC-OS3 
uC-CPU  | https://github.com/Micrium/uC-CPU
uC-LIB  | https://github.com/Micrium/uC-LIB

They can be downloaded as a zip file, or better, can be cloned using git.

    git clone https://github.com/Micrium/uC-OS3
    git clone https://github.com/Micrium/uC-CPU
    git clone https://github.com/Micrium/uC-LIB



##The uc/os-iii package

The uC/OS iii depends on the following packages:

-   uC/OS-III: the uC/OS-III
-   uC/LIB: non standard functions and macros to test characters, generate random numbers, manage memory, manipulate string, etc.
-   uC/CPU: manager (some) timers and clock frequency. Specifies some parameters like stack growth.
-   uC/CSP: (optionally) chip support funcions like GPIO pins, etc.

It is composed of a small set of C source and header files common to all platforms and another set, specific to a target. There are also different implementations of memory managers.

|  Folder                                               |         Files                                                   |
|-------------------------------------------------------|-----------------------------------------------------------------|
|  Micrium/Software/uCOS-III/Source                     | os_cfg_app.c os_core.c os_dbg.c os_flag.c os_int.c os_mem.c     |
|                                                       | os_msg.c os_mutex.c os_pend_multi.c os_prio.c os_q.c os_sem.c   |
|                                                       | os_stat.c os_tick.c os_time.c os_tmr.c os_var.c os_h os_type.h  |
|  Micrium/Software/uCOS-III/Ports                      | os_cpu_c.c os_cpu_a.asm os_cpu.h os_cpu_a.inc                   |
|  Micrium/Software/uC-CPU                              | cpu_core.c cpu_core.h cpu_def.h                                 |
|  Micrium/Software/uC-CPU/ARM-Cortex-M3/GCC            | cpu.h cpu_a.asm cpu_c.c                                         |
|  Micrium/Software/uC-LIB                              | lib_ascii.c lib_ascii.h lib_def.h lib_math.c lib_math.h         |
|                                                       | lib_mem.c lib_mem.h lib_str.c lib_str.h                         |
|  bsp                                                  | bsp.c bsp.h bsp_int.c bsp_int.h                                 |
|  project                                              | app.c app_hooks.c includes.h app_cfg.h cpu_cfg.h lib_cfg.h      |
|                                                       | os_app_hooks.h os_cfg.h os_cfg_app.h                            |


##Configuration

uC/OS-III is highly configurable. The configuration is done by editing header files.

TBD!!!!!!!!!!!!!!!!!


| File               |       Description                                        |
|--------------------|----------------------------------------------------------|
| app_cfg.h          |                        |
| cpu_cfg.h          | |
| csp_cfg.h          | |
| lib_cfg.h          | |
| os_app_hooks.h     | |
| os_cfg.h           | |
| os_cfg_app.h       | |
| os_type.h          | |
| os_cfg.h                      |          |
| os_app


##Modifications to Makefile

1.  Insert the following lines at the beginning
2.  Modify the line
3.  Modify the line

##Tasks

The uc/os has two low priorities already installed:

-   Idle task with the lowest possible priority (OS_LOWEST_PRIO)
-   Stats task with the next lowest possible priority (OS_LOWEST_PRIO-1), which can be disabled by setting OS_TASK_STAT_EN tgo 0.
-   Tick task

The tasks implements an infinite loop as bellow

    void Task(void *pdata) {
     // local variables
     // initialization
     	// infinite loop
        while(1) { // Could be for(;;)
         // actions
        // wait for something: OSTaskDelay, OSSemPend, etc.
        }
    }

From uC/OS III book, page 75

> A few important points are worth noting. For one thing, you can create as many tasks as you want before calling OSStart(). However, it is recommended to only create one task as shown in the example because, having a single application task allows μC/OS-III to determine the relative speed of the CPU. This allows μC/OS-III to determine the percentage of CPU usage at run-time. Also, if the application needs other kernel objects such as semaphores and message queues then it is recommended that these be created prior to calling OSStart(). Finally, notice that interrupts are not enabled.

This is emphasized again in page 134.

> If the application uses the statistic task, it should call OSStatTaskCPUUsageInit() from the first, and only application task created in the main() function as shown in Listing 5-5. The startup code should create only one task before calling OSStart(). The single task created is, of course, allowed to create other tasks, but only after calling OSStatTaskCPUUsageInit()." This means that the OSStatTaskCPUUsageInit must be run without any other task to calibrate the measurement of CPU usage.

The initialization task must have the highest priority, i.e.. 0, because if it creates a task with a higher priority, a switch occurs immediately, and it can takes a long time to create the other tasks. This task can enter an infinite loop or kill itself (using *OSTaskKill*).

The start task, generally called *StartTask*, has the following pattern.

    void TaskStart(void \*param) {
    #if OS_CRITICAL_METHOD == 3
    	OS_CPU_SR cpu_sr;
    #endif

        // Initialize
        ...
        // Start timer
        ...
        // Initialize statistics. Must be run as only task.
        OSStatTaskCPUUsageInit();
        // Create tasks
        OSCreateTask(Task1,.......);
        OSCreateTask(Task2,.......);
        OSCreateTask(Task3,.......);
        while(1) { // Could be for(;;)
        	OSTimeDly(OS_TICKS_PER_SEC); // Could be OSTimeDlyHMSM(0,0,1,0);
        }
    }

##Code

1.  Implement *os_app.h* file.

        #ifndef OS_APP_H
        #define OS_APP_H
         /* Nothing yet */
        #endif

1.  Implement the os_conf.h header file. Better copy one from an example in Examples folder or the *os_conf_r.h* from source folder and modify it.
2.  Implement a main function with the following pattern:

   	 void main(void) {
   		// Local variables
        ...
        // Setup hardware but do not enable timer
        ...
        // Initialize uc/os
        OSInit();
        // Create Semaphores, Events, MessageQueue etc
        ...
        // Create a single task (TaskStart) with priority 0 (highest),
        // which will create the other tasks
        OSTaskCreate(TaskStart, void \*) 0, TaskStartStack, 0);
        // Start uc/os
        OSStart();
    }

##More information

[uC/OS](https://www.micrium.com/)[34]
[uC/OS III books](https://www.micrium.com/books/ucosiii/)[35]
[Myths about uC/OS](http://www.electronicdesign.com/embedded-revolution/11-myths-about-cos)[36]
[uC/OS ii port on Cortex M3 Application Note](https://www.element14.com/community/docs/DOC-35592/l/micrium-an1018-application-note-for-μcos-ii-and-the-arm-cortex-m3-processors)[37]
