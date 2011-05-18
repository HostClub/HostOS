#include "usb_ehci.h"
#include "c_io.h"
#include "ulib.h"
#include "support.h"
#include "x86arch.h"
#include "kalloc.h"
#include "cakesh.h"

#include "usb_ehci_defs.h"

#define USB_EHCI_DEBUG

#define DEFAULT_NUM_PORTS 8

struct _pci_dev * _ehci;

uint32_t port_statuses[DEFAULT_NUM_PORTS];

struct _qtd_head * queue_head;
uint8_t * data_buffer;


struct _qtd_head * _create_descriptor_qtd(int device , int num);
struct _qtd_head * _create_init_qtd(int device , int new_device);

void usb_ehci_init(struct _pci_dev * device)
{
	_ehci = device;

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
	_CTRLDSSEGMENT = (_OPERATIONALBASE + CTRLDSSEGMENT_OFFSET);
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

		//Write the appropriate bytes to transfer control
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
				c_printf("Resetting port %d\n" , port);
				//Port is a high speed device, wants a reset
				*port_address |= PORT_POWER_ENABLE;


				*port_address &= ~PORT_ENABLE;

				*port_address |= PORT_RESET_ENABLE;

				//Wait for the port to reset
				sleep(250);

				*port_address &= ~PORT_RESET_ENABLE;

				sleep(10);
				c_printf("Current Port Status %x\n" , *port_address);


			}

			//Should probably deal with it being a low speed device here
		}
	}

	//Lets set up the CTRLDSSEGMENT before we access any data
	*_CTRLDSSEGMENT = (uint32_t)0;

	//Interrupt Setup

	_USBINTR = _OPERATIONALBASE + USBINTR_OFFSET;

	//Set up the ISR first so we don't recieve unexpected interrupts
	__install_isr(USB_INT_VEC , _isr_usb_int);

	uint32_t usbintr = 0;

	//Enable all the interrupts
	usbintr |= USB_INT_ENABLE; 
	usbintr |= PORT_CHANGE_INT_ENABLE;
	usbintr |= ASYNC_ADV_INT_ENABLE;
	usbintr |= USB_ERR_INT_ENABLE;
	usbintr |= HOST_ERR_INT_ENABLE;

	*_USBINTR = usbintr;

	c_printf("USBINTR %x\n" , *_USBINTR);

	while(1)
	{

		sleep(1000);
#ifdef USB_EHCI_DEBUG
		c_printf("USBSTS %x\n" , *_USBSTS);	
		c_printf("USBCMD %x\n" , *_USBCMD);

		uint32_t error  = _pci_config_read_word(_ehci->bus->number , _ehci->device_num , _ehci->function_num , 0x04);

		c_printf("PCI Status %x\n" , error);

		c_printf("CTRLDSSEGMENT %x\n" , *_CTRLDSSEGMENT);
#endif		

		int port_index;

		for(port_index = 0; port_index < _N_PORTS; port_index++)
		{
			int port_status = port_statuses[port_index];

			uint32_t * port_address = _OPERATIONALBASE + PORTSC_OFFSET + (4 * port_index);
			if(port_status & CONNECT_STATUS_CHANGE_ENABLE && ((port_status & LINE_STATUS_MASK) >> LINE_STATUS_OFFSET) != K_STATE)
			{
				c_printf("Resetting port %d\n" , port_index);
				//Port is a high speed device, wants a reset
				*port_address |= PORT_POWER_ENABLE;


				*port_address &= ~PORT_ENABLE;

				*port_address |= PORT_RESET_ENABLE;

				//Wait for the port to reset
				sleep(250);

				*port_address &= ~PORT_RESET_ENABLE;

				sleep(10);
				c_printf("Current Port Status %x\n" , *port_address);


				sleep(1000);

				//Set up the interrogation queues
				struct _qtd_head * head = _create_init_qtd(port_index , port_index);


				c_printf("Current QTD Head %x\n" , head);

				print_qtd_head(head);

				struct _qtd_head * setup  = _create_descriptor_qtd(port_index , 1);

				head->qhlp &= 0x1F;
				head->qhlp |= (uint32_t)setup;

				setup->qhlp &= 0x1F;
				setup->qhlp |= (uint32_t)head;

				c_printf("Current QTD Head %x\n" , setup);
				
				print_qtd_head(setup);
				
				*_ASYNCLISTADDR = head;

				*_USBCMD |= ASYNC_ENABLE;
				sleep(20);
				*_USBCMD |= ASYNC_ADV_DOORBELL_ENABLE;
			
				
				sleep(100);

				print_qtd_buffer(data_buffer , 18);
				__panic("EXITING USB");

			}
		}

	}

	/*load_process("get_port_statuses" , get_port_statuses);

	load_process("usb_reset_ports" , reset_ports);
	*/
}

