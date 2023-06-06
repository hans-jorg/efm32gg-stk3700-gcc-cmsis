##
#  @file     Makefile
#  @brief    General makefile for EMF32GG
#  @version  v1.3
#  @date     23/09/2021
#
#  @note     CMSIS library used
#
#  @note     It works now with gnuwin32 Make. It can be find in
#            http://gnuwin32.sourceforge.net/packages/make.htm
#
#  @note options
#  @param all      generate image file
#  @param build    generate the axf file
#  @param flash    write image file into microcontroller
#  @param edit     open source files for editing
#  @param debug    creates debug session (default: gdb}
#  @param   gdb        creates debug session with gdb (same as debug}
#  @param   tui        creates debug session with gdb text user interface
#  @param   ddd        creates debug session with ddd
#  @param   nemiver    creates debug session with nemiver
#  @param openocd  start openocd for debugging
#  @param proxy    start GDB proxy
#  @param size     show code size
#  @param nm       list symbols
#  @param dump     show object code in assembly format
#
#
#  @note        There is support for JLink and OpenOCD.
#               The OpenOCD is not fully tested
#


###############################################################################
# Main parameters                                                             #
###############################################################################


#
# Program name
#
PROGNAME=tt2

#
# Verbose output for Make (Coment to have verbose output}
#
#MAKEFLAGS+= --silent --no-print-directory
.SILENT:

#
# Compatibility Windows/Linux
#
ifeq (${OS},Windows_NT)
HOSTOS :=Windows
else
HOSTOS :=${shell uname -s}
endif

#
# Test if it is run inside or by vs code. If so, disable some unneeded targets like edit,. gdb. tui. ...
#
ifdef VSCODE_CLI
INSIDEVSCODE:=yes
else
INSIDEVSCODE:=no
endif

#
# Defines the part (device} that this project uses
PART=EFM32GG990F1024
# Used to find correct CMSIS include file
PARTCLASSCMSIS=EFM32GG

#
# Source files
#
SRCFILES=${wildcard *.c}
#SRCFILES= main.c

#
# Include the common make definitions.
#
PREFIX:=arm-none-eabi

#
# Set debug mode
#
DEBUG=y

#
# Gecko_SDK DIr
#
GECKOSDKDIR=../../Gecko_SDK

#
# CMSIS header files folders (ARM and manufacturer specific)
#
# Use CMSIS inside Gecko SDK
CMSISDIR=${GECKOSDKDIR}/platform/CMSIS
CMSISDEVINCDIR=${GECKOSDKDIR}/platform/Device/SiliconLabs/EFM32GG/Include


#
# Commands Linux/Windows
#
ifeq (${HOSTOS},Windows)
RM=rmdir /s /q
MKDIR=mkdir
MV=move
CP=copy
NOOUT= > NUL
else
RM=rm -rf
MKDIR=mkdir -p
MV=mv
CP=cp
NOOUT=
endif


#
# When using VSCODE
#

ifeq (${INSIDEVSCODE},yes)
#
# SVC Dir contains the SVC files.
# This is only needed for the  Cortex Debug extension of VS Code
#
# See https://www.silabs.com/community/mcu/32-bit/knowledge-base.entry.html/
#     2017/07/04/svd_file_for_efm32d-gRFh
#
SVCDIR :=../../SVD-${PARTCLASSCMSIS}
endif

#
# Default target
#
#default: usage
default: build

