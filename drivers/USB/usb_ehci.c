#include "usb_ehci.h"
#include "c_io.h"
#include "ulib.h"
#include "support.h"
#include "x86arch.h"
#include "kalloc.h"

#include "usb_ehci_defs.h"

#define USB_EHCI_DEBUG


void usb_ehci_init(struct _pci_dev * device)
{

#ifdef USB_EHCI_DEBUG
	c_puts("In USB ECHI INIT\n");
#endif

	//I could check this to see if I can do 64 bit addressing, but whatever
	//Need to and to align to the 32 bit DWord boundary (yes DWord is 32 bit in
	//ehci land)
	_BASE = device->base_address[0] & BASE_MASK;

	//Base Registers
	_CAPLENGTH = (_BASE + CAPLENGTH_OFFSET);
	_HSCPARAMS = (_BASE + HSCPARAMS_OFFSET);
	_HCCPARAMS = (_BASE + HCCPARAMS_OFFSET);

	_OPERATIONALBASE = _BASE + *_CAPLENGTH;

	//Operation Registers
	_USBCMD = (_OPERATIONALBASE + USBCMD_OFFSET);
	_USBSTS = (_OPERATIONALBASE + USBSTS_OFFSET);
	_CONFIGFLAG = (_OPERATIONALBASE + CONFIGFLAG_OFFSET);

	_ASYNCLISTADDR = (_OPERATIONALBASE + ASYNCLISTADDR_OFFSET);
#ifdef USB_EHCI_DEBUG
	c_printf("USBBASE %x\n" , _BASE);
	c_printf("CAPLENGTH %x\n" , *_CAPLENGTH);
	c_printf("HSCPARAMS %x\n" , *_HSCPARAMS);	
	c_printf("HCCPARAMS %x\n" , *_HCCPARAMS);	
	c_printf("OPERATIONALBASE %x\n" , _OPERATIONALBASE);	
	c_printf("USBCMD: %x %x\n" , _USBCMD , *_USBCMD);
	c_printf("USBSTS %x %x\n" , _USBSTS ,  *_USBSTS);
	c_printf("CONFIGFLAG %x\n" , _CONFIGFLAG);
#endif

	_N_PORTS = ( *_HSCPARAMS & N_PORTS_MASK ) >> N_PORTS_OFFSET;
	
	//Lets try resetting everything before we try doing things
	/*
	c_puts("Stopping Host controller\n");

	*_USBCMD &= ~RUN_ENABLE;

	c_printf("USBCMD %x\n" , *_USBCMD);
	
	sleep(10);

	c_printf("USBSTS %x\n" , *_USBSTS);


	while( !(*_USBSTS & HCHALTED_ENABLE))
	{
		//c_printf("USBSTS %x\n" , *_USBSTS);
	}

	c_puts("Host controller halted!\n");

	*_USBCMD |= HCRESET_ENABLE;

	c_puts("Host controller reset!\n");

	sleep(20);


	*_USBCMD |= RUN_ENABLE;

	c_printf("USBCMD %x\n" , *_USBCMD);
	c_printf("USBSTS %x\n" , *_USBSTS);

	//Wait for the software to become unhalted
	//ie HCHALTED = 0
	while( *_USBSTS & HCHALTED_ENABLE)
	{

	}

	//Now we reset all ports

	int port;
	uint32_t * port_address;

	for(port = 0; port < _N_PORTS; port++)
	{
		port_address = _OPERATIONALBASE + PORTSC_OFFSET + (4 * port);

		uint32_t port_status = *port_address;


		c_printf("port number: %d port status: %x\n" , port , port_status);

		*port_address |= PORT_RESET_ENABLE;		
	}

	c_puts("Ports reset\n");
	
	*/

	//Initilization Stuff
	uint32_t eecp = (*_HCCPARAMS & EECP_MASK) >> EECP_OFFSET;

#ifdef USB_EHCI_DEBUG
	c_printf("EECP %x\n" , eecp);
#endif

	//Check to see if we've got the extended capabilities register
	if (eecp > 0)
	{
		_USBLEGSUP = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , eecp);
		_USBLEGCTLSTS = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , eecp + USBLEGCTLSTS_OFFSET);

#ifdef USB_EHCI_DEBUG
		c_printf("USBLEGSUP %x\n" , _USBLEGSUP);
		c_printf("USBLEGCTLSTS %x\n" , _USBLEGCTLSTS);
