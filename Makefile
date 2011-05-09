.PHONY: build

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
U_C_SRC = kernel/clock.c kernel/klibc.c kernel/queue.c kernel/process.c kernel/scheduler.c lib/sio.c \
	  kernel/stack.c kernel/syscall.c kernel/system.c lib/ulibc.c mm/mm.c mm/paging.c mm/framefunc.c test/user.c
U_C_OBJ = build/clock.o build/klibc.o build/queue.o build/process.o build/scheduler.o build/sio.o \
	  build/stack.o build/syscall.o build/system.o build/ulibc.o build/mm.o build/paging.o build/framefunc.o build/user.o
U_S_SRC = kernel/klibs.S lib/ulibs.S
U_S_OBJ = build/klibs.o build/ulibs.o
U_LIBS	=


BUILD_DIR = build
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))

INCLUDES = -I. -Iinclude/

all:	build build/prog.o

build: 
	@echo "Building HostOS....."
	mkdir -p build
	cd boot &&  make && mv *.o ../build/ && mv *.b ../build/
	cd mm && make && mv *.o ../build/
	cd kernel && make && mv *.o ../build/
	cd lib && make && mv *.o ../build/
	cd utils && make
	cd test && make && mv *.o ../build/
	@echo "Building complete...."

BOOT_OBJ = build/bootstrap.b
BOOT_SRC = boot/bootstrap.S

S_OBJ = build/startup.o build/isr_stubs.o $(U_S_OBJ)
S_SRC = boot/startup.S kernel/isr_stubs.S $(U_S_SRC)

C_OBJ = build/c_io.o build/support.o $(U_C_OBJ)
C_SRC = lib/c_io.c boot/support.c $(U_C_SRC)

OBJECTS = $(S_OBJ) $(C_OBJ)
SOURCES = $(BOOT_SRC) $(S_SRC) $(C_SRC)

#usb.image: build/bootstrap.b build/prog.b build/prog.nl utils/BuildImage utils/Offsets #prog.dis 
usb.image: build/bootstrap.b build/prog.b utils/BuildImage utils/Offsets #prog.dis 
	utils/BuildImage -d usb -o build/usb.image -b build/bootstrap.b build/prog.b 0x10000

#floppy.image: build/bootstrap.b build/prog.b build/prog.nl utils/BuildImage utils/Offsets #prog.dis 
#floppy.image: build/bootstrap.b build/prog.b utils/BuildImage utils/Offsets #prog.dis 
floppy.image: build build/prog.b
	utils/BuildImage -d floppy -o build/floppy.image -b build/bootstrap.b build/prog.b 0x10000

build/prog.out: $(OBJECTS)
	$(LD) -o build/prog.out $(OBJECTS)

build/prog.o:	$(OBJECTS)
	$(LD) -o build/prog.o -Ttext 0x10000 $(OBJECTS) $(U_LIBS)

build/prog.b:	build/prog.o
	$(LD) -o build/prog.b -s --oformat binary -Ttext 0x10000 build/prog.o

build/prog.nl: build/prog.o
	nm -Bng build/prog.o | pr -w80 -3 > build/prog.nl

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


clean:
	@echo "Cleaning...."
	cd boot && make clean
	cd kernel && make clean
	cd lib && make clean
	cd mm && make clean
	cd build && rm -f *.o *.b *.nl *.image
	cd utils && rm -f BuildImage && rm -f Offsets