#
# Compiler parameters
#   See readme.txt in the the share/doc/gcc-arm-none-eabi/ subfolder
#   of the GNU ARM installation folder
#
#   The EFM32GG990F1024 is a Cortex M3 processor with  a1024 MByte Flash memory and 
#   a 128KByte RAM.
#   It has no FPU!. So it will use a software emulation of the FPU (default)
#
#   The default printf and scanf functions do not support floating point data.
#   In order to use printf and scanf that support floating point, 
#   the *-u _printf_float* and *-u scanf_float* parameters must be set during compilation
#
CPUFLAGS= -mcpu=cortex-m3 -mthumb
FPUFLAGS= -mfloat-abi=soft
#
# Specs script (modification of compiler and linker flags}
#
# This parameter is only recognized by gcc.
# Linking must be done by a gcc call instead of ld
#
# Alternatives are:
#   nosys.specs:    no libc or libm
#   nano.specs:     minimal libc (newlib-nano)
#   rdimon.specs:   semihosting (serial interface thru debug lines)
#   rdpmon.specs:   RDP
#   redboot.specs:
#   picolibc.specs:
#
SPECFLAGS= --specs=nano.specs

ifeq (${INSIDEVSCODE},no)
#
# default debugger
#
debug: gdb
#debug: tui
#debug: ddd
#debug: nemiver
endif

#
# Folder for object files
#
OBJDIR=gcc

#
# Object files
#
OBJFILES=${addprefix ${OBJDIR}/,${SRCFILES:.c=.o}}


#
# Use sections to optimize code generation.
# Functions and data are put in separated sections.
# The linker can drop a (function or data) section
# if there is no reference to i
#

ASECTIONS=  -ffunction-sections                     \
            -fdata-sections                         \


CSECTIONS=  -ffunction-sections                     \
            -fdata-sections                         \

LSECTIONS=  -gc-sections


#
# C Error and Warning Messages Flags
#
CERRORFLAGS=                                        \
            -Wall                                   \
            -std=c11                                \
            -pedantic                               \

#
# Include path
#
CMSISINCDIR= ${CMSISDIR}/Include
INCLUDEPATH= ${CMSISDEVINCDIR} ${CMSISINCDIR}


###############################################################################
# Compilation parameters                                                      #
###############################################################################


#
#
# The flags passed to the assembler.
#
AFLAGS=                                             \
            ${CPUFLAGS}                             \
            ${FPUFLAGS}                             \
            ${addprefix -I ,${INCLUDEPATH}}         \
            ${ASECTIONS}                            \


#
# The flags passed to the compiler.
#
CFLAGS=                                             \
            ${CPUFLAGS}                             \
            ${FPUFLAGS}                             \
            ${addprefix -I ,${INCLUDEPATH}}         \
            ${CSECTIONS}                            \
            -D${PART}                               \

#
# To generate map output file
#
MAPFLAGS=  -Map,${OBJDIR}/${PROGNAME}.map

#
# The flags passed to the linker.
#
# For now only one parameter when using gcc to link
#
LDFLAGS=                                            \
            ${LSECTIONS}                            \
            ${MAPFLAGS}                             \


#
# Generate debug version if DEBUG is set
#
ifneq (${DEBUG},)
CFLAGS+=-g -D DEBUG -O0
else
CFLAGS+=-Os
endif

#
# Additional Flags
#
CFLAGS+= -Wuninitialized -Werror

#
# Controlling dependencies on header files
#
DEPFLAGS=-MT $@  -MMD -MP -MF ${OBJDIR}/$*.d

#
# libraries linked
#
# Thery are modified by the specs files
#
#LIBS=-lm -lc -lgcc
LIBS=
#
#
#

###############################################################################
# Generally it is not needed to modify the lines bellow                       #
###############################################################################


###############################################################################
#   Commands                                                                  #
###############################################################################

#
# Terminal application (used to open new windows in debug}
#
#TERMAPP=xterm
TERMAPP=gnome-terminal

#
# The command for calling the compiler.
#
CC=${PREFIX}-gcc

#
# The command for calling the library archiver.
#
AR=${PREFIX}-ar

#
# The command for calling the linker
# Using gcc instead of ld to use --specs parameters
# To try:
#     -ffunction-sections
#     -fdata-sections
#     -fno-builtin
#     -nostdlib
#     -nodefaultlibs
#  ???
#
COMMA=,
LD=${PREFIX}-gcc ${CFLAGS} -nostartfiles  ${addprefix -Wl${COMMA},${LDFLAGS}}
#LD=${PREFIX}-ld ${LDFLAGS}


