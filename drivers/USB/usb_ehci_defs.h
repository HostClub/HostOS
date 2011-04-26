//PCI Stuff
#define BASE_MASK 0xFFFFFF00
uint32_t _BASE;

#define USBLEGSUP_OFFSET 0x00
uint32_t _USBLEGSUP;

#define HC_OS_OWNED_OFFSET 24
#define HC_OS_OWNED_ENABLE (1 << HC_OS_OWNED_OFFSET)

#define HC_BIOS_OWNED_ENABLE (1 << 16)

#define USBLEGCTLSTS_OFFSET 0x04
uint32_t _USBLEGCTLSTS;

#define SMI_OS_OWN_ENABLE (1<<13)

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


#define ASYNC_ENABLE (1 << 5)
#define HCRESET_ENABLE (1 << 1)
#define RUN_ENABLE (1 << 0)


#define USBSTS_OFFSET 4
uint32_t * _USBSTS;

#define USB_INT (1 << 0)
#define PORT_CHANGE_DETECT (1 << 2)
#define FRAME_LIST_ROLLOVER_ENABLE (1 << 3)
#define HCHALTED_ENABLE (1 << 12)

#define USBINTR_OFFSET 8
uint32_t * _USBINTR;


//Interrupt enable flags

//Refer to table 2-11 of the EHCI spec

#define ASYNC_ADV_INT_ENABLE 	0x00000020
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

#define CONFIGFLAG_ENABLE (1<<0)

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
#define LINE_STATUS_MASK (0x03 << LINE_STATUS_OFFSET)

#define SEO_STATE 	0x00
//Low Speed Device
#define K_STATE	 	0x01
#define J_STATE 	0x02
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
uint32_t _MAX_PACKET_LENGTH = 64;

/*
struct _qtd
{
	unsigned next_qtd: 27;
	unsigned :4;
	unsigned next_qtd_terminate: 1;
	
	unsigned alternate_qtd: 27;
	unsigned :4;
	unsigned alternate_qtd_terminate: 1;
	
	unsigned data_toggle: 1;

	unsigned bytes_to_transfer: 14;

	unsigned interrupt_on_complete: 1;

	unsigned current_page: 2;
	
	unsigned error_counter: 2;

	unsigned pid_code: 2;
	#define OUT_TOKEN 0x00
	#define IN_TOKEN 0x01
	#define SETUP_TOKEN 0x02

	unsigned status: 8;

	void * buffer_list[5];
	#define BUFFER_POINTER_OFFSET 12
}__attribute__((packed));

struct _qtd_head
{
	uint32_t qhlp: 27;
	unsigned:2;
	uint8_t queue_head_type: 2;
	#define QUEUE_HEAD_TYPE 0x01

	uint8_t queue_terminate: 1;

	uint8_t nak_reload: 3;
	uint8_t control_endpoint: 1;
	uint16_t max_packet_length: 10;

	uint8_t reclamation_head: 1;
	uint8_t data_toggle_control: 1;

	uint8_t endpoint_speed: 2;
	#define EPS_HIGH_SPEED 0x02

	uint8_t endpoint_number: 3;

	uint8_t inactivate_on_next: 1;

	uint8_t device_address: 6;

	//change this to bitfields later
	uint32_t endpoint_cap;

	uint32_t curr_qtd: 27;

	struct _qtd qtd;
}__attribute__((packed));
*/


