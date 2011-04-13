#include "usb_ehci.h"
#include "c_io.h"
#include "ulib.h"

#define BASE_MASK 0xFFFFFF00
uint32_t _BASE;

#define CAPLENGTH_OFFSET 0x00
uint8_t _CAPLENGTH;

#define HCIVERSION_OFFSET 0x02
uint16_t _HCIVERSION;

#define HSCPARAMS_OFFSET 0x04
uint32_t _HSCPARAMS;

#define DEBUG_PORT_NUMBER_MASK 0x00F00000
#define DEBUG_PORT_NUMBER_OFFSET  20

#define P_INDICATOR_MASK 0x00010000
#define P_INDICATOR_OFFSET 16

#define N_CC_MASK 0x0000F000
#define N_CC_OFFSET 12

#define N_PCC_MASK 0x00000F00
#define N_PCC_OFFSET 8

#define PORT_BINDING_RULES_MASK 0x00000040
#define PORT_BINDING_OFFSET 7

#define PPC_MASK 0x00000010
#define PPC_OFFSET 4

#define N_PORTS_MASK 0x0000000f
#define N_PORTS_OFFSET 0

#define HCCPARAMS_OFFSET 0x08
uint32_t _HCCPARAMS;

#define EECP_MASK 0x0000FF00
#define EECP_OFFSET 8

#define IST_MASK 0x000000F0 
#define IST_OFFSET 4

#define ASPC_MASK 0x00000004
#define ASPC_OFFSET 2

#define PFLF_MASK 0x00000002
#define PFLF_OFFSET 1

//64 Bit flag
#define SF_ADDRESS_MASK 0x00000001
#define SF_ADDRESS_OFFSET 0

#define HCSPPORTROUTE_OFFSET 0x0c
uint32_t _HCSPPORTROUTE;

uint32_t _OPERATIONALBASE;

#define USBCMD_OFFSET 0x00
uint32_t _USBCMD;

#define USBSTS_OFFSET 0x04
uint32_t _USBSTS;

#define USBINTR_OFFSET 0x08
uint32_t _USBINTR;

#define FRINDEX_OFFSET 0x0C
uint32_t _FRINDEX;

#define CTRLDSSEGMENT_OFFSET 0x10
uint32_t _CTRLDSSEGMENT;

#define PERIODICLISTBASE_OFFSET 0x14
uint32_t _PERIODICLISTBASE;

#define ASYNCLISTADDR_OFFSET 0x18
uint32_t _ASYNCLISTADDR;

#define CONFIGFLAG_OFFSET 0x40
uint32_t _CONFIGFLAG;

//Need to add the port number * 4 - 1 to this
//To get the port status
#define PORTSC_OFFSET 0x44



void usb_ehci_init(struct _pci_dev * device)
{
	c_puts("In USB ECHI INIT\n");
	
	//I could check this to see if I can do 64 bit addressing, but whatever
	//Need to and to align to the 32 bit DWord boundary (yes DWord is 32 bit in
	//ehci land)
	_BASE = device->base_address[0] & BASE_MASK;

	c_printf("usbbase %x\n" , _BASE);

	_CAPLENGTH = *(uint8_t *)(_BASE + CAPLENGTH_OFFSET);

	_HSCPARAMS = *(uint32_t *)(_BASE + HSCPARAMS_OFFSET);

	c_printf("HSCPARAMS %x\n" , _HSCPARAMS);

	uint32_t n_ports = ( _HSCPARAMS & N_PORTS_MASK ) >> N_PORTS_OFFSET;

	_OPERATIONALBASE = _BASE + _CAPLENGTH;

	_USBCMD = *(uint32_t *)(_OPERATIONALBASE + USBCMD_OFFSET);

	c_printf("USBCMD: %x\n" , _USBCMD);


	int port;

	for(port = 0; port < n_ports; port++)
	{
		uint32_t port_address = _OPERATIONALBASE + PORTSC_OFFSET + 4 * port;

		uint32_t port_status = * (uint32_t *)(port_address);

		c_printf("Port Number: %d Port Status: %x\n" , port , port_status);
	}

	sleep(10000);
}

