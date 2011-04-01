h05919
s 00001/00000/00185
d D 1.11 11/01/31 18:20:21 wrc 11 10
c added REPORT_MYSTERY_INTS to control int 0x27 messages
e
s 00005/00002/00180
d D 1.10 10/11/05 18:02:37 wrc 10 9
c updated for new USB jack locations
e
s 00015/00006/00167
d D 1.9 09/10/18 16:29:44 wrc 9 8
c updated for usb flash drive use
e
s 00022/00007/00151
d D 1.8 06/10/14 13:58:03 wrc 8 7
c configuration option changes
e
s 00001/00001/00157
d D 1.7 06/10/14 13:44:45 wrc 7 6
c sp1/sp2 compatability tweaks
e
s 00013/00004/00145
d D 1.6 06/10/14 12:43:30 wrc 6 5
c added Offsets and .S.o xform rules for sp1/sp2 compatability
e
s 00013/00011/00136
d D 1.5 05/03/15 15:46:57 wrc 5 4
c (wrc) minor name changes
e
s 00008/00009/00139
d D 1.4 03/09/30 11:27:01 kar 4 3
c Update for installation of latest OS & utilities
e
s 00028/00028/00120
d D 1.3 02/05/16 15:14:49 kar 3 2
c Version 2: improvements based on my experience with 20013 506.
e
s 00008/00007/00140
d D 1.2 02/01/22 16:50:04 kar 2 1
c New version for interrupt stuff
e
s 00147/00000/00000
d D 1.1 02/01/15 16:18:23 kar 1 0
c date and time created 02/01/15 16:18:23 by kar
e
u
U
f e 0
t
T
I 1
#
I 5
# SCCS ID: %W%	%G%
#
E 5
D 7
# Makefile to control the compiling, assembling and linking
E 7
I 7
# Makefile to control the compiling, assembling and linking of
E 7
D 8
# standalone programs in the DSL
E 8
I 8
# standalone programs in the DSL.  Used for both 4003-406 and
# 4003-506 (with appropriate tweaking).
E 8
#
D 5
# SCCS ID: %W%	%G%
#
E 5

#
# User supplied files
#
U_C_SRC = main.c
U_C_OBJ = main.o
U_S_SRC =
U_S_OBJ =
I 4
U_LIBS	=
E 4

#
I 8
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP2_CONFIG		enable SP2-specific startup variations
I 11
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
E 11
#
USER_OPTIONS = -DCLEAR_BSS_SEGMENT

#
E 8
I 5
D 6
# DO NOT CHANGE ANYTHING BELOW THIS POINT!!!
E 6
I 6
# YOU SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS POINT!!!
E 6
#
E 5
# Compilation/assembly control
#

D 2
INCLUDES = -I. -I/home/fac/kar/include -I/home/fac/wrc/include
E 2
I 2
D 3
INCLUDES = -I. -I/home/fac/kar/include
E 3
I 3
#
D 4
# We only want the include files in the current directory
E 4
I 4
D 5
# We only want to include from the current directory and ~kar/include
E 5
I 5
# We only want to include from the current directory and ~wrc/include
E 5
E 4
#
D 4
INCLUDES = -I.
E 4
I 4
D 5
INCLUDES = -I. -I/home/fac/kar/include
E 5
I 5
INCLUDES = -I. -I/home/fac/wrc/include
E 5
E 4
E 3
E 2

I 8
#
# Compilation/assembly/linking commands and options
#
CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc -I- $(INCLUDES)

E 8
CC = gcc
D 3
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes -nostdinc -I- $(INCLUDES)
E 3
I 3
D 4
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes -nostdinc -I- $(INCLUDES) -ggdb
E 4
I 4
D 8
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes -nostdinc -I- $(INCLUDES)
E 8
I 8
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)
E 8
E 4
E 3

D 3
AS = as
E 3
I 3
AS = gas
E 3
ASFLAGS =

