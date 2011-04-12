#include "usb_ehci.h"
#include "c_io.h"

void usb_ehci_init(struct _pci_dev * device)
{
	c_puts("In USB ECHI INIT\n");
	
	//I could check this to see if I can do 64 bit addressing, but whatever
	//Need to and to align to the 32 bit DWord boundary (yes DWord is 32 bit in
	//ehci land)
	uint32_t usbbase = device->base_address[0] & 0xFFFFFF00;

	c_printf("usbbase %x\n" , usbbase);

	uint8_t caplength = *(uint8_t *)usbbase;

	c_printf("caplength %d\n" , caplength);

	uint32_t hscparams = * (uint32_t *)(usbbase + 0x04);

	c_printf("hscparams %x\n" , hscparams);

	uint32_t hccparams = * (uint32_t *)(usbbase + 0x08);

	c_printf("hccparams %x\n" , hccparams);

	int sbrn = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , 0x60);

	c_printf("sbrn %x\n" , sbrn);
}

