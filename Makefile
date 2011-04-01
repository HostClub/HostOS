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
U_C_SRC = kalloc.c clock.c klibc.c queue.c process.c scheduler.c sio.c \
	  stack.c syscall.c system.c ulibc.c user.c
U_C_OBJ = kalloc.o clock.o klibc.o queue.o process.o scheduler.o sio.o \
	  stack.o syscall.o system.o ulibc.o user.o
U_S_SRC = klibs.S ulibs.S
U_S_OBJ = klibs.o ulibs.o
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

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) -o $*.o $*.s -a=$*.lst
	$(RM) -f $*.s

.s.b:
	$(AS) -o $*.o $*.s -a=$*.lst
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

# Binary/source file for system bootstrap code

BOOT_OBJ = bootstrap.b
BOOT_SRC = bootstrap.S

# Assembly language object/source files

S_OBJ = startup.o isr_stubs.o $(U_S_OBJ)
S_SRC =	startup.S isr_stubs.S $(U_S_SRC)

# C object/source files

C_OBJ =	c_io.o support.o $(U_C_OBJ)
C_SRC =	c_io.c support.c $(U_C_SRC)

# Collections of files

OBJECTS = $(S_OBJ) $(C_OBJ)

SOURCES = $(BOOT_SRC) $(S_SRC) $(C_SRC)

#
# Targets for remaking bootable image of the program
#
# Default target:  usb.image
#

usb.image: bootstrap.b prog.b prog.nl BuildImage Offsets #prog.dis 
	./BuildImage -d usb -o usb.image -b bootstrap.b prog.b 0x10000

floppy.image: bootstrap.b prog.b prog.nl BuildImage Offsets #prog.dis 
	./BuildImage -d floppy -o floppy.image -b bootstrap.b prog.b 0x10000

prog.out: $(OBJECTS)
	$(LD) -o prog.out $(OBJECTS)

prog.o:	$(OBJECTS)
	$(LD) -o prog.o -Ttext 0x10000 $(OBJECTS) $(U_LIBS)

prog.b:	prog.o
	$(LD) -o prog.b -s --oformat binary -Ttext 0x10000 prog.o

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

BuildImage:	BuildImage.c
	$(CC) -o BuildImage BuildImage.c

Offsets:	Offsets.c
	$(CC) $(INCLUDES) -o Offsets Offsets.c

#
# Clean out this directory
#

clean:
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage Offsets

#
# Create a printable namelist from the prog.o file
#

prog.nl: prog.o
	nm -Bng prog.o | pr -w80 -3 > prog.nl

#
# Generate a disassembly
#

prog.dis: prog.o
	dis prog.o > prog.dis

#
#       makedepend is a program which creates dependency lists by
#       looking at the #include lines in the source files
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

bootstrap.o: bootstrap.h
startup.o: bootstrap.h
isr_stubs.o: bootstrap.h
ulibs.o: syscall.h headers.h x86arch.h
c_io.o: c_io.h startup.h support.h x86arch.h
support.o: startup.h support.h c_io.h x86arch.h
support.o: bootstrap.h
kalloc.o: headers.h kalloc.h x86arch.h
clock.o: headers.h clock.h process.h queue.h stack.h scheduler.h sio.h
clock.o: startup.h x86arch.h
klibc.o: headers.h
queue.o: headers.h queue.h process.h clock.h stack.h
process.o: headers.h process.h clock.h queue.h stack.h
scheduler.o: headers.h scheduler.h process.h clock.h queue.h stack.h
sio.o: headers.h sio.h queue.h process.h clock.h stack.h scheduler.h
sio.o: startup.h uart.h x86arch.h
stack.o: headers.h stack.h queue.h
syscall.o: headers.h syscall.h x86arch.h process.h
syscall.o: clock.h queue.h stack.h scheduler.h sio.h system.h types.h
syscall.o: startup.h ulib.h
system.o: headers.h system.h types.h process.h clock.h queue.h stack.h
system.o: bootstrap.h syscall.h x86arch.h sio.h
system.o: scheduler.h user.h ulib.h
ulibc.o: headers.h
user.o: headers.h user.h
