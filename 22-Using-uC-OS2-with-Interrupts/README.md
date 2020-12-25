19  Serial Communication under uC/OS-II
=======================================

uC/OS II is a real time preemtive real time kernel. It was a proprietary.  It was was bought by Silicon Labs and now it is a open source project hosted on github.

There are two ways to implement serial communication in software:

* Polling. The UART is checked often, if it has received any data. 
* Interrupt. When a character is received, the UART generates an interrupt.

By polling, if the UART is not checked often enough, there is a danger of a data loss.  In other hand, the increase of check frequency leads to a high CPU load.

The use of interrupt is challenging, because there is a hazard risk between the access to data done in the interrupt routine and the access done in the main task.

If a kernel is not used, access can be blocked to interrupt routines by disabling interrupts. This can be done only for very short time to avoid noticeable jitter.

When a kernel is used, the guidelines specificied for kernel can be used. One method is to disable interrupts as above, but this is not always practical.

A rule is "Interrupt routines should never wait".

When the processing demands long times, a common approach is to divide the processing in a bottom and a top parts.
The bottom part is done in the interrupt routine. The top part, in a task.

    OSSemaphore S;

    void InterruptRoutine(void) {
    
        OSEnterInt();
        OSSemPost(S);           // Free semaphore
        OSExitInt();
    
    }
    
    int TaskProcessing() {

        OSCreateSem(S);
        S=1;
        while(1) {
            OSSemPend(S);       // Wait for semaphore
            // Process
        }
    }

Other mechanisms,like messages, can be used too.





##More information

* [uC/OS II on Cortex M](https://www.state-machine.com/qpc/ucos-ii.html)[26]
* [uC/OS II Book](https://www.micrium.com/download/µcos-ii-the-real-time-kernel-2nd-edition/)[27]
* [uC/OS II on Cortex M4 Book](https://www.micrium.com/download/µcos-ii-the-real-time-kernel-for-the-freescale-kinetis/)[28]
* [uC/OS II port on Cortex M3 and M4](https://github.com/tony/gpc/tree/master/3rd_party/uCOS-II)[29]
* [us/os II port on Cortex M3 Application Note](https://www.element14.com/community/docs/DOC-35592/l/micrium-an1018-application-note-for-μcos-ii-and-the-arm-cortex-m3-processors)[30]
