CSOURCES := c_io.c ulibc.c mutex.c
SSOURCES := ulibs.S
ASMFILES := $(CSOURCES:.c=.s) $(SSOURCES:.S=.s)
OBJFILES := $(CSOURCES:.c=.o) $(SSOURCES:.S=.o)

BUILD_DIR = ../build

USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG
INCLUDES = -I. -I../include

CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc -iquote $(INCLUDES)

CC = gcc
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = as
ASFLAGS =

LD = ld

.PHONY: all clean
.INTERMEDIATE: $(ASMFILES)

all: $(OBJFILES)

%.s: %.c
	$(CC) $(CFLAGS) -S -o $@ $<

%.s: %.S
	$(CPP) $(CPPFLAGS) -o $@ $<

%.o: %.s
	$(AS) -o $@ $< -a=$*.lst

%.b: %.o
	$(LD) -Ttext 0x0 -s --oformat binary -e begtext -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJFILES) $(ASMFILES)
