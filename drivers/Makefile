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

PCI_C_SRC = PCI/pci.c
PCI_C_OBJ = ../build/pci.o

USB_C_SRC = USB/usb_ehci.c 
USB_C_OBJ = ../build/usb_ehci.o

U_C_SRC = 
U_C_OBJ = 
U_LIBS	=


BUILD_DIR = ../build
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))

INCLUDES = -I. -I../include/

all:	build 
build: 
	@echo "Building HostOS drivers....."
	cd PCI && make && mv *.o ../../build/.
	cd USB && make && mv *.o ../../build/.

C_OBJ =  $(U_C_OBJ)
C_SRC =  $(U_C_SRC)

OBJECTS = $(C_OBJ)
SOURCES = $(C_SRC)


clean:
	@echo "Cleaning drivers...."
	