void _reset_port(int port)
{
	uint32_t * port_address = _OPERATIONALBASE + PORTSC_OFFSET + (4 * port);

#ifdef USB_EHCI_DEBUG
	c_printf("Resetting port %d\n" , port);
#endif
	
	*port_address |= PORT_POWER_ENABLE;
	*port_address &= ~PORT_ENABLE;
	
	*port_address |= PORT_RESET_ENABLE;

	//Wait for the port to reset
	sleep(250);

	*port_address &= ~PORT_RESET_ENABLE;

#ifdef USB_EHCI_DEBUG
	sleep(10);
	c_printf("Current Port Status %x\n" , *port_address);
#endif

}

void _isr_usb_int(int vector , int code)
{

#ifdef USB_EHCI_DEBUG
	c_printf("Recieved USB Interrupt %d %d\n" , vector , code);
#endif

	int port;
	uint32_t * port_address;

#ifdef USB_EHCI_DEBUG
	c_printf("USBINTR %x\n" , *_USBINTR);
	c_printf("USBSTS %x\n" , *_USBSTS);
	c_printf("USBCMD %x\n" , *_USBCMD);
#endif

	//If we detect a port change, chage the statuses
	if(*_USBSTS & PORT_CHANGE_DETECT)
	{

		for( port = 0; port < _N_PORTS; port++)
		{
			port_address = _OPERATIONALBASE + PORTSC_OFFSET + (4 * port);

			uint32_t port_status = *port_address;

			port_statuses[port] = port_status;

			if(port_status & CONNECT_STATUS_CHANGE_ENABLE)
			{
#ifdef USB_EHCI_DEBUG
				c_printf("Something changed on port %d %x\n" , port , *port_address);
#endif
			}

		}

		*_USBSTS &= PORT_CHANGE_DETECT;

	}


	if(*_USBSTS & USB_INT_ENABLE)
	{
		*_USBSTS &= USB_INT_ENABLE; 
	}

	if(*_USBSTS & USB_ERR_INT_ENABLE)
	{
		c_puts("RECIEVED USB ERROR\n");
		*_USBSTS &= USB_ERR_INT_ENABLE;
	}

	if(*_USBSTS & HOST_ERR_INT_ENABLE)
	{
		//This is a bad place to be
		c_puts("RECIEVED HOST ERROR\n");
		*_USBSTS &= HOST_ERR_INT_ENABLE;
		uint32_t error  = _pci_config_read_word(_ehci->bus->number , _ehci->device_num , _ehci->function_num , 0x04);

		c_printf("Current Error: %08x\n" , error);

	}

	if(*_USBSTS & FRAME_LIST_ROLLOVER_ENABLE)
	{
		*_USBSTS &= FRAME_LIST_ROLLOVER_ENABLE;
	}

	if(*_USBSTS & ASYNC_ADV_INT_ENABLE)
	{
		*_USBSTS &= ASYNC_ADV_INT_ENABLE; 

#ifdef USB_EHCI_DEBUG
		//Print the current ASYNC head for debugging purposes
		struct _qtd_head * head = *_ASYNCLISTADDR;
		print_qtd_head(head);
#endif
	}

#ifdef USB_EHCI_DEBUG

	c_printf("USBCMD %x\n" , *_USBCMD);
	c_printf("USBSTS %x\n" , *_USBSTS);
#endif

	//We've serviced all interrupts at this point

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
	__outb( PIC_SLAVE_CMD_PORT, PIC_EOI );

}