#endif

		_pci_config_write_word(device->bus->number , device->device_num , device->function_num , eecp + USBLEGCTLSTS_OFFSET , _USBLEGCTLSTS | SMI_OS_OWN_ENABLE);
		_pci_config_write_word(device->bus->number , device->device_num , device->function_num , eecp , _USBLEGSUP | HC_OS_OWNED_ENABLE);
		
		//Spin while BIOS claims they are owning EHCI
		//Should replace this with interrupts 
		while( _USBLEGSUP & HC_BIOS_OWNED_ENABLE )
		{
			_USBLEGSUP = _pci_config_read_word(device->bus->number , device->device_num , device->function_num , eecp);	
		}

#ifdef USB_EHCI_DEBUG
		c_printf("USBLEGSUP %x\n" , _USBLEGSUP);
#endif		
	}

	//Last real step in ownership process, now we just have to deal
	//with what happens after transferring control
	*_CONFIGFLAG |= CONFIGFLAG_ENABLE;


	if(!(*_USBCMD & RUN_ENABLE))
	{
	//It seems the host controller likes to shut off after transferring control
		*_USBCMD |= RUN_ENABLE;

#ifdef USB_EHCI_DEBUG
		c_puts("HOST CONTROLLER REENABLED\n");
#endif
	}

	int port;

	for(port = 0; port < _N_PORTS; port++)
	{
		uint32_t * port_address = _OPERATIONALBASE + PORTSC_OFFSET + (4 * port);
		uint32_t port_status = *port_address;

		//Lets check if the port is not in k_state
		//If the port has a connection and the Line status is a high speed device
		
		if(port_status & CONNECT_STATUS)
		{
			if(((port_status & LINE_STATUS_MASK) >> LINE_STATUS_OFFSET) != K_STATE)
			{
				//Port is a high speed device, wants a reset
				*port_address |= PORT_RESET_ENABLE;
				*port_address &= ~PORT_ENABLE;
				
				//Wait for the port to reset
				sleep(10);

				*port_address &= ~PORT_RESET_ENABLE;
			}
		
			//Should probably deal with it being a low speed device here
		}
	}

	//Interrupt Setup

	_USBINTR = _OPERATIONALBASE + USBINTR_OFFSET;

	*_USBINTR = USB_INT_ENABLE | PORT_CHANGE_INT_ENABLE | ASYNC_ADV_INT_ENABLE;
	
	__install_isr(USB_INT_VEC , _isr_usb_int);

	while(1)
	{
		sleep(1000);
#ifdef USB_EHCI_DEBUG
	c_printf("USBINTR %x\n" , *_USBINTR);
	c_printf("USBSTS %x\n" , *_USBSTS);
	c_printf("USBCMD %x\n" , *_USBCMD);
#endif	
	}

}

void _isr_usb_int(int vector , int code)
{
	
#ifdef USB_EHCI_DEBUG
	c_printf("Recieved USB Interrupt %d %d\n" , vector , code);
#endif

	//while(*_USBSTS & *_USBINTR)
	//{
		int port;
		uint32_t * port_address;

#ifdef USB_EHCI_DEBUG
		c_printf("USBINTR %x\n" , *_USBINTR);
		c_printf("USBSTS %x\n" , *_USBSTS);
#endif

		if(*_USBSTS & PORT_CHANGE_DETECT)
		{

			for( port = 0; port < _N_PORTS; port++)
			{
				port_address = _OPERATIONALBASE + PORTSC_OFFSET + (4 * port);

				uint32_t port_status = *port_address;

				if(port_status & CONNECT_STATUS_CHANGE_ENABLE)
				{
#ifdef USB_EHCI_DEBUG
					c_printf("Something changed on port %d %x\n" , port , *port_address);
#endif
					//*port_address |= CONNECT_STATUS_CHANGE_ENABLE;
				
					struct _qtd_head * head = _create_setup_qtd(port);

#ifdef USB_EHCI_DEBUG
					uint32_t * field_ptr = head;
					
					int i;
					for(i = 0; i < 4; i++)
					{
						c_printf("%x\n" , field_ptr[i]);
					}
#endif
					*_ASYNCLISTADDR = head;
#ifdef USB_EHCI_DEBUG
					c_puts("Enabling ASYNC Transfers\n");
#endif
					*_USBCMD |= ASYNC_ENABLE;
					*_USBCMD |= ASYNC_ADV_DOORBELL_ENABLE;
				}
			}

			*_USBSTS |= PORT_CHANGE_DETECT;
			
		}
		else if(*_USBSTS & USB_INT_ENABLE)
		{
			*_USBSTS |= USB_INT; 
		}
		else if(*_USBSTS & FRAME_LIST_ROLLOVER_ENABLE)
		{
			*_USBSTS |= FRAME_LIST_ROLLOVER_ENABLE;
		}
		else if(*_USBSTS & ASYNC_ADV_INT_ENABLE)
		{
			*_USBSTS |= INT_ASYNC_ADV_ENABLE; 
		}

#ifdef USB_EHCI_DEBUG
		c_printf("USBSTS %x\n" , *_USBSTS);
#endif

	//}

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
	__outb( PIC_SLAVE_CMD_PORT, PIC_EOI );

}

