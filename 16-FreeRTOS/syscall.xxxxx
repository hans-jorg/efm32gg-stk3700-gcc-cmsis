/**
 * @file     syscall.c
 * @brief    System calls for newlib
 * @version  V1.0
 * @date     23/01/2016
 *
 * @note     See https://sourceware.org/newlib/libc.html#Syscalls
 * @note     All names start with underscore (Standard C)
 * @note     See www.sourceware.com/newlib
 *
 **/


#include <stdlib.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <locale.h>

#include "TM4C123GH6PM.h"
#include "uart.h"

/**
 * @brief Interface to a serial interface in a minimal implementation
 *
 */
//@{
void SerialInit(void)           { UART_Init();                }
void SerialWrite(char c)        { UART_SendChar(c);           }
int  SerialRead(void)           { return UART_ReceiveChar();  }
int  SerialStatus(void)         { return UART_GetStatus();    }
//@}

/**
 * @brief Location of heap
 *
 * @note Heap locate between end of BSS area and end of Stack
 *
 * @note Defined by the linker
 * @note Stack runs from higher to lower addresses
 * @note See symbols been defined in linker script tm4c123.ld
 *
 */
//@{
extern char _StackTop;
extern char _end;
extern char _bss_end;       /* Defined by the linker */
//@}

/*
 * GetStackPointer
 */
static inline uint32_t GetStackPointer(void) { return __get_MSP(); }
/**
 * @brief errno
 *
 * @note  The C library must be compatible with development environments that
 *        supply fully functional versions of these subroutines. Such
 *        environments usually return error codes in a global errno. However,
 *        the Red Hat newlib C library provides a macro definition for errno
 *        in the header file errno.h, as part of its support for reentrant
 *        routines (see Reentrancy).
 *
 * @note  The bridge between these two interpretations of errno is
 *        straightforward: the C library routines with OS interface calls
 *        capture the errno values returned globally, and record them in
 *        the appropriate field of the reentrancy structure (so that you can
 *        query them using the errno macro from errno.h).
 *
 * @note  This mechanism becomes visible when you write stub routines for OS
 *        interfaces. You must include errno.h, then disable the macro
 *        like below.
 */

#include <errno.h>
#undef errno
extern int errno;

/**
 * @brief _main
 *
 * @note  Initializes library.
 *        Must be called before entering main.
 *        Best place is inside _main in system_DEVICE.c or more simple, replace it
 */

void __sinit(struct _reent *reent);
extern struct _reent *_impure_ptr;

void _main(void) {
    __sinit(_impure_ptr);

    SerialInit();
}

/**
 * @brief _exit
 *
 * @note  Exit a program without cleaning up files. If your system doesn’t
 *        provide this, it is best to avoid linking with subroutines that
 *        require it (exit, system).
 * @note Should not be used.
 *       Just in case.
 */

void _exit(void) {

}


/**
 * @brief _read
 *
 * @note  Read from a file.
 *        Minimal implementation.
 */

int _read(int file, char *ptr, int len) {
int nc = 0;
char c;
int nb;

    while( nc < len ) {
        if( SerialStatus()==0 )
            break;
        c = SerialRead();
        *ptr++ = c;
        nc++;
    }
    return nc;
}

/**
 * @brief _write
 *
 * @note  Write to a file. libc subroutines will use this system routine
 *        for output to all files, including stdout—so if you need to generate
 *        any output, for example to a serial port for debugging, you should
 *        make your minimal write capable of doing this.
 * @note  The following minimal implementation is an incomplete example;
 *        it relies on a outbyte subroutine (not shown; typically, you must
 *        write this in assembler from examples provided by your hardware
 *        manufacturer) to actually perform the output.
 */

int _write(int file, char *ptr, int len) {
int todo;

    for (todo = 0; todo < len; todo++) {
        SerialWrite(*ptr++);
    }
    return len;
}

/**
 * @brief _close
 *
 * @note  Close a file.
 *        Minimal implementation
 */

int _close(int file) {
    return -1;
}

/**
 * @brief _environ
 *
 * @note  A pointer to a list of environment variables and their values.
 *        For a minimal environment, this empty list is adequate.
 */

char *__env[1] = { 0 };
char **environ = __env;

/**
 * @brief _execve
 *
 * @note  Transfer control to a new process.
 *        Minimal implementation (for a system without processes).
 */

int _execve(char *name, char **argv, char **env) {
    errno = ENOMEM;
    return -1;
}

/**
 * @brief fork
 *
 * @note  Create a new process.
 *        Minimal implementation (for a system without processes).
 */

int _fork(void) {
    errno = EAGAIN;
    return -1;
}

/**
 * @brief fstat
 *
 * @note  Status of an open file. For consistency with other minimal
 *        implementations in these examples, all files are regarded as
 *        character special devices.
 * @note  The sys/stat.h header file required is distributed in the include
 *        subdirectory for this C library.
 */

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/**
 * @brief _getpid
 *
 * @note  Process-ID; this is sometimes used to generate strings unlikely to
 *        conflict with other processes.
 *        Minimal implementation, for a system without processes.
 */

int _getpid(void) {
    return 1;
}

/**
 * @brief _isatty
 *
 * @note  Query whether output stream is a terminal.
 *        For consistency with the other minimal implementations, which only
 *        support output to stdout, this minimal implementation is suggested.
 */

int _isatty(int file) {
    return 1;
}

/**
 * @brief _kill
 *
 * @note  Send a signal.
 *        Minimal implementation.
 */

int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

/**
 * @brief _link
 *
 * @note  Establish a new name for an existing file.
 *        Minimal implementation.
 */

int _link(char *old, char *new) {
    errno = EMLINK;
    return -1;
}

/**
 * @brief _lseek
 *
 * @note  Set position in a file.
 *        Minimal implementation.
 */

int _lseek(int file, int ptr, int dir) {
    return 0;
}

/**
 * @brief _open
 *
 * @note
    Open a file. Minimal implementation:
 */

int _open(const char *name, int flags, int mode) {
    return -1;
}

/**
 * @brief _sbrk
 *
 * @note  Increase program data space. As malloc and related functions depend
 *        on this, it is useful to have a working implementation.
 *        The following suffices for a standalone system; it exploits the
 *        symbol _heap_end defined in linker script
 */

void * _sbrk(int incr) {
static char *heap_end = 0;
char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_bss_end;
    }
    prev_heap_end = heap_end;
    // Should be present SP instead of StackTop
    if ( (uint32_t)heap_end + incr > GetStackPointer() ) {
        abort ();
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

/**
 * @brief _stat
 *
 * @note  Status of a file (by name).
 *        Minimal implementation.
 */

int _stat(const char *path, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/**
 * @brief _times
 *
 * @note  Timing information for current process.
 *        Minimal implementation.
 */

clock_t _times(struct tms *buf) {
    return -1;
}

/**
 * @brief _unlink
 *
 * @note  Remove a file’s directory entry.
 *        Minimal implementation.
 */

int _unlink(char *name) {
    errno = ENOENT;
    return -1;
}

/**
 * @brief _wait
 *
 * @note  Wait for a child process.
 *        Minimal implementation.
 */

int wait(int *status) {
    errno = ECHILD;
    return -1;
}


/**************************************************************************************************/

