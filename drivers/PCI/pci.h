//
//STRUCTS
//


struct _pci_dev
{
	struct _pci_bus * bus;
	struct _pci_dev * sibling;	//Next device on bus
	struct _pci_dev * next;		//Next device in chain

	uint32_t device_function;
	uint16_t vendor;
	uint16_t device;
	uint32_t class;

	uint32_t header_type;
	uint32_t io_queue;
	uint64_t base_address[6];
	uint64_t size[6];
	uint64_t rom_address;
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
//CONSTANTS
//

int CONFIG_ADDRESS = 0xCF8;
int CONFIG_DATA = 0xCFC;

struct _pci_bus * list_pci();

struct _pci_dev * find_pci(struct _pci_dev * search);

uint16_t _pci_config_read_word(uint16_t bus , uint16_t slot , uint16_t function , uint16_t offset);
