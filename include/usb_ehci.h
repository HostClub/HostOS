

#ifndef USB_EHCI_H
#define USB_EHCI_H

#include "pci.h"

void usb_ehci_init(struct _pci_dev * device);

void _isr_usb_int(int vector , int code);

struct _qtd
{
	/*unsigned next_qtd: 27;
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
	  */

	uint32_t next_qtd;

	#define NEXT_QTD_OFFSET 5
	#define NEXT_QTD_TERMINATE (1 << 0)

	uint32_t alternate_qtd;

	#define ALTERNATE_QTD_OFFSET 5
	#define ALTERNATE_QTD_TERMINATE (1 << 0)

	uint32_t qtd_token;

	#define DATA_TOGGLE_OFFSET 31
	#define DATA_TOGGLE_ENABLE (1 << DATA_TOGGLE_OFFSET)
	#define BYTES_TO_TRANSFER_OFFSET 16
	#define INTERRUPT_ON_COMPLETE_ENABLE (1 << 15)

	#define PID_CODE_OFFSET 8

	#define OUT_TOKEN (0x00 << PID_CODE_OFFSET)
	#define IN_TOKEN (0x01 << PID_CODE_OFFSET)
	#define SETUP_TOKEN (0x02 << PID_CODE_OFFSET)

	#define STATUS_ACTIVE_ENABLE (1 << 7)

	void * buffer_list[5];
#define BUFFER_POINTER_OFFSET 12
}__attribute__((__packed__));

struct _qtd_head
{
	/*uint32_t qhlp: 27;
unsigned:2;
	 uint8_t queue_head_type: 2;
	*/
	uint32_t qhlp;
	#define QHLP_OFFSET 5
	#define QUEUE_HEAD_TYPE_OFFSET 2
	#define QUEUE_HEAD_TYPE (0x01 << QUEUE_HEAD_TYPE_OFFSET)
	#define QHLP_TERMINATE 1

	uint32_t endpoint_char;

	#define MAX_PACKET_LENGTH_OFFSET 16
	
	#define EPS_OFFSET 12
	#define EPS_HIGH_SPEED (0x02 << EPS_OFFSET)
	
	#define ENDPOINT_NUMBER_OFFSET 8

	#define DEVICE_ADDRESS_OFFSET 0
	uint32_t endpoint_cap;


	#define CURR_QTD_OFFSET 5
	uint32_t curr_qtd;

	struct _qtd qtd;
}__attribute__((__packed__));

struct _ehci_request
{
	uint8_t type;
	uint8_t request;
	uint8_t value_high;
	uint8_t value_low;
	uint16_t index;
	uint16_t length;
}__attribute__((packed));

struct _qtd * _create_qtd(uint32_t next , uint8_t toggle , uint32_t bytes_to_transfer , uint8_t pid_code);

void _alloc_qtd_buffer(struct _qtd * curr_qtd , uint32_t buffer_size);

struct _qtd_head * _create_setup_qtd(int device);

struct _qtd_head * _create_qtd_head(uint32_t next , int device , int endpoint);

#endif