struct _qtd * _create_qtd(uint32_t next , uint8_t toggle , uint32_t bytes_to_transfer , uint8_t pid_code)
{
#ifdef USB_EHCI_DEBUG
	c_puts("Creating new qtd\n");
#endif

	struct _qtd * curr_qtd = _kalloc(sizeof(struct _qtd));

	memset(curr_qtd , 0 , sizeof(struct _qtd));

	if(next == -1)
	{
		curr_qtd->next_qtd |= NEXT_QTD_TERMINATE;
	}
	else
	{
		curr_qtd->next_qtd |= next << NEXT_QTD_OFFSET;
	}

	curr_qtd->alternate_qtd |= ALTERNATE_QTD_TERMINATE;

	curr_qtd->qtd_token |= toggle << DATA_TOGGLE_OFFSET;
	curr_qtd->qtd_token |= bytes_to_transfer << BYTES_TO_TRANSFER_OFFSET;
	curr_qtd->qtd_token |= INTERRUPT_ON_COMPLETE_ENABLE;
	curr_qtd->qtd_token |= pid_code << PID_CODE_OFFSET;
	curr_qtd->qtd_token |= STATUS_ACTIVE_ENABLE;

	_alloc_qtd_buffer(curr_qtd , bytes_to_transfer);

	return curr_qtd;
}

void _alloc_qtd_buffer(struct _qtd * curr_qtd , uint32_t buffer_size)
{
	struct _echi_request * buffer = _kalloc(buffer_size);

	//uint32_t offset = buffer % EHCI_PAGE_SIZE;

	//uint32_t

	c_printf("Current Buffer %x\n" , buffer);

	curr_qtd->buffer_list[0] = buffer;
}

struct _qtd_head * _create_setup_qtd(int device)
{
	

	struct _qtd * next = _create_qtd(-1 , 0 , 0 , OUT_TOKEN);

	next = _create_qtd(next , 1 , 12 , IN_TOKEN);

	struct _qtd * setup_qtd = _create_qtd(next , 0 , 8 , SETUP_TOKEN);

	struct _ehci_request * setup_request = setup_qtd->buffer_list[0];

	setup_request->type = 0x80;
	setup_request->request = 6;
	setup_request->value_high = 3;
	setup_request->length = 12;
#ifdef USB_EHCI_DEBUG
	c_puts("Current setup register\n");
	uint32_t * index_ptr = setup_qtd;
	int i;
	for(i = 0; i < 8; i ++)
	{
		c_printf("%d: %x\n" ,i ,  index_ptr[i]);
	}

	c_printf("%x\n" , setup_qtd->buffer_list[0]);
	c_printf("%x\n" , (uint32_t *)setup_qtd->buffer_list[0]);
#endif

	struct _qtd_head * head = _create_qtd_head(-1 , device , 0);

	//head->curr_qtd = setup_qtd;
	head->qtd.next_qtd = setup_qtd;

	return head;
}


struct _qtd_head * _create_qtd_head(uint32_t next , int device , int endpoint)
{
#ifdef USB_EHCI_DEBUG
	c_puts("Creating new qtd_head\n");
#endif
	struct _qtd_head * curr_qtd_head = _kalloc(sizeof(struct _qtd_head));
	
	memset(curr_qtd_head , 0 , sizeof(struct _qtd_head));

	if(next != -1)
	{
		curr_qtd_head->qhlp |= next << QHLP_OFFSET;
	}
	else
	{
		curr_qtd_head->qhlp |= QUEUE_HEAD_TERMINATE;
	}
	
	curr_qtd_head->qhlp |= QUEUE_HEAD_TYPE;
	

	curr_qtd_head->endpoint_char |= _MAX_PACKET_LENGTH << MAX_PACKET_LENGTH_OFFSET;
	curr_qtd_head->endpoint_char |= EPS_HIGH_SPEED;	
	curr_qtd_head->endpoint_char |= endpoint << ENDPOINT_OFFSET;
	curr_qtd_head->endpoint_char |= device << DEVICE_OFFSET;

	return curr_qtd_head;
}
