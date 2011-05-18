#include "types.h"

uint32_t usb_ms_write(uint32_t id , void * buffer , uint32_t bytes_to_write);

uint32_t usb_ms_read(uint32_t id , void * buffer , uint32_t bytes_to_write);

void _configure_port(uint32_t port);
