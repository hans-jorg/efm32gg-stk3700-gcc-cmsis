19  Using uC/OS-II
==================

uC/OS II is a proprietary[24] real time preemptive kernel with a small footprint. It can be downloaded from www.micrium.com for non commercial purposes. If it is used in a commercial product, there are license fees.

The main features of uC/OS II are:

-   Preemptive
-   Coded in (mainly) C
-   Portable
-   Fixed priority scheduling
-   Up to 255 tasks
-   Semaphores, Mailboxes, Queues, Timers and other mechanisms
-   Small fooprint, but each task must have a stack

It was programmed mainly in C, with a small part, dependent on the compiler and processor, in C and Assembly. This enables the port of this kernel to other processors and compilers.

To port the uC/OS to another system, the following requirements must be attended (See uC/OS II book, page 350):

1.  The processor has a C compiler that generates reentrant code.
2.  Interrupts can be disabled and enabled from C.
3.  The processor supports interrupts and can provide an interrupt that occurs at regular intervals (typically between 10 and 100Hz).
4.  The processor supports a hardware stack that can accommodate a fair amount of data (possibly many kilobytes).
5.  The processor has instructions to load and store the stack pointer and other CPU registers, either on the stack or in memory

##Download

It couçld be downloaded from Micrium website and a register required). But now, after Micrium was acquired by SiliconLabs, it is now a open source project, housted on  [github](https://github.com/Micrium/uC-OS2). The license now permits commercial use.

It is composed of a small set of C source and header files common to all platforms and another set, specific to a target. There are also different implementations of memory managers.

|  Folder                                                      |         Files                                             |
|--------------------------------------------------------------|------------------------------------------------------ ----|
|  Micrium/Software/uCOS-II/Source                             |  os_cfg_r.h  os_dbg_r.c  os_mbox.c  os_mutex.c  os_sem.c  |
|                                                              |  os_time.c  ucos_ii.c os_core.c   os_flag.c   os_mem.c    |
|                                                              |  os_q.c os_task.c  os_tmr.c   ucos_ii.h                   |
| Micrium/Software/uCOS-II/Ports/ARM-Cortex-M3/Generic/GCC     |  os_cpu.h    os_cpu_a.asm os_cpu_c.c os_dbg.c             |
|  Project Folder                                              |  os_cfg.h includes.h                                      |


The uC/OS II version from Micrium website lacks the port folder entirely. Ports available in the Micrium do not use GCC compiler. A Cortex M3 port for GCC could be found in [github](https://github.com/huyugui/STDFS/tree/master/lcd-demo/ucos/uCOS-II/Ports/ARM-Cortex-M3/Generic/GCC)[25]. *But beware!! Both must be compatible. i.e. same version.*

##Configuration

uC/OS is highly configurable. The main configuration is done by editing the file *os_cfg.h*.

##Modifications to Makefile

1.  Insert the following lines at the beginning
2.  Modify the line
3.  Modify the line

##Tasks

The uC/OS has two low priorities already installed:

-   Idle task with the lowest possible priority (*OS_LOWEST_PRIO*)
-   Stats task with the next lowest possible priorit (*OS_LOWEST_PRIO-1*), which can be disabled by setting *OS_TASK_STAT_EN* to 0.

The tasks are identified by their priorities. So, no two tasks with the same priority.

From uC/OS II book:

> If your application uses the statistic task, you must call OSStatInit() (see OS_CORE.C ) from the first and only task created in your application during initialization. In other words, your startup code must create only one task before calling OSStart() . From this one task, you must call OSStatInit() before you create your other application tasks. The single task that you create is, of course, allowed to create other tasks, but only after calling OSStatInit().

The initialization task must have the highest priority, i.e. 0, because if it creates a higher priority task, a switch occurs immediately, and it can takes a long time to create the other tasks.

A task generally implements a infinite loop as bellow.

    void Task(void \*pdata) {
    // local variables
        // initialization
        // infinite loop
        while(1) { // Could be for(;;)
            // actions
            // wait for something: OSTaskDelay, OSSemPend, etc.
        }
    }

Another important point from uC/OS II book (page 134) is

> You must enable ticker interrupts after multitasking has started, that is, after calling OSStart() . In other words, you should initialize ticker interrupts in the first task that executes following a call to OSStart(). A common mistake is to enable ticker interrupts after OSInit() and before OSStart() , as shown in Listing 3.22. Potentially, the tick interrupt could be serviced before μC/OS-II starts the first task. At this point, μC/OS-II is in an unknown state, so your application crashes.

The start task, generally called *StartTask*, has the following pattern.

    void TaskStart(void \*param) {
    #if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr;
    #endif
         // Initialize
         ...
         // Start timer
         ...
         // Initialize statistics
         OSStatInit();

         // Create tasks
         OSCreateTask(Task1,.......);
         OSCreateTask(Task2,.......);
         OSCreateTask(Task3,.......);

         while(1) { // Could be for(;;)
            OSTimeDly(OS_TICKS_PER_SEC); // Could be OSTimeDlyHMSM(0,0,1,0);
            }
	}

The infinite loop can be replaced by *OSTaskDel(0)*, which autodelete the *TaskStart*.

##Code

1.  Implement an os_app.h

        #ifndef OS_APP_H
        #define OS_APP_H
            /* Nothing yet \*/
        #endif

2.  Implement an os_conf.h header file. Better copy one from an example in Examples folder or the os_conf_r.h from sources folder and modify it.

3.  Implement a main function with the following pattern:

        ´´´
        void TaskStart(void *param) {
        #if OS_CRITICAL_METHOD == 3
        OS_CPU_SR cpu_sr;
        #endif

            // Initialize
            ...
            // Start timer
            ...
            // Initialize statistics
            OSStatInit();

            // Time enough to calculate Maximal number of idle task activation
            OSTimeDly(2);

            // Create tasks
            OSCreateTask(Task1,.......);
            OSCreateTask(Task2,.......);
            OSCreateTask(Task3,.......);

            OSTaskDel(0); // Auto destroy
            }

        }

        void main(void) {
        // Local variables
        ...
        // Setup hardware but do not enable timer
        ...
        // Initialize uC/OS
        OSInit();

        // Create Semaphores, Events, MessageQueue etc
        ...
        // Create a single task (TaskStart) with priority 0 (highest),
        // which will create the other tasks
        OSTaskCreate(TaskStart, void \*) 0, TaskStartStack, 0);
        // Start uC/OS
        OSStart();
    }
    ´´´'