#
# The command for extracting images from the linked executables.
#
OBJCOPY=${PREFIX}-objcopy

#
# The command for disassembly
#
OBJDUMP=${PREFIX}-objdump

#
# The command for listing size of code
#
OBJSIZE=${PREFIX}-size

#
# The command for listing symbol table
#
OBJNM=${PREFIX}-nm

#
# Debuggers
#

## GDB with and without TUI
GDB=${PREFIX}-gdb

## nemiver
NEMIVER=nemiver
NEMIVERFLAGS=

## ddd
DDD=ddd
DDDFLAGS=

## cdbg
CDBG=cdbg
CDBGFLAGS=

## kdbg
KDBG=kdbg
KDBGFLAGS=


#
# GUI Editor
#
GUIEDITOR=gedit --new-window
#GUIEDITOR=geany

#
# Doxygen
#
DOXYGEN=doxygen

#
# cflow to generate call graphs
#
CFLOW=cflow
CFLOWFLAGS=-l  -b --omit-arguments

#
# Options for flash and debug
#

USE_JLINK=yes
#USE_OPENOCD=yes


###############################################################################
#   Command parameters                                                        #
###############################################################################


ifeq (${USE_JLINK},yes)
#
# JLink
#
ifeq (${HOSTOS},Windows)
# For Windows
JLINK=JLink
GDBPROXY=JLinkGDBServer
JLINKPARMS= -Device ${PART} -If SWD -Speed 4000
JLINKFLASHSCRIPT=${OBJDIR}/flash.jlink
else
# For Linux
JLINK=JLinkExe
GDBPROXY=JLinkGDBServer
JLINKPARMS= -Device ${PART} -If SWD -Speed 4000
JLINKFLASHSCRIPT=${OBJDIR}/flash.jlink
endif
endif


ifeq (${USE_OPENOCD},yes)
#
# OpenOCD
#
OPENOCD=openocd
GPDBPROXY=openocd
OPENOCDSCRIPTDIR=/usr/share/openocd/scripts
OPENOCDFLAGS=  -f ${OPENOCDSCRIPTDIR}/interface/stlink-v2.cfg \
               -f ${OPENOCDSCRIPTDIR}/target/efm32_stlink.cfg
endif


#
# Flags for disassembler
#
ODDISFLAGS= -x -S -D
ODDUMPFLAGS= -x -s

#
# Doxygen parameters
#
DOXYGENCFG=Doxyfile

#
# Project name (for Doxygen}
#
PROJNAME=${shell basename ${CURDIR}}

#
# Serial terminal communication
#
TTYTERM=/dev/ttyACM0
TTYBAUD=115200

#
# Serial terminal emulator
#
# Use one of configuration below
# cu
   #TTYPROG=cu
   #TTYPARMS=-l ${TTYTERM} -s ${TTYBAUD}
# screen
   #TTYPROG=screen
   #TTYPARMS= ${TTYTERM} ${TTYBAUD}
# minicom
   TTYPROG=minicom
   TTYPARMS=-D ${TTYTERM} -b ${TTYBAUD}
# putty
   #TTYPROG=putty
# tip
   #TTYPROG=tip
   #TTYPARMS=-${TTYBAUD} ${TTYTERM}


#
# The flags passed to the debugger
#
GDBFLAGS=-x ${GDBINIT} -n

#
# GDB initialization file
#
GDBINIT=${OBJDIR}/gdbinit


#
# Linker script
#
#LINKERSCRIPT=${PROGNAME}.ld
LINKERSCRIPT=efm32gg.ld
#LINKERSCRIPT=${shell echo ${PART}| tr A-Z a-z}.ld

#
# Entry Point
#
ENTRY=Reset_Handler


###############################################################################
#   Rules                                                                     #
###############################################################################

#
# The rule for building the object file from each C source file.
#
${OBJDIR}/%.o: %.c
	@echo   "Compiling           ${<}"
	${CC} -c ${CFLAGS} ${SPECFLAGS} -D${OBJDIR} ${DEPFLAGS} -o ${@} ${<}

