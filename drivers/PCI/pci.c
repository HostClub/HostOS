#include "pci.h"

void _pci_init(void)
{
	//Need to memset root to 0
	pci_root.subordinate = _pci_list( &root );	
}

uint32_t  _pci_list(struct _pci_bus * bus)
{
	uint8_t device, function;
	uint32_t max = bus->secondary;
	
	struct _pci_dev * device;
	struct _pci_dev ** bus_last = &bus->devices;

	struct _pci_bus * child;

	for(device = 0; device < 0x0f; device_function++)
	{
		for(function = 0; function < 0x04; function++)
		{

			uint16_t vendor_id;
			if((vendor_id = _pci_config_read_word(bus->number , device , function , 0)) != 0xfff)
			{
				_cprintf("%s\n" , vendor_id);				
			}
		}
	}
}

//Maybe these should be chars?
uint16_t _pci_config_read_word(uint8_t bus , uint8_t device, uint8_t function , uint8_t offset)
{
	uint32_t address;
	uint32_t lbus = (uint64_t)bus;
	uint32_t lslot = (uint64_t)device;
	uint32_t lfunc = (uint64_t)function;
	uint16_t ret = 0;


	//TODO: Remove magic numbers
	address = (uint64_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (offset & 0xfc) | ((UINT32)0x80000000));

	/* write out the address */
	__outl (CONFIG_ADDRESS, address);
	/* read in the data */
	ret = (uint16_t)((__inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
	return (ret);
}
