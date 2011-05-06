

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
	*/

	uint32_t next_qtd;
	#define NEXT_QTD_OFFSET 5

	#define NEXT_QTD_TERMINATE 0x01

	  /*uint32_t alternate_qtd: 27;
uint32_t :4;
	  uint32_t alternate_qtd_terminate: 1;
	*/
	uint32_t alternate_qtd;
	#define ALTERNATE_QTD_OFFSET 5

	#define ALTERNATE_QTD_TERMINATE 0x01

	
	uint32_t qtd_token;
	#define DATA_TOGGLE_OFFSET 31
	#define DATA_TOGGLE_ENABLE (1 << DATA_TOGGLE_OFFSET)

	#define BYTES_TO_TRANSFER_OFFSET 16

	#define INTERRUPT_ON_COMPLETE_ENABLE (1 << 15)

	#define PID_CODE_OFFSET 8

	#define OUT_TOKEN (0x00)
	#define IN_TOKEN (0x01)
	#define SETUP_TOKEN (0x02)

	#define STATUS_ACTIVE_ENABLE (1 << 7)

	/*uint32_t data_toggle: 1;

	  uint32_t bytes_to_transfer: 14;

	  uint32_t interrupt_on_complete: 1;

	  uint32_t current_page: 2;

	  uint32_t error_counter: 2;

	  uint32_t pid_code: 2;
	
	  uint32_t status: 8;
	*/
	  void * buffer_list[5];
#define BUFFER_POINTER_OFFSET 12
}__attribute__((packed));

struct _qtd_head
{
	
	 /*uint32_t queue_head_type: 2;
	 uint32_t queue_terminate: 1;
	uint32_t qhlp: 27;
uint32_t:2;
	*/

	uint32_t qhlp;
	#define QHLP_OFFSET 5
	
	#define QUEUE_HEAD_TYPE (0x01 << 1)
	#define QUEUE_HEAD_TERMINATE 1


	uint32_t endpoint_char;

	#define MAX_PACKET_LENGTH_OFFSET 16


	/*
	 uint32_t nak_reload: 3;
	 uint32_t control_endpoint: 1;
	 uint32_t max_packet_length: 10;

	 uint32_t reclamation_head: 1;
	 uint32_t data_toggle_control: 1;

	 uint32_t endpoint_speed: 2;
	 */
	#define EPS_OFFSET 12
	#define EPS_HIGH_SPEED (0x02 << EPS_OFFSET)

	#define ENDPOINT_OFFSET 8

	#define DEVICE_OFFSET 0
/*
	 uint32_t endpoint_number: 3;

	 uint32_t inactivate_on_next: 1;

	 uint32_t device_address: 6;
*/


	 //change this to bitfields later
	 uint32_t endpoint_cap;

	 uint32_t curr_qtd;
	#define CURR_QTD_OFFSET 5

	 struct _qtd qtd;
}__attribute__((packed));

struct _ehci_request
{
	uint8_t type;
	uint8_t request;
	uint8_t value_low;
	uint8_t value_high;
	uint16_t index;
	uint16_t length;
} __attribute__((packed));


struct _qtd * _create_qtd(uint32_t next , uint8_t toggle , uint32_t bytes_to_transfer , uint8_t pid_code);

void _alloc_qtd_buffer(struct _qtd * curr_qtd , uint32_t buffer_size);

struct _qtd_head * _create_setup_qtd(int device);

struct _qtd_head * _create_qtd_head(uint32_t next , int device , int endpoint);

#endif
