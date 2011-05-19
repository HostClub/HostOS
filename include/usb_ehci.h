

#ifndef USB_EHCI_H
#define USB_EHCI_H

#include "pci.h"

void usb_ehci_init(struct _pci_dev * device);

void _isr_usb_int(int vector , int code);


#define QTD_TERMINATE 0x01

#define OUT_TOKEN (0x00)
#define IN_TOKEN (0x01)
#define SETUP_TOKEN (0x02)

#define STATUS_ACTIVE_ENABLE (1 << 7)

#define QUEUE_HEAD_TYPE_OFFSET 1

#define QUEUE_HEAD_TYPE (0x01) << QUEUE_HEAD_TYPE_OFFSET


#define BUFFER_SIZE 4096
#define MAX_BYTES_TRANSFER 5 * BUFFER_SIZE

struct _qtd_token
{
	uint8_t status;

	uint8_t pid:          2;
	uint8_t error_counter: 2;
	uint8_t curr_page:     3;
	uint8_t interrupt:    1;
	uint16_t bytes_to_transfer:      15;
	uint16_t data_toggle:  1;

} __attribute__((packed));

struct _qtd
{
	uint32_t next;
	uint32_t alternate;
	struct _qtd_token token;
	uint32_t data_buffer[5];
	uint32_t extend_data_buffer[5];	
} __attribute__((packed));


struct _qtd_head
{
	uint32_t qhlp;

	uint32_t device_address:       7;
	uint32_t inactive:            1;
	uint32_t endpoint:            4;
	uint32_t endpoint_speed:       2;
	#define EPS_HIGH_SPEED 0x02
	uint32_t data_toggle:   1;
	uint32_t H:                   1;
	uint32_t max_packet_length:    11;
	uint32_t control_edndpoint_flag: 1;
	uint32_t nak_count_reload:      4;
	
	uint8_t interrupt_schedule_mask;
	uint8_t split_completion_mask;
	uint16_t hub_address:             7;
	uint16_t port_number:          7;
	uint16_t mult:                2;
	
	uint32_t curr_qtd;
	struct _qtd qtd;
} __attribute__((packed));

struct _ehci_request
{
	uint8_t type;
	uint8_t request;
	uint8_t value_high;
	uint8_t value_low;
	uint16_t index;
	uint16_t length;
} __attribute__((packed));


struct _qtd * _create_qtd(uint32_t next , uint8_t toggle , uint32_t bytes_to_transfer , uint8_t pid_code);

void _alloc_qtd_buffer(struct _qtd * curr_qtd , uint32_t buffer_size);

struct _qtd * _create_setup_qtd(uint32_t next , uint8_t toggle , uint32_t bytes_to_transfer , uint8_t pid_code , uint8_t type , uint8_t request ,
							uint8_t value_low , uint8_t value_high , uint16_t index , uint16_t length);

struct _qtd_head * _create_qtd_head(uint32_t next , int device , int endpoint , int reclamation);

void print_qtd(struct _qtd * qtd);

void print_qtd_head(struct _qtd_head * qtd_head);

void print_qtd_buffer(uint8_t * qtd_buffer , int buffer_size);

//Helper methods

void _free_qtd_head(struct _qtd_head * head);

void _add_async_schedule(struct _qtd_head * head);

uint32_t _perform_async_schedule();

uint32_t _check_port_status(int port);


#endif
