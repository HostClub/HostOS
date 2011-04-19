#include "usb_ehci.h"
#include "c_io.h"
#include "ulib.h"
#include "support.h"

//PCI Stuff
#define BASE_MASK 0xFFFFFF00
uint32_t _BASE;

#define USBLEGSUP_OFFSET 0x00
uint32_t _USBLEGSUP;

#define HC_OS_OWNED_ENABLE (1<<24)

#define USBLEGCTLSTS_OFFSET 0x04
uint32_t _USBLEGCTLSTS;

//Memory Stuff
#define CAPLENGTH_OFFSET 0x00
uint8_t * _CAPLENGTH;

#define HCIVERSION_OFFSET 0x02
uint16_t * _HCIVERSION;

#define HSCPARAMS_OFFSET 0x04
uint32_t * _HSCPARAMS;

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
uint32_t * _HCCPARAMS;

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
uint32_t * _HCSPPORTROUTE;

uint32_t _OPERATIONALBASE;

#define USBCMD_OFFSET 0x00
uint32_t * _USBCMD;

#define HCRESET_ENABLE (1 << 1)

#define USBSTS_OFFSET 4
uint32_t * _USBSTS;

#define USB_INT (1 << 0)

#define PORT_CHANGE_DETECT (1 << 2)

#define USBINTR_OFFSET 8
uint32_t * _USBINTR;


//Interrupt enable flags

//Refer to table 2-11 of the EHCI spec

#define ASYNC_ADVANCE_ENABLE 		0x00000020
#define HOST_SYSTEM_ERROR_ENABLE 	0x00000010
#define FRAME_LIST_ROLLOVER_INT_ENABLE 	0x00000008
#define PORT_CHANGE_INT_ENABLE 		0x00000004
#define USB_ERROR_INT_ENABLE 		0x00000002

#define USB_INT_ENABLE 			0x00000001
#define USB_INT_VEC 0x2a

#define FRINDEX_OFFSET 0x0C
uint32_t * _FRINDEX;

#define CTRLDSSEGMENT_OFFSET 0x10
uint32_t * _CTRLDSSEGMENT;

#define PERIODICLISTBASE_OFFSET 0x14
uint32_t * _PERIODICLISTBASE;

#define ASYNCLISTADDR_OFFSET 0x18
uint32_t * _ASYNCLISTADDR;

#define CONFIGFLAG_OFFSET 0x40
uint32_t * _CONFIGFLAG;

//Need to add the port number * 4 - 1 to this
//To get the port status
#define PORTSC_OFFSET 0x44

//Trying a different way of defining constants
#define WAKE_OVERCURRENT_ENABLE (1 << 22)
#define WAKE_DISCONNET_ENABLE (1 << 21)
#define WAKE_CONNECT_ENABLE (1 << 20)

//Port Test control stuff
//EHCI Spec 4.14 USB2 Spec 7
#define PORT_TEST_CONTROL_OFFSET 16
#define PORT_TEST_CONTROL_MASK (0x0f << PORT_TEST_CONTROL_OFFSET)

#define TEST_NE 		0x00
#define TEST_JSTATE		0x01
#define TEST_KSTATE 		0x02
#define TEST_SEO_NAK 		0x03
#define TEST_PACKET 		0x04
#define TEST_FORCE_ENABLE 	0x05

//Port indicator defines
#define PORT_INDICATOR_CONTROL_OFFSET 14
#define PORT_INDICATOR_CONTROL_MASK (0x03 << PORT_INDICATOR_CONTROL_OFFSET)

#define PORT_INDICATOR_OFF 0x00
#define PORT_INDICATOR_AMBER 0x01
#define PORT_INDICATOR_GREEN 0x02
#define PORT_INDICATOR_UNDEFINED 0x03

#define PORT_OWNER_ENABLE (1 << 13)

//Depends on PPC in HSCPARAMS
#define PORT_POWER_ENABLE (1 << 12)

//Line Status Defines
#define LINE_STATUS_OFFSET 10
#define LINE_STATUS_MAST (0x03 << LINE_STATUS_OFFSET)

#define SEO_STATE 	0x00
#define J_STATE	 	0x01
//Low Speed Device
#define K_STATE 	0x02
#define UNDEFINED_STATE 0x03

#define PORT_RESET_ENABLE 		(1 << 8)
#define PORT_SUSPEND_ENABLE 		(1 << 7)
#define PORT_FORCE_RESUME_ENABLE 	(1 << 6)
#define OVERCURRENT_CHANGE_ENABLE 	(1 << 5)
#define OVERCURRENT_ACTIVE_ENABLE 	(1 << 4)
#define PORT_ENABLE_CHANGE_ENABLE 	(1 << 3)
#define PORT_ENABLE 			(1 << 2)
#define CONNECT_STATUS_CHANGE_ENABLE 	(1 << 1)
#define CONNECT_STATUS			(1 << 0)

