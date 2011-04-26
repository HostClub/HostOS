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
	pci_devices = NULL;
	root.subordinate = _pci_list( &root );	
}

uint32_t  _pci_list(struct _pci_bus * bus)
{
	uint8_t dev_num, function_num;
	uint32_t max = bus->secondary;

	struct _pci_dev * device;
	struct _pci_dev ** bus_last = &bus->devices;
	struct pci_dev * temp;

	struct _pci_bus * child;

	//c_printf( "Current Bus #%d\n", bus->number );

	for(dev_num = 0; dev_num < 0x20; dev_num++)
	{
		for(function_num = 0; function_num < 0x08; function_num++)
		{

			uint32_t vendor_device_id = _pci_config_read_word(bus->number , dev_num , function_num , 0x00);

			//c_printf("vendor_device_id %x\n" , vendor_device_id);

			if(vendor_device_id != -1)
			{
				//This is just debugging info, you can probably remove most of this

			//	c_puts("Found a function\n");

				
				//c_printf("bus_num: %d device_num: %d function: %d value:%x\n" , bus->number, dev_num , function_num , vendor_device_id);
				uint32_t status = _pci_config_read_word(bus->number , dev_num , function_num , 0x04);

				//c_printf("status %x\n" , status);

				uint32_t class_address = _pci_config_read_word(bus->number , dev_num , function_num , 0x08);

				uint8_t class_code = class_address >> 24 & 0xff;
				uint8_t subclass = class_address >> 16 & 0xff;
				uint8_t prog_if = class_address >> 8 & 0xff;

//				c_printf("class address %x class code %x subclass %x prog_if %x\n" ,class_address ,  class_code , subclass , prog_if);

				uint32_t register_0c = _pci_config_read_word(bus->number , dev_num , function_num , 0x0c);

				uint8_t bist = register_0c >> 24 & 0xff;
				uint8_t hdr_type = register_0c >> 16 & 0xff;
				uint8_t latency_timer = register_0c >> 8 & 0xff;

			

				//The real shit
				

			
				device = (struct _pci_dev *)_kalloc(sizeof(struct _pci_dev));

				//check if malloc fails
				if( device == NULL ){
					c_puts("FUCK EVERY SINGLE THING\n");
				}	

				//Set bus
				
				device->next = NULL;
				device->bus = bus;

				//Add to list of devices
				if( pci_devices == NULL ){
					c_puts( "Setting the first device\n" );
					pci_devices = device;
				}
				else
				{
					struct _pci_dev * temp = pci_devices;
					
					//find last device in chain
					while( temp->next != NULL )
					{	
						temp = temp->next;
					}

					temp->next = device;

				}

				//We can check here if it is a PCI bus or not
				if( hdr_type == 0x01 ){


					//c_printf( "Found a new PCI-to-PCI bus!\n" );

					//Handle PCItoPCI bus
					child = (struct _pci_bus *)_kalloc(sizeof(struct _pci_bus));

					//Handle kalloc failing
					if( child == NULL ){}

					child->next = bus->children;
					bus->children = child;
					child->self = device;
					child->parent = bus;

					child->number = child->secondary = ++max;
					child->primary = bus->secondary;
					child->subordinate = 0xff;

					//Recursively call _pci_list
					max = _pci_list( child );		
					child->subordinate = max;
				}
	
				//Set vendor and device fields
				uint16_t vendor_id = (vendor_device_id >> 16) & 0xffff;
				uint16_t device_id = vendor_device_id & 0xffff;
				
				device->vendor = vendor_id;
				device->device = device_id;

				device->device_num = dev_num;
				device->function_num = function_num;
				
//				device->device_num = dev_num;
//				device->function_num = function_num;

				uint32_t class = _pci_config_read_word(bus->number , dev_num , function_num , 0x08);
				device->class = class;

//				c_printf( "Class code: %x\n", class );
					
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
				//The size array contains the size in memory of the spaces pointed at by the base address registers
			
			
				//Throwing this in to test USB2 code, remove this and include
				//of ehci when done
				

				/*c_puts("Sleeping...\n");

				sleep( 1000 );l
				*/
			}			
		}
	}
}

//
// finds the first _pci_dev that matches the class code in the passed struct.
// starts search at beginning if next is null or at the _pci_dev pointed to in next
//
struct _pci_dev * _find_pci(struct _pci_dev * search)
{

	struct _pci_dev * temp = pci_devices;

	//if next is provided, start search there
	if( search->next != NULL )
	{
		temp = search->next;
	}

	while( temp != NULL )
	{
		//check if current classcode matches current classcode
		if( temp->class == search->class )
		{
			//found it
			return temp;
		}

		temp = temp->next;

	}
    
	return NULL;

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

void _pci_config_write_word(uint8_t bus , uint8_t device, uint8_t function , uint8_t offset, uint32_t data)
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
	/* write out the data */

	__outl(CONFIG_DATA, data);
}

void _pci_config_write_byte(uint8_t bus , uint8_t device, uint8_t function , uint8_t register_offset, uint8_t offset, uint8_t data)
{
	uint32_t address;
	uint32_t lbus = (uint32_t)bus;
	uint32_t ldevice = (uint32_t)device;
	uint32_t lfunc = (uint32_t)function;
	uint32_t loffset = (uint32_t)register_offset;
	uint32_t ret = 0;


	//TODO: Remove magic numbers
	address = (uint32_t)((lbus << 16) | (ldevice << 11) |
			(lfunc << 8) | (loffset & 0xfc) | ((uint32_t)0x80000000));
	/* write out the address */
	__outl (CONFIG_ADDRESS, address);
	/* write out the data */

	c_printf("%x %x\n" , CONFIG_DATA , CONFIG_DATA + offset);

	__outb(CONFIG_DATA + offset, data);
}