#
# The rule for building the object file from each assembly source file.
#
${OBJDIR}/%.o: %.S
	@echo   "Assembling          ${<}"
	${CC} -c ${AFLAGS} ${SPECFLAGS} -D${OBJDIR} -o ${@} -c ${<}

#
# The rule for creating an object library.
#
${OBJDIR}/%.a:
	@echo   "Archiving           ${@}"
	${AR} -cr ${@} ${^}

#
# The rule for linking the application.
#
# LDFLAGS are incorporated in the LD symbol, because
# it is used differently by gcc and ld
#
${OBJDIR}/%.axf:  ${OBJFILES}
	@echo   "Linking             ${@}"
	${LD}   -T '${LINKERSCRIPT}'        \
	       ${SPECFLAGS}                \
	       -Wl,--print-memory-usage    \
	       --entry '${ENTRY}'          \
	       -o ${@}                     \
	        ${OBJFILES}                \
	        ${LIBS}

${OBJDIR}/%.bin: ${OBJDIR}/%.axf
	@echo   "Generating binary   ${@}"
	${OBJCOPY} -O binary ${^} ${@}


###############################################################################
#   Targets                                                                   #
###############################################################################

#
#
#
help: usage
usage:
	@echo "Usage:"
	@echo "make all          generate image file in ${OBJDIR}"
	@echo "make build        generate .axf file in "
	@echo "make edit         open files in a editor"
	@echo "make flash        write to flash using ${FLASHER}"
	@echo "make clean        delete all generated files"
	@echo "make debug        creates debug session with gdb"
	@echo "make   gdb        creates debug session with gdb (same as debug}"
	@echo "make   tui        creates debug session with gdb text user interface"
	@echo "make   ddd        creates debug session with ddd"
	@echo "make   nemiver    creates debug session with nemiver"
	@echo "make dis          disassemble output file into ${OBJDIR}/${PROGNAME}.S"
	@echo "make dump         generate a hexadecimal dump file into ${OBJDIR}/${PROGNAME}.dump"
	@echo "make nm           list symbol table in standard output"
	@echo "make size         list size of output file"
	@echo "make term         open a terminal for serial communication to board"
	@echo "make openocd      start an openocd proxy"
	@echo "make proxy        start a gdb proxy"
	@echo "make docs         generates docs using doxygen"

#
# The default rule, which causes the ${PROGNAME} example to be built
# and generates a binary file, that is ready to be flashed.
#
all: ${OBJDIR} ${OBJDIR}/${PROGNAME}.bin
	@echo Done


#
# The default rule, which causes the ${PROGNAME} example to be built.
#
build: ${OBJDIR} ${OBJDIR}/${PROGNAME}.axf
	@echo Done

#
# Transfer binary to board
#
flash: deploy
burn: deploy
ifeq (${USE_JLINK},yes)
deploy:  ${OBJDIR} svd ${OBJDIR}/${PROGNAME}.bin ${JLINKFLASHSCRIPT}
	echo ${JLINK} ${JLINKPARMS} -CommanderScript ${JLINKFLASHSCRIPT}
	${JLINK} ${JLINKPARMS} -CommanderScript ${JLINKFLASHSCRIPT}

#
# Flash script
#
${JLINKFLASHSCRIPT}: FORCE
	echo r > ${JLINKFLASHSCRIPT}
	echo h >> ${JLINKFLASHSCRIPT}
	echo loadbin ${OBJDIR}/${PROGNAME}.bin,0 >> ${JLINKFLASHSCRIPT}
	echo exit >> ${JLINKFLASHSCRIPT}
FORCE:
endif


ifeq (${USE_OPENOCD},yes)
deploy: svd ${OBJDIR}/${PROGNAME}.bin ${OPENOCDFLASHSCRIPT}
	sudo ${OPENOCD} -f ${OPENOCDBOARD} -f ${OPENOCDFLASHSCRIPT}

