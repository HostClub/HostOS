BUILD_DIR = ../build

USER_OPTIONS = -DCLEAR_BSS_SEGMENT -DISR_DEBUGGING_CODE -DSP2_CONFIG
INCLUDES += -I. -I../include

CPPFLAGS = $(USER_OPTIONS) -I- $(INCLUDES)

CC = gcc
CFLAGS = -fno-builtin -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = as
ASFLAGS =

all: BuildImage Offsets

BuildImage: BuildImage.c
	$(CC) -o $@ $^

Offsets: Offsets.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f BuildImage Offsets
