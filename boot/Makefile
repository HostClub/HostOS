BUILD_DIR = ../build

USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG
INCLUDES = -I. -I../include

CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc -I- $(INCLUDES)

AS = as
ASFLAGS =

.SUFFIXES:	.S .b

all: bootstrap.o startup.o bootstrap.b

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

clean:
	rm -f *.nl *.lst *.b *.o *.image *.dis BuildImage Offsets