//Non-Spec Globals

uint32_t _N_PORTS;


void usb_ehci_init(struct _pci_dev * device)
{
	c_puts("In USB ECHI INIT\n");
	
	//I could check this to see if I can do 64 bit addressing, but whatever
	//Need to and to align to the 32 bit DWord boundary (yes DWord is 32 bit in
	//ehci land)
	_BASE = device->base_address[0] & BASE_MASK;

	c_printf("usbbase %x\n" , _BASE);


	//Not sure if I should store the data or the address in the variables
	_CAPLENGTH = (_BASE + CAPLENGTH_OFFSET);
	c_printf("CAPLENGTH %x\n" , *_CAPLENGTH);

	_HSCPARAMS = (_BASE + HSCPARAMS_OFFSET);
	c_printf("HSCPARAMS %x\n" , *_HSCPARAMS);

	_HCCPARAMS = (_BASE + HCCPARAMS_OFFSET);
	c_printf("HCCPARAMS %x\n" , *_HCCPARAMS);

	_N_PORTS = ( *_HSCPARAMS & N_PORTS_MASK ) >> N_PORTS_OFFSET;

	_OPERATIONALBASE = _BASE + *_CAPLENGTH;

	c_printf("OPERATIONALBASE %x\n" , _OPERATIONALBASE);

	_USBCMD = (_OPERATIONALBASE + USBCMD_OFFSET);

	c_printf("USBCMD: %x %x\n" , _USBCMD , *_USBCMD);
		
	c_printf("%x %x\n" , USBSTS_OFFSET , _OPERATIONALBASE + USBSTS_OFFSET);

	_USBSTS = (_OPERATIONALBASE + USBSTS_OFFSET);

	c_printf("USBSTS %x %x\n" , _USBSTS ,  *_USBSTS);


	//Implementation Stuff

	uint32_t sbrn = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , 0x60);

	c_printf("sbrn %x\n" , sbrn);

	uint32_t eecp = (*_HCCPARAMS & EECP_MASK) >> EECP_OFFSET;

	c_printf("EECP %x\n" , eecp);

	if (eecp > 0)
	{
		_USBLEGSUP = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , eecp);

		_USBLEGCTLSTS = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , eecp + USBLEGCTLSTS_OFFSET);

		c_printf("USBLEGSUP %x\n" , _USBLEGSUP);
		c_printf("USBLEGCTLSTS %x\n" , _USBLEGCTLSTS);
		
		sleep(100);

		_pci_config_write_word(device->bus->number , device->device_num , device->function_num , eecp , _USBLEGSUP | HC_OS_OWNED_ENABLE);

		sleep(100);
		c_puts("In between write and read\n");
		sleep(100);

		_USBLEGSUP = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , eecp);

		sleep(100);

		c_printf("USBLEGSUP %x\n" , _USBLEGSUP);
		
	}


	//Interrupt Setup
		
	_USBINTR = _OPERATIONALBASE + USBINTR_OFFSET;

	*_USBINTR = USB_INT_ENABLE | PORT_CHANGE_INT_ENABLE;
	
	__install_isr(USB_INT_VEC , _isr_usb_int);
	
	c_printf("USBINTR %x\n" , *_USBINTR);



	//Implement the actual Init function from the ECHI spec section 4.1

	sleep(10000000000);
}

void _isr_usb_int(int vector , int code)
{
	c_printf("Recieved USB Interrupt! %d %d\n" , vector , code);

	int port;

	for(port = 0; port < _N_PORTS; port++)
	{
		uint32_t * port_address = _OPERATIONALBASE + PORTSC_OFFSET + (4 * port);

		uint32_t port_status = *port_address;

		
		c_printf("Port Number: %d Port Status: %x\n" , port , port_status);

		if(port_status & CONNECT_STATUS_CHANGE_ENABLE)
		{
			c_printf("SOMETHING CHANGED ON PORT %d\n" , port);
		}	
	}

	

	c_printf("USBINTR %x\n" , *_USBINTR);

	c_printf("USBSTS %x\n" , *_USBSTS);

	if(*_USBSTS & PORT_CHANGE_DETECT)
	{
		*_USBSTS &= ~PORT_CHANGE_DETECT;
	}
	else if(*_USBSTS & USB_INT_ENABLE)
	{
		*_USBSTS &= ~USB_INT; 
	}

	c_printf("USBSTS %x\n" , *_USBSTS);
}

