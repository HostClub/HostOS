#
# SCCS ID: %W%	%G%
#
# Makefile to control the compiling, assembling and linking of
# standalone programs in the DSL.  Used for both 4003-406 and
# 4003-506 (with appropriate tweaking).
#

#
# User supplied files
#
U_C_SRC = kernel/kalloc.c kernel/clock.c kernel/klibc.c kernel/queue.c kernel/process.c kernel/scheduler.c lib/sio.c \
	  kernel/stack.c kernel/syscall.c kernel/system.c lib/ulibc.c test/user.c
U_C_OBJ = build/kalloc.o build/clock.o build/klibc.o build/queue.o build/process.o build/scheduler.o build/sio.o \
	  build/stack.o build/syscall.o build/system.o build/ulibc.o build/user.o
U_S_SRC = kernel/klibs.S lib/ulibs.S
U_S_OBJ = build/klibs.o build/ulibs.o
U_LIBS	=

#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP2_CONFIG		enable SP2-specific startup variations
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG

#
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!!
#
# Compilation/assembly control
#

#
# We only want to include from the current directory and ~wrc/include
#
INCLUDES = -I.

#
# Compilation/assembly/linking commands and options
#
CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc -I- $(INCLUDES)

CC = gcc
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = as
ASFLAGS =

LD = ld

#		
# Transformation rules - these ensure that all compilation
# flags that are necessary are specified
#
# Note use of 'cpp' to convert .S files to temporary .s files: this allows
# use of #include/#define/#ifdef statements. However, the line numbers of
# error messages reflect the .s file rather than the original .S file. 
# (If the .s file already exists before a .S file is assembled, then
# the temporary .s file is not deleted.  This is useful for figuring
# out the line numbers of error messages, but take care not to accidentally
# start fixing things by editing the .s file.)
#

.SUFFIXES:	.S .b

.c.s:
	$(CC) $(CFLAGS) -S $*.c

#.S.s:
build/%.S: %.s
	$(CPP) $(CPPFLAGS) -o $*.s $*.S -C boot


#.S.o:
build/%.o: %.S
	$(CPP) $(CPPFLAGS) -o $*.s $*.S -C boot
	$(AS) -o $*.o $*.s -a=$*.lst
	$(RM) -f $*.s

.s.b:
	$(AS) -o build/$*.o $*.s -a=$*.lst
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b build/$*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

# Binary/source file for system bootstrap code

BOOT_OBJ = boot/bootstrap.b
BOOT_SRC = boot/bootstrap.S

# Assembly language object/source files

S_OBJ = build/startup.o build/isr_stubs.o $(U_S_OBJ)
S_SRC = boot/startup.S kernel/isr_stubs.S $(U_S_SRC)

# C object/source files

C_OBJ = build/c_io.o build/support.o $(U_C_OBJ)
C_SRC = lib/c_io.c boot/support.c $(U_C_SRC)

# Collections of files

OBJECTS = $(S_OBJ) $(C_OBJ)

SOURCES = $(BOOT_SRC) $(S_SRC) $(C_SRC)

#
# Targets for remaking bootable image of the program
#
# Default target:  usb.image
#

usb.image: boot/bootstrap.b build/prog.b build/prog.nl utils/BuildImage utils/Offsets #prog.dis 
	./BuildImage -d usb -o usb.image -b build/bootstrap.b build/prog.b 0x10000

floppy.image: boot/bootstrap.b build/prog.b build/prog.nl utils/BuildImage utils/Offsets #prog.dis 
	./BuildImage -d floppy -o floppy.image -b build/bootstrap.b build/prog.b 0x10000

build/prog.out: $(OBJECTS)
	$(LD) -o build/prog.out $(OBJECTS)

build/prog.o:	$(OBJECTS)
	$(LD) -o build/prog.o -Ttext 0x10000 $(OBJECTS) $(U_LIBS)

build/prog.b:	build/prog.o
	$(LD) -o prog.b -s --oformat binary -Ttext 0x10000 build/prog.o

#
# Targets for copying bootable image onto boot devices
#

floppy:	floppy.image
	dd if=floppy.image of=/dev/fd0

usbu:	usb.image
	dd if=usb.image of=/local/devices/usbu

usbl:	usb.image
	dd if=usb.image of=/local/devices/usbl

usbr:	usb.image
	dd if=usb.image of=/local/devices/usbr

#
# Special rule for creating the modification and offset programs
#
# These are required because we don't want to use the same options
# as for the standalone binaries.
#

utils/BuildImage:	utils/BuildImage.c
	$(CC) -o utils/BuildImage utils/BuildImage.c

utils/Offsets:	utils/Offsets.c
	$(CC) $(INCLUDES) -o utils/Offsets utils/Offsets.c

#
# Clean out this directory
#

clean:
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage Offsets

#
# Create a printable namelist from the prog.o file
#

build/prog.nl: build/prog.o
	nm -Bng build/prog.o | pr -w80 -3 > build/prog.nl

#
# Generate a disassembly
#

build/prog.dis: build/prog.o
	dis build/prog.o > build/prog.dis

#
#       makedepend is a program which creates dependency lists by
#       looking at the #include lines in the source files
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

build/bootstrap.o: boot/bootstrap.h
build/startup.o: boot/bootstrap.h
build/isr_stubs.o: boot/bootstrap.h
build/ulibs.o: kernel/syscall.h include/include/headers.h arch/x86arch.h
build/c_io.o: lib/c_io.h boot/startup.h boot/support.h arch/x86arch.h
build/support.o: boot/startup.h boot/support.h lib/c_io.h arch/x86arch.h
build/support.o: boot/bootstrap.h
build/kalloc.o: include/headers.h kernel/kalloc.h arch/x86arch.h
build/clock.o: include/headers.h kernel/clock.h kernel/process.h kernel/queue.h kernel/stack.h kernel/scheduler.h lib/sio.h
build/clock.o: boot/startup.h arch/x86arch.h
build/klibc.o: include/headers.h
build/queue.o: include/headers.h kernel/queue.h kernel/process.h kernel/clock.h kernel/stack.h
build/process.o: include/headers.h kernel/process.h kernel/clock.h kernel/queue.h kernel/stack.h
build/scheduler.o: include/headers.h kernel/scheduler.h kernel/process.h kernel/clock.h kernel/queue.h kernel/stack.h
build/sio.o: include/headers.h lib/sio.h kernel/queue.h kernel/process.h kernel/clock.h kernel/stack.h kernel/scheduler.h
build/sio.o: boot/startup.h arch/uart.h arch/x86arch.h
build/stack.o: include/headers.h kernel/stack.h kernel/queue.h
build/syscall.o: include/headers.h kernel/syscall.h arch/x86arch.h kernel/process.h
build/syscall.o: kernel/clock.h kernel/queue.h kernel/stack.h kernel/scheduler.h lib/sio.h kernel/system.h include/types.h
build/syscall.o: boot/startup.h lib/ulib.h
build/system.o: include/headers.h kernel/system.h include/types.h kernel/process.h kernel/clock.h kernel/queue.h kernel/stack.h
build/system.o: boot/bootstrap.h kernel/syscall.h arch/x86arch.h lib/sio.h
build/system.o: kernel/scheduler.h test/user.h lib/ulib.h
build/ulibc.o: include/headers.h
build/user.o: include/headers.h test/user.h