#
# Flash script
#
${OPENOCDFLASHSCRIPT}:
	echo reset halt > ${OPENOCDFLASHSCRIPT}
	echo flash probe 0 >> ${OPENOCDFLASHSCRIPT}
	echo flash write_image erase ${OBJDIR}/${PROGNAME}.bin 0 >> ${OPENOCDFLASHSCRIPT}
	echo reset run >> ${OPENOCDFLASHSCRIPT}
endif

#
# Disassembling/Dump
#
dis: disassembly
disassembly: ${OBJDIR}/${PROGNAME}.axf
	@echo   "Disassembling ${^} and storing output in ${OBJDIR}/${PROGNAME}.S"
	${OBJDUMP} ${ODDISFLAGS} $^ > ${OBJDIR}/${PROGNAME}.S

dump: ${OBJDIR}/${PROGNAME}.axf
	@echo   "Dumping ${^} and storing output in ${OBJDIR}/${PROGNAME}.dump"
	${OBJDUMP} ${ODDUMPFLAGS} $^ > ${OBJDIR}/${PROGNAME}.dump
imagedump: ${OBJDIR}/${PROGNAME}.bin
	  $\{OBJDUMP} -x $^ > ${OBJDIR}/${PROGNAME}.hexdump

#
# List size
#
size: ${OBJDIR}/${PROGNAME}.axf
	${OBJSIZE} -A -x $^

#
# List size
#
nm: ${OBJDIR}/${PROGNAME}.axf
	${OBJNM} $^

#
# The rule to create the target directory.
#
${OBJDIR}:
	${MKDIR} ${OBJDIR}


#
# Rules for building the ${PROGNAME} example.
#
${OBJDIR}/${PROGNAME}.bin: ${OBJDIR}/${PROGNAME}.axf
${OBJDIR}/${PROGNAME}.axf: ${OBJFILES}

ifeq (${INSIDEVSCODE},no)
#
# Open files in editor windows
#
edit:
	${GUIEDITOR} Makefile *.c *.h *.ld &

#
# Debug with  gdb
#
gdb: ${OBJDIR}/${PROGNAME}.bin ${GDBINIT} gdbproxy
	${GDB} ${GDBFLAGS} ${OBJDIR}/${PROGNAME}.axf

#
# Debug using gdb with tui (text user interface}
#
tui: ${OBJDIR}/${PROGNAME}.bin ${GDBINIT} gdbproxy
	${GDB} --tui ${GDBFLAGS} ${OBJDIR}/${PROGNAME}.axf
#
# Debug using ddd
#
ddd: ${OBJDIR}/${PROGNAME}.bin ${GDBINIT} gdbproxy
	${DDD} ${DDDFLAGS} --debugger "${GDB} ${GDBFLAGS}" ${OBJDIR}/${PROGNAME}.axf
#
# Debug using nemiver
#
nemiver: ${OBJDIR}/${PROGNAME}.bin ${GDBINIT} gdbproxy
	${NEMIVER} ${NEMIVERFLAGS} --remote=localhost:2331  --gdb-binary=`which ${GDB}`     ${OBJDIR}/${PROGNAME}.axf
#
# Debug using cgdb GUI
#
cgdb: ${OBJDIR}/${PROGNAME}.bin ${GDBINIT}
	${CGDB} -d `which ${GDB}`  -x ${OBJDIR}/gdbinit ${OBJDIR}/${PROGNAME}.axf
#
# Debug using kdbg GUI
#
kdbg: ${OBJDIR}/${PROGNAME}.bin ${GDBINIT}
	${KDBG}  -r localhost:2331 ${OBJDIR}/${PROGNAME}.axf
endif

