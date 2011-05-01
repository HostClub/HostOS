BSOURCES := bootstrap.S trampoline.S
SOURCES := $(BSOURCES) startup.S
ASMFILES := $(SOURCES:.S=.s)
OBJFILES := $(SOURCES:.S=.o)
BFILES := $(BSOURCES:.S=.b)

BUILD_DIR = ../build

USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG
INCLUDES = -I. -I../include

CPP = cpp
CPPFLAGS = $(USER_OPTIONS) -nostdinc -iquote $(INCLUDES)

AS = as
ASFLAGS =

.PHONY: all clean
.INTERMEDIATE: $(ASMFILES)

all: $(OBJFILES) $(BFILES)

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
	$(RM) $(OBJFILES) $(BFILES) $(ASMFILES)