# Modification of project files

Besides the Makefile, other file must be modified in order to point to interrupt routines inside the uc/os code.

The lines defining the interruptio functions

    void PendSV_Handler(void)     __attribute__ ((weak, alias("Default_Handler")));
    void SysTick_Handler(void)    __attribute__ ((weak, alias("Default_Handler")));

must be modified to

    /* Modified or uc/os-ii                                                       */
    void OS_CPU_PendSVHandler(void)     __attribute__ ((weak, alias("Default_Handler")));
    void OS_CPU_SysTickHandler(void)    __attribute__ ((weak, alias("Default_Handler")));

The lines specifying the interrupt vector

    OS_CPU_PendSVHandler,                     /*      PendSV Handler              */
    OS_CPU_SysTickHandler,                    /*      SysTick Handler             */

must be modified to

    /* Modified or uc/os-ii                                                       */
    OS_CPU_PendSVHandler,                     /*      PendSV Handler              */
    OS_CPU_SysTickHandler,                    /*      SysTick Handler             */

##More information

[uC/OS II on Cortex M](https://www.state-machine.com/qpc/ucos-ii.html)[26]
[uC/OS II Book](https://www.micrium.com/download/µcos-ii-the-real-time-kernel-2nd-edition/)[27]
[uC/OS II on Cortex M4 Book](https://www.micrium.com/download/µcos-ii-the-real-time-kernel-for-the-freescale-kinetis/)[28]
[uC/OS II port on Cortex M3 and M4](https://github.com/tony/gpc/tree/master/3rd_party/uCOS-II)[29]
[us/os II port on Cortex M3 Application Note](https://www.element14.com/community/docs/DOC-35592/l/micrium-an1018-application-note-for-μcos-ii-and-the-arm-cortex-m3-processors)[30]