#
# Start GDB proxy
#
ifeq (${USE_JLINK},yes)
proxy:
gdbproxy:
	echo "Starting GDB Proxy"; \
	echo sh -c \" ${GDBPROXY} ${JLINKPARMS} \"  ; \
	${TERMAPP} -- ${GDBPROXY} ${JLINKPARMS}  ; \
#
# Create gdbinit
#
${GDBINIT}: FORCE
	echo # Run this script using gdb source command > ${GDBINIT}
	echo target remote localhost:2331 >> ${GDBINIT}
	echo monitor halt >> ${GDBINIT}
	echo load ${OBJDIR}/${PROGNAME}.axf >> ${GDBINIT}
	echo monitor reset >> ${GDBINIT}
	echo break main >> ${GDBINIT}
	echo monitor go >> ${GDBINIT}
endif

ifeq (${USE_OPENOCD},yes)
proxy:
gdbproxy:
	if [ -c ${TTYTERM} ] ; then echo "Starting OpenOCD"; \
	   ${TERMAPP} -- ${GDBPROXY} -f ${OPENOCDBOARD}  ; \
	else echo "Could not open ${GDBPROXY}"; fi
#
# Create gdbinit
#
${GDBINIT}:
	echo # Run this script using gdb source command > ${GDBINIT}
	echo target remote localhost:3333 >> ${GDBINIT}
	echo break main >> ${GDBINIT}
	echo continue >> ${GDBINIT}
endif

#
# Open a terminal windows
# OBS: Remove or disable modemmanager in case of busy device
#
terminal: term
term:
	if [ -c ${TTYTERM} ] ; then echo Starting terminal; \
	${TERMAPP} -- ${TTYPROG} ${TTYPARMS}  ; \
	else echo "Could not open ${TTYTERM}"; fi

#
# Generate documentation using doxygen
#
docs: doxygen
doxygen: ${DOXYGENCFG}
	${DOXYGEN} ${DOXYGENCFG}
	echo Done.

SEDSCRIPT=dox.sed
${DOXYGENCFG}:
	${DOXYGEN} -g ${DOXYGENCFG}
	echo /^PROJECT_NAME/cPROJECT_NAME           = \"${PROJNAME}\" > ${SEDSCRIPT}
	echo /^FULL_PATH_NAMES/cFULL_PATH_NAMES     = NO >> ${SEDSCRIPT}
	echo /^OPTIMIZE_OUTPUT_FOR_C/cOPTIMIZE_OUTPUT_FOR_C    = YES >> ${SEDSCRIPT}
	echo /^DISTRIBUTE_GROUP_DOC/cDISTRIBUTE_GROUP_DOC    = YES >> ${SEDSCRIPT}
	echo /^EXTRACT_STATIC/cEXTRACT_STATIC    = YES >> ${SEDSCRIPT}
	sed -i -f ${SEDSCRIPT} ${DOXYGENCFG}
	-${RM}   ${SEDSCRIPT}



#
# Clean the generated documentation
#
docs-clean:
	-${RM}  html latex && echo Done.

#
# Clean out all the generated files
#
clean:
	-$(RM) ${OBJDIR} ${wildcard *~} html latex docs NOOUT
	-$(RM) .vscode/*.log NOOUT
	echo Done

#
# Copy the SVD to OBJDIR folder
#
ifeq (${INSIDEVSCODE},yes)
svd: ${OBJDIR} ${OBJDIR}/${PART}.svd
else
svd:
endif
${OBJDIR}/${PART}.svd: FORCE
	-${CP} - ${SVCDIR}/${PART}.svd  ${OBJDIR}

#
# generates a call graph
#
CFLOWFLAGS=-l  -b --omit-arguments
cflow:
	{ ${CFLOWFLAGS} -D${PART} ${addprefix -I ,${INCLUDEPATH}} ${SRCFILES} 2>&1} | egrep -v "^cflow"


# These symbols are not files (or directories}
.PHONY: nm edit debug terminal term doxygen docs docs-clean cflow
.PHONY: all burn clean debug default deploy deploy dis disassembly
.PHONY: docs docs-clean doxygen dump edit flash gdbproxy proxy
.PHONY: help imagedump nm size term terminal usage svd

#
# Dependencies
#
-include ${OBJFILES:%.o=%.d}