D 8
CPP = cpp
D 2
CPPFLAGS = -traditional -nostdinc $(INCLUDES)
E 2
I 2
CPPFLAGS = -nostdinc $(INCLUDES)
E 8
I 8
LD = gld
E 8
E 2

#		
# Transformation rules - these ensure that all compilation
# flags that are necessary are specified
#
D 3
# Note use of 'cc' to assemble R4600 statements - this allows
# use of CPP #include/#define statements
E 3
I 3
D 5
# Note use of 'cc' to convert .S files to temporary .s files: this allows
# use of CPP #include/#define statements. However, the line numbers of
E 5
I 5
# Note use of 'cpp' to convert .S files to temporary .s files: this allows
D 8
# use of #include/#define statements. However, the line numbers of
E 8
I 8
# use of #include/#define/#ifdef statements. However, the line numbers of
E 8
E 5
# error messages reflect the .s file rather than the original .S file. 
# (If the .s file already exists before a .S file is assembled, then
# the temporary .s file is not deleted.  This is useful for figuring
# out the line numbers of error messages, but take care not to accidentally
# start fixing things by editing the .s file.)
E 3
#

.SUFFIXES:	.S .b

.c.s:
	$(CC) $(CFLAGS) -S $*.c

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

I 6
.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) -o $*.o $*.s -a=$*.lst

E 6
.s.b:
	$(AS) -o $*.o $*.s -a=$*.lst
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

D 3
.S.o:
	$(CC) $(CFLAGS) -c $*.S

E 3
D 5
# Binary/source file for system start routines
E 5
I 5
# Binary/source file for system bootstrap code
E 5

D 5
START_OBJ = bootstrap.b
START_SRC = bootstrap.S
E 5
I 5
BOOT_OBJ = bootstrap.b
BOOT_SRC = bootstrap.S
E 5

D 8
# Object/source files from assembly files in the system
E 8
I 8
# Assembly language object/source files
E 8

D 2
S_OBJ = startup.o $(U_S_OBJ)
S_SRC =	startup.S $(U_S_SRC)
E 2
I 2
S_OBJ = startup.o isr_stubs.o $(U_S_OBJ)
S_SRC =	startup.S isr_stubs.S $(U_S_SRC)
E 2

D 8
# Object/source files from C files in system
E 8
I 8
# C object/source files
E 8

D 2
C_OBJ =	c_io.o $(U_C_OBJ)
C_SRC =	c_io.c $(U_C_SRC)
E 2
I 2
D 3
C_OBJ =	c_io.o support.o $(U_C_OBJ)
C_SRC =	c_io.c support.c $(U_C_SRC)
E 3
I 3
D 4
C_OBJ =	c_io.o support.o gdb_stub.o $(U_C_OBJ)
C_SRC =	c_io.c support.c gdb_stub.c $(U_C_SRC)
E 4
I 4
C_OBJ =	c_io.o support.o $(U_C_OBJ)
C_SRC =	c_io.c support.c $(U_C_SRC)
E 4
E 3
E 2

# Collections of files

D 5
OBJECTS  = $(S_OBJ) $(C_OBJ)
E 5
I 5
OBJECTS = $(S_OBJ) $(C_OBJ)
E 5

D 3
HEADERS = c_io.h
E 3
I 3
D 4
HEADERS = c_io.h gdb_stub.h
E 3

E 4
D 5
SOURCES = $(START_SRC) $(S_SRC) $(C_SRC)
E 5
I 5
SOURCES = $(BOOT_SRC) $(S_SRC) $(C_SRC)
E 5

#
# Targets for remaking bootable image of the program
#
D 9
# This is the default target
E 9
I 9
# Default target:  usb.image
E 9
#

D 3
prog.img: bootstrap.b prog.b nl dis modify
	cat bootstrap.b prog.b > prog.img
	./modify prog.img prog.b
