#include "pci.h"
#include "startup.h"
#include "c_io.h"

#define CONFIG_DATA 0xCFC
#define CONFIG_ADDRESS 0xCF8

#define USB2_HOST_CONTROLLER 0x000c0320


void _pci_init(void)
{
	//Need to memset root to 0
	root.subordinate = _pci_list( &root );	
}

uint32_t  _pci_list(struct _pci_bus * bus)
{
	uint8_t device_num, function;
	uint32_t max = bus->secondary;

	struct _pci_dev * device;
	struct _pci_dev ** bus_last = &bus->devices;

	struct _pci_bus * child;

	for(device_num = 0; device_num < 0x0f; device_num++)
	{
		for(function = 0; function < 0x04; function++)
		{

			uint32_t vendor_id = _pci_config_read_word(bus->number , device_num , function , 0);


			if(vendor_id != -1)
			{
				c_puts("Found a function\n");



				c_printf("device_num: %d function: %d value:%x\n" , device_num , function , vendor_id);

				uint32_t status = _pci_config_read_word(bus->number , device_num , function , 0x04);

				c_printf("status %x\n" , status);

				uint32_t class_address = _pci_config_read_word(bus->number , device_num , function , 0x08);

				uint8_t class_code = class_address >> 24 & 0xff;
				uint8_t subclass = class_address >> 16 & 0xff;
				uint8_t prog_if = class_address >> 8 & 0xff;

				c_printf("class address %x class code %x subclass %x prog_if %x\n" ,class_address ,  class_code , subclass , prog_if);

				uint32_t device_function = class_address >> 8;
				
				if(device_function == USB2_HOST_CONTROLLER)
				{
					c_puts("Found a host controller!\n");
				}
	
				uint32_t bist_address = _pci_config_read_word(bus->number , device_num , function , 0x0c);

				c_printf("bist adress %x\n" , bist_address);

			}
			/*if((vendor_id = _pci_config_read_word(bus->number , device_num , function , 0)) != 0xfff)
			  {
			  c_puts("Found good funtion!\n");
			  c_printf("%d\n" , vendor_id);				
			  }*/
		}
	}
}

//Maybe these should be chars?
uint32_t _pci_config_read_word(uint8_t bus , uint8_t device, uint8_t function , uint8_t offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t lslot = (uint32_t)device;
	uint32_t lfunc = (uint32_t)function;
	uint32_t loffset = (uint32_t)offset;
	uint32_t ret = 0;


	//TODO: Remove magic numbers
	address = (uint64_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (loffset & 0xfc) | ((uint32_t)0x80000000));
	/* write out the address */
	__outl (CONFIG_ADDRESS, address);
	/* read in the data */
	//ret = (uint16_t)((__inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);

	ret = (uint32_t)(__inl(CONFIG_DATA));
	return (ret);
}
