

#ifndef USB_EHCI_H
#define USB_EHCI_H

#include "pci.h"

void usb_ehci_init(struct _pci_dev * device);

void _isr_usb_int(int vector , int code);

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
}__attribute__((__packed__));

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

struct _qtd_head * _create_qtd_head(int device , int endpoint);

#endif