E 3
I 3
D 4
disk.image: bootstrap.b prog.b prog.nl prog.dis BuildImage
E 4
I 4
D 9
disk.image: bootstrap.b prog.b prog.nl BuildImage #prog.dis 
E 4
	./BuildImage -o disk.image bootstrap.b prog.b 0x10000
E 9
I 9
usb.image: bootstrap.b prog.b prog.nl BuildImage #prog.dis 
	./BuildImage -d usb -o usb.image -b bootstrap.b prog.b 0x10000
E 9
E 3

I 9
floppy.image: bootstrap.b prog.b prog.nl BuildImage #prog.dis 
	./BuildImage -d floppy -o floppy.image -b bootstrap.b prog.b 0x10000

E 9
I 3
prog.out: $(OBJECTS)
	$(LD) -o prog.out $(OBJECTS)

E 3
prog.o:	$(OBJECTS)
D 3
	$(LD) -o prog.o -Ttext 0x10800 $(OBJECTS)
E 3
I 3
D 4
	$(LD) -o prog.o -Ttext 0x10000 $(OBJECTS)
E 4
I 4
	$(LD) -o prog.o -Ttext 0x10000 $(OBJECTS) $(U_LIBS)
E 4
E 3

prog.b:	prog.o
D 3
	$(LD) -o prog.b -s --oformat binary -Ttext 0x10800 prog.o
E 3
I 3
	$(LD) -o prog.b -s --oformat binary -Ttext 0x10000 prog.o
E 3

#
D 9
# Target for copying bootable image onto a floppy
E 9
I 9
# Targets for copying bootable image onto boot devices
E 9
#

D 3
floppy:	prog.img
	dd if=prog.img of=/dev/fd0
E 3
I 3
D 9
floppy:	disk.image
	dd if=disk.image of=/dev/fd0
E 9
I 9
floppy:	floppy.image
	dd if=floppy.image of=/dev/fd0
E 9
E 3

I 9
usbu:	usb.image
D 10
	dd if=usb.image of=/local/usbu
E 10
I 10
	dd if=usb.image of=/local/devices/usbu
E 10

usbl:	usb.image
D 10
	dd if=usb.image of=/local/usbl
E 10
I 10
	dd if=usb.image of=/local/devices/usbl
E 10

I 10
usbr:	usb.image
	dd if=usb.image of=/local/devices/usbr

E 10
E 9
#
D 6
# Special rule for creating the modification program (don't want
# to use the same options as for the OS binaries)
E 6
I 6
# Special rule for creating the modification and offset programs
E 6
#
I 6
# These are required because we don't want to use the same options
# as for the standalone binaries.
#
E 6

D 3
modify:	modify.c
	$(CC) -o modify modify.c
E 3
I 3
BuildImage:	BuildImage.c
	$(CC) -o BuildImage BuildImage.c
E 3

I 6
Offsets:	Offsets.c
	$(CC) $(INCLUDES) -o Offsets Offsets.c

E 6
#
# Clean out this directory
#

clean:
D 3
	rm -f *.nl *.lst *.b *.o *.img *.dis modify 
E 3
I 3
D 6
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage 
E 6
I 6
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage Offsets
E 6
E 3

#
# Create a printable namelist from the prog.o file
#

D 3
nl:	prog.nl

E 3
prog.nl: prog.o
	nm -Bng prog.o | pr -w80 -3 > prog.nl

#
# Generate a disassembly
#

D 3
dis:	prog.dis

E 3
prog.dis: prog.o
	dis prog.o > prog.dis

#
#       makedepend is a program which creates dependency lists by
#       looking at the #include lines in the source files
#

depend:
	makedepend $(INCLUDES) $(SOURCES)

# DO NOT DELETE THIS LINE -- make depend depends on it.

D 2
c_io.o: c_io.h startup.h
E 2
I 2
D 3
c_io.o: c_io.h startup.h support.h /home/fac/kar/include/x86arch.h
support.o: startup.h support.h c_io.h /home/fac/kar/include/x86arch.h
E 2
main.o: c_io.h
E 3
E 1
