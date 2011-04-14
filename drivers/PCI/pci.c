#include "pci.h"
#include "startup.h"
#include "c_io.h"
#include "kalloc.h"
#include "usb_ehci.h"
#include "ulib.h"

#define CONFIG_DATA 0xCFC
#define CONFIG_ADDRESS 0xCF8

#define USB_EHCI 0x0c032000


void _pci_init(void)
{
	//Need to memset root to 0
	root.subordinate = _pci_list( &root );	
}

uint32_t  _pci_list(struct _pci_bus * bus)
{
	uint8_t dev_num, function_num;
	uint32_t max = bus->secondary;

	struct _pci_dev * device;
	struct _pci_dev ** bus_last = &bus->devices;

	struct _pci_bus * child;

	for(dev_num = 0; dev_num < 0x20; dev_num++)
	{
		for(function_num = 0; function_num < 0x08; function_num++)
		{

			uint32_t vendor_device_id = _pci_config_read_word(bus->number , dev_num , function_num , 0x00);

			//c_printf("vendor_device_id %x\n" , vendor_device_id);

			if(vendor_device_id != -1)
			{
				//This is just debugging info, you can probably remove most of this

				c_puts("Found a function\n");

				

				c_printf("device_num: %d function: %d value:%x\n" , dev_num , function_num , vendor_device_id);

				uint32_t status = _pci_config_read_word(bus->number , dev_num , function_num , 0x04);

				c_printf("status %x\n" , status);

				uint32_t class_address = _pci_config_read_word(bus->number , dev_num , function_num , 0x08);

				uint8_t class_code = class_address >> 24 & 0xff;
				uint8_t subclass = class_address >> 16 & 0xff;
				uint8_t prog_if = class_address >> 8 & 0xff;

				c_printf("class address %x class code %x subclass %x prog_if %x\n" ,class_address ,  class_code , subclass , prog_if);
				

				//The real shit
				

				//We can check here if it is a PCI bus or not
				
				device = (struct _pci_dev *)_kalloc(sizeof(struct _pci_dev));

				//check if malloc fails

				//Set bus
				
				device->bus = bus;

				//Set vendor and device fields
				uint16_t vendor_id = (vendor_device_id >> 16) & 0xffff;
				uint16_t device_id = vendor_device_id & 0xffff;
				
				device->vendor = vendor_id;
				device->device = device_id;
				
				device->device_num = dev_num;
				device->function_num = function_num;

				uint32_t class = _pci_config_read_word(bus->number , dev_num , function_num , 0x08);
				device->class = class;
					
				//I don't know what device_function is supposed to be

				uint32_t header_type = _pci_config_read_word(bus->number , dev_num , function_num , 0x0c);

				device->header_type = header_type;
			
				//Now we read in all the base address registers

				device->base_address[0] = _pci_config_read_word(bus->number , dev_num , function_num , 0x10);
				
				device->base_address[1] = _pci_config_read_word(bus->number , dev_num , function_num , 0x14);
				
				device->base_address[2] = _pci_config_read_word(bus->number , dev_num , function_num , 0x18);
				
				device->base_address[3] = _pci_config_read_word(bus->number , dev_num , function_num , 0x1c);
				
				device->base_address[4] = _pci_config_read_word(bus->number , dev_num , function_num , 0x20);
				
				device->base_address[5] = _pci_config_read_word(bus->number , dev_num , function_num , 0x24);
				//I'm not really sure what the size array or rom address is for
				
				
				//Throwing this in to test USB2 code, remove this and include
				//of ehci when done
				
				if((class & 0xFFFFFF00) == USB_EHCI)
				{
					usb_ehci_init(device);
				}

				/*c_puts("Sleeping...\n");

				sleep( 1000 );
				*/
			}			
		}
	}
}

//Maybe these should be chars?
uint32_t _pci_config_read_word(uint8_t bus , uint8_t device, uint8_t function , uint8_t offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t ldevice = (uint32_t)device;
	uint32_t lfunc = (uint32_t)function;
	uint32_t loffset = (uint32_t)offset;
	uint32_t ret = 0;


	//TODO: Remove magic numbers
	address = (uint32_t)((lbus << 16) | (ldevice << 11) |
			(lfunc << 8) | (loffset & 0xfc) | ((uint32_t)0x80000000));
	/* write out the address */
	__outl (CONFIG_ADDRESS, address);
	/* read in the data */
	//ret = (uint16_t)((__inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);

	ret = (uint32_t)(__inl(CONFIG_DATA));
	return (ret);
}