uint32_t _get_port_status(int port)
{
	return port_statuses[port];
}

uint32_t * _get_port_statuses( void )
{
	return port_statuses;
}

void _add_async_schedule(struct _qtd_head * head)
{
	*_USBCMD |= ASYNC_ENABLE;

	*_ASYNCLISTADDR = head;
}

uint32_t _perform_async_schedule()
{
	*_USBCMD |= ASYNC_ENABLE;
	
	sleep(20);

	*_USBCMD |= ASYNC_ADV_DOORBELL_ENABLE;

	//Wait until The async schedule is done
	while(!(*_USBSTS & ASYNC_ADV_INT_ENABLE))
	{
		sleep(10);
	}

	*_USBCMD &= ~ASYNC_ENABLE;
}

void print_qtd_token(struct _qtd_token * token)
{
	c_puts("QTD Token: ");
	print_qtd_buffer(token , 4);
}

void print_qtd(struct _qtd * qtd)
{
	c_printf("Curr qtd %x\n" , qtd);
	c_printf("Next QTD: %x\n" , qtd->next);
	c_printf("Alternate QTD: %x\n" , qtd->alternate);
	print_qtd_token(&qtd->token);
	c_printf("Current buffer: %x\n" , qtd->data_buffer[0]);

	print_qtd_buffer(qtd->data_buffer[0] , 8);

}

void print_qtd_head(struct _qtd_head * qtd_head)
{
	
	//Rather than deal with a bitfield I just print 
	//all the bytes
	print_qtd_buffer(qtd_head , 68);

	c_printf("Buffer Contents\n");

	print_qtd_buffer(qtd_head->qtd.data_buffer[0] , 8);

}

void print_qtd_buffer(uint8_t * qtd_buffer , int buffer_size)
{
	//Just print the bytes, 4 at a time, from the buffer
	int buffer_pointer;
	for(buffer_pointer = 0; buffer_pointer < buffer_size; buffer_pointer++)
	{

		c_printf("%02x" , qtd_buffer[buffer_pointer]);

		if((buffer_pointer + 1) % 4 == 0)
		{
			c_puts(" ");
		}
	}

	c_printf("\n");
}

struct _qtd * _create_qtd(uint32_t next , uint8_t toggle , uint32_t bytes_to_transfer , uint8_t pid_code)
{
#ifdef USB_EHCI_DEBUG
	c_puts("Creating new qtd\n");
#endif

	struct _qtd * curr_qtd = _kalloc_align(sizeof(struct _qtd) , 32);

#ifdef USB_EHCI_DEBUG
	c_printf("Current qtd %x\n" , curr_qtd);
#endif

	memset(curr_qtd , 0 , sizeof(struct _qtd));

	if(next == -1)
	{

		curr_qtd->next = QTD_TERMINATE;
	}
	else
	{
		//Must be 32 byte aligned
		curr_qtd->next = next;
	}

	curr_qtd->alternate = QTD_TERMINATE;

	//Set all the standard bytes for a QTD
	curr_qtd->token.data_toggle = toggle;
	curr_qtd->token.bytes_to_transfer = bytes_to_transfer;
	curr_qtd->token.interrupt = 1;
	curr_qtd->token.pid = pid_code;
	curr_qtd->token.status |= STATUS_ACTIVE_ENABLE;
	curr_qtd->token.error_counter = 3;

	//Create the data buffer
	if(bytes_to_transfer > 0)
	{
		_alloc_qtd_buffer(curr_qtd , bytes_to_transfer);
	}

	return curr_qtd;
}

