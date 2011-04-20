
#ifndef PCI_H
#define PCI_H

#include "types.h"

//
//STRUCTS
//


struct _pci_dev
{
	struct _pci_bus * bus;
	struct _pci_dev * sibling;	//Next device on bus
	struct _pci_dev * next;		//Next device in chain

	uint8_t device_num;
	uint8_t function_num;
	uint16_t vendor;
	uint16_t device;
	uint32_t class;

	uint32_t header_type;
	uint32_t io_queue;
	uint32_t base_address[6];
	uint32_t size[6];
	uint32_t rom_address;
};

struct _pci_bus
{
	struct _pci_bus * parent;
	struct _pci_bus * children;
	struct _pci_bus * next;
	struct _pci_dev * self;
	struct _pci_dev * devices;

	uint8_t number;
	uint8_t primary;
	uint8_t secondary;
	uint8_t subordinate;

	uint64_t mem_sz;
	uint64_t prefmem_sz;
	uint64_t io_sz;

	uint32_t membase;
	uint32_t memlimit;

	uint32_t prefmembase;
	uint32_t prefmemlimit;

	uint32_t iobase;
	uint32_t iolimit;
};

//
//GLOBAL VARIABLES
//

struct _pci_bus root;
struct _pci_dev pci_devices;

//
//METHOD HEADERS
//

void _pci_init(void);

uint32_t _pci_list(struct _pci_bus * bus);

void _find_pci(struct _pci_dev * search);

uint32_t _pci_config_read_word(uint8_t bus , uint8_t slot , uint8_t function , uint8_t offset);

void _pci_config_write_word(uint8_t bus , uint8_t slot , uint8_t function , uint8_t offset, uint32_t data );

//Valid values for byte offset are 0,8,16,24 - Will not do error checking
void _pci_config_write_byte(uint8_t bus , uint8_t slot , uint8_t function , uint8_t register_offset, uint8_t byte_offset , uint8_t data );


#endif
