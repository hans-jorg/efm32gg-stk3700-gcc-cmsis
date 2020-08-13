15  Newlib
==========

##A port of stdio library to embedded systems

To enhance portability, a multiple layer systems composed of libraries of functions was added to the C language. In UNIX/Linux machines, the different layers are shown below.


    +-----------------------------------+
    |                                   |
    |         Application               |
    |                                   |
    +-----------------------------------+
    |                                   |
    |           C Lib                   |
    |                                   |
    +-----------------------------------+
    |                                   |
    |           POSIX                   |
    |                                   |
    +-----------------------------------+
    |                                   |
    |      Operating System             |
    |                                   |
    +-----------------------------------+
    |                                   |
    |           Hardware                |
    |                                   |
    +-----------------------------------+

For clarification, both the C standard library and the POSIX provide functions to open, read, write and close files. The C standard library provides much more, including a stream based abstraction to I/O and a mathematical library.

    +------------------+-----------------+-----------------+-----------------+-----------------+
    |   Library        | Open a file     | Read from file  | Write to file   | Close file      |
    +------------------+-----------------+-----------------+-----------------+-----------------+
    |   C lib          |    fopen        |   fread         |   fwrite        |   fclose        |
    +------------------+-----------------+-----------------+-----------------+-----------------+
    |  POSIX           |    open         |   read          |   write         |   close         |
    +------------------+-----------------+-----------------+-----------------+-----------------+
    |  C99/C++ conform |   _open         |  _read          |  _write         |  _close         |
    +------------------+-----------------+-----------------+-----------------+-----------------+


In UNIX/Linux machines there is, for historical and compatibility, the symbols (function names, struct names, etc.) share the same space, which can lead to unexpected name collision (Try to give the name write to a function in your application!!). The correct way is to preprend an underscore (_) to these names, because all symbols started by underscore are reserved for the implementation.

##Newlib

A full implementation of the standard C library is part of the embedded arm gcc. It is based on [newlib](https://sourceware.org/newlib)[20]. The capacity of the library is only limited by the board hardware. For example, if there is no file system, all functions related to files are limited to *stdin* and *stdout*/*stderr*.



    +--------------------------------------------------------------------------------+
    |                                                                                |
    |                              Application                                       |
    |                                                                                |
    +--------------------------------------------------------------------------------+
    |                                                                                |
    |                              Newlib                                            |
    |                                                                                |
    |                                 +----------------------------------------------+
    |                                 |                                              |
    |                                 |               libgloss                       |
    |                                 |                                              |
    +---------------------------------+----------------------------------------------+
    |                                                                                |
    |                    Hardware                                                    |
    |                                                                                |
    +--------------------------------------------------------------------------------+


The libgloss is the glue between software and hardware. It includes routines that depends only on the microcontroller architecture (e.g. Cortex-M3) and routines that depends on the microcontroller and board.

The newlib includes the following libraries:

-   *libc.a*: standard c library
-   *libm.a*: standard mathematical library
-   *libnosys.a*: library with stub
-   *libc-nano.a*: small footprint libc optimized for bare bone systems (without operating system)
-   *libg.a*: other name for *libc.a*
-   *libgcc.a*: routines needed in the code generated by the compiler.

There are different versions of the newlib, in the lib folder. The compiler will use one of them according the command line parameters.

    ./thumb/v8-m.main/fpv5-sp/hard/libc.a
    ./thumb/v8-m.main/fpv5-sp/softfp/libc.a
    ./thumb/v8-m.main/fpv5/hard/libc.a
    ./thumb/v8-m.main/fpv5/softfp/libc.a
    ./thumb/v8-m.main/libc.a
    ./thumb/v7-ar/fpv3/hard/libc.a
    ./thumb/v7-ar/fpv3/softfp/libc.a
    ./thumb/v7-ar/libc.a
    ./thumb/v8-m.base/libc.a
    ./thumb/v7e-m/fpv5/hard/libc.a
    ./thumb/v7e-m/fpv5/softfp/libc.a
    ./thumb/v7e-m/fpv4-sp/hard/libc.a
    ./thumb/v7e-m/fpv4-sp/softfp/libc.a
    ./thumb/v7e-m/libc.a
    ./thumb/v6-m/libc.a
    ./thumb/v7-m/libc.a
    ./thumb/libc.a
    ./hard/libc.a
    ./libc.a

One of the problems of using a library is that, inadvertently, a lot of dependencies is generated and the code size exploeds. For example, *printf* includes support for floating point, and using it, all support for floating point is added, and this is big for processors without floating point units.

To avoid it, newlib provides a *iprintf* without support for floating point.

##Compiling using newlib

In the previous examples, the library was not used. Symbols from the library were defined outside it and there was no need to use objects from the library.

NewliB depends only on a set of functions, that mimic the corresponding POSIX functions. The functions that must be implemented are shown it the table below.

|   &nbsp;    |   &nbsp;    |  &nbsp;    |   &nbsp;    |  &nbsp;    |   &nbsp;   | &nbsp;    | &nbsp;     |  &nbsp;    |
|-------------|-------------|------------|-------------|------------|------------|-----------|------------|------------|
| _exit       | close       | environ    | execve      | fork       | fstat      | getpid    | isatty     | kill       |
| link        | lseek       | open       | read        | sbrk       | stat       | times     | unlink     | wait       |
| write       |   &nbsp;    |    &nbsp;  |     &nbsp;  |   &nbsp;   |   &nbsp;   |   &nbsp;  |    &nbsp;  |    &nbsp;  |


Most of these function can be only a stub, returning a (non) fatal error or other meaningful result. Exceptions are the read and write function, that redirect the input and output to UART;

The makefile is configured to use the nano version of the libraries. Commenting the line *SPECFLAGS= --specs=nano.specs* the normal libraries are used. The spec file specifies a rewriting of the linker parameters.

The size of the code generated can be obtained by the command

	arm-none-eabi-size gcc/uart-cdc.axf

The results of using nano or normal version of the libraries and the ministdio (last example) can be compared in the table below.


| Section   | Size using nano| Size using normal | Size using ministdio |
|-----------|-----:|----------------------------:|---------------------:|
  Code      | 9079 |     27437                   |            5282      |
  Data      |  108 |      2484                   |               8      |
  BSS       |  276 |       356                   |             256      |
|-----------|-----:|----------------------------:|---------------------:|
  Total     | 9463 |     30277                   |            5546      |