struct _qtd * _create_setup_qtd(uint32_t next , uint8_t toggle , uint32_t bytes_to_transfer , uint8_t pid_code , uint8_t type , uint8_t request,
				uint8_t value_high , uint8_t value_low , uint16_t index , uint16_t length)
{
	struct _qtd * qtd = _create_qtd(next , toggle , bytes_to_transfer , pid_code);



	//Create the request associated with this setup
	struct _ehci_request * req = qtd->data_buffer[0];

	req->type = type;
	req->request = request;
	req->value_high = value_high;
	req->value_low = value_low;
	req->index = index;
	req->length = length;

	return qtd;
}

void _alloc_qtd_buffer(struct _qtd * curr_qtd , uint32_t buffer_size)
{
	//Allocate a full buffer, probably not neccesary
	struct _echi_request * buffer = _kalloc_align(4096 , 4096);

	memset(buffer , 0 , 4096);

	curr_qtd->data_buffer[0] = buffer;
}

//This QTD head sets the address of device
struct _qtd_head * _create_init_qtd(int device , int new_device)
{
	struct _qtd_head * head = _create_qtd_head(-1 , device , 0 , 1);

	struct _qtd * next = _create_qtd(-1 , 1 , 0 , IN_TOKEN);
	c_puts("Input QTD\n");	
	print_qtd(next);
	
	struct _qtd * setup_qtd = _create_setup_qtd(next , 0 , 8 , SETUP_TOKEN , 0 , 5 , 0 , 5 , 0 , 0);

	c_puts("Setup QTD\n");
	print_qtd(setup_qtd);

	c_puts("Current setup register\n");	
	print_qtd_buffer(setup_qtd->data_buffer[0] , 8);

	head->qtd.next = setup_qtd;

	return head;

}

struct _qtd_head * _create_descriptor_qtd(int device , int num)
{
	int i = 0;


	struct _qtd_head * head = _create_qtd_head(0 , device, 0 , 1);

	struct _qtd * next = _create_qtd(-1 , 1 , 18 , IN_TOKEN);
	c_puts("Input QTD\n");	
	print_qtd(next);

	data_buffer = next->data_buffer[0];

	struct _qtd * setup_qtd = _create_setup_qtd(next , 0 , 8 , SETUP_TOKEN , 0x80 , 6 , 1 , 0  , 0 , 18);

	c_puts("Setup QTD\n");
	print_qtd(setup_qtd);



	head->qtd.next = setup_qtd;


	//Just a placeholder 
	/*struct _qtd * next_head_setup = _create_setup_qtd(device);
	  struct _qtd_head * next_head = _create_qtd_head(head , device , 0 , 0);
	  next_head->qtd.next_qtd = next_head_setup;


	  head->qhlp |= (uint32_t)next_head;
	  */

	head->qhlp |= (uint32_t)head;


	return head;
}

	
	

struct _qtd_head * _create_qtd_head(uint32_t next , int device , int endpoint , int reclamation)
{
#ifdef USB_EHCI_DEBUG
	c_puts("Creating new qtd_head\n");
#endif
	//Need to be 32 byte aligned
	struct _qtd_head * curr_qtd_head = _kalloc_align(sizeof(struct _qtd_head) , 32);

	memset(curr_qtd_head , 0 , sizeof(struct _qtd_head));

	if(next == -1)
	{
		//Need to create a circular queue
		curr_qtd_head->qhlp |= (uint32_t)curr_qtd_head;
	}
	else
	{
		//Next must be 32 byte aligned
		curr_qtd_head->qhlp |= next;
	}

	curr_qtd_head->qhlp |= QUEUE_HEAD_TYPE;

	//Set the appropriate qtd head fields
	curr_qtd_head->device_address = device;
	curr_qtd_head->endpoint = endpoint;
	curr_qtd_head->endpoint_speed = EPS_HIGH_SPEED;
	curr_qtd_head->data_toggle = 1;
	curr_qtd_head->H = reclamation;
	curr_qtd_head->max_packet_length = _MAX_PACKET_LENGTH;
	
	curr_qtd_head->nak_count_reload = 15;

	curr_qtd_head->mult = 1;

	return curr_qtd_head;
}
