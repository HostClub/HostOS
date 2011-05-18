#include "usb_mass_storage.h"
#include "usb_ehci.h"


//ID in this case is the port number
//Returns the bytes written or error
uint32_t usb_ms_write(uint32_t id , void * buffer , uint32_t bytes_to_write)
{
	//First lets check if the port is even plugged in

	if(!_check_port_status(id))
	{
		//This port is not plugged in
		return -1;
	}

	//Next lets see if it is configured
	_configure_port(id);


	//Check to see what endpoint we are supposed to be sending this to?
	//Gotta find some LUN to


	int endpoint = 0;
	//Ok, now to the actual write

	//First we create the command to write
	struct _qtd_head * start_head = _create_qtd_head(0 , id , endpoint , 1);

	struct _qtd * input = _create_qtd(-1 , 1 , 0 , IN_TOKEN);
	struct _qtd * output = _create_qtd(input , 1 , 32 , OUT_TOKEN);

	//Create SCSI command
	//output->data_buffer[0] = scsi command

	start_head->qtd.next = output;

	int max_bytes_to_write = MAX_BYTES_TRANSFER;

	int byte_count = bytes_to_write;
	int i;

	struct _qtd_head * prev_head = start_head;
	//Should probably link all of these together
	for(i = 0; i < (bytes_to_write / max_bytes_to_write); i++)
	{
		struct _qtd_head * head = _create_qtd_head(0 , id , endpoint , 1);
		prev_head->qhlp |= (uint32_t)head;

		input = _create_qtd(-1 , 1 , 0 , IN_TOKEN);

		int bytes_to_transfer = max_bytes_to_write;

		if(byte_count < max_bytes_to_write)
		{
			bytes_to_transfer = byte_count;
		}

		output = _create_qtd(input , 1 , bytes_to_transfer , OUT_TOKEN);

		//Set all the pointers. Since we are reading from a contiguous buffer, 
		//We can just keep adding from the original pointer
		int j;
		for(j = 0; j < (bytes_to_transfer / BUFFER_SIZE); j++)
		{
			output->data_buffer[j] = buffer + i * max_bytes_to_write + j * BUFFER_SIZE;
		}

		head->qtd.next = output; 

		prev_head = head;
	}

	//Here we could perform the async schedule while we wait for the 
	//Reclamation bit to be set

	_add_async_schedule(start_head);
	while(_perform_async_schedule())
	{
		//Could add up bytes transferred here
		//_perform_async_schedule();

	}
	_free_qtd_head(start_head);
}

uint32_t usb_ms_read(uint32_t id , void * buffer , uint32_t bytes_to_read)
{
	if(!_check_port_status(id))
	{
		//This port is not plugged in
		return -1;
	}

	//Next lets see if it is configured
	_configure_port(id);


	//Check to see what endpoint we are supposed to be sending this to?
	//Gotta find some LUN to


	int endpoint = 0;
	//Ok, now to the actual write

	//First we create the command to write
	struct _qtd_head * start_head = _create_qtd_head(0 , id , endpoint , 1);

	struct _qtd * input = _create_qtd(-1 , 1 , 0 , IN_TOKEN);
	struct _qtd * output = _create_qtd(input , 1 , 32 , OUT_TOKEN);

	//Create SCSI command
	//output->data_buffer[0] = scsi command

	start_head->qtd.next = output;

	int max_bytes_to_read = MAX_BYTES_TRANSFER;

	int byte_count = bytes_to_read;
	int i;

	struct _qtd_head * prev_head = start_head;
	//Should probably link all of these together
	for(i = 0; i < (bytes_to_read / max_bytes_to_read); i++)
	{
		struct _qtd_head * head = _create_qtd_head(0 , id , endpoint , 1);
		prev_head->qhlp |= (uint32_t)head;

		int bytes_to_transfer = max_bytes_to_read;

		if(byte_count < max_bytes_to_read)
		{
			bytes_to_transfer = byte_count;
		}

		input = _create_qtd(-1 , 1 , bytes_to_transfer , IN_TOKEN);

		//Set all the pointers. Since we are reading from a contiguous buffer, 
		//We can just keep adding from the original pointer
		int j;
		for(j = 0; j < (bytes_to_transfer / BUFFER_SIZE); j++)
		{
			output->data_buffer[j] = buffer + i * max_bytes_to_read + j * BUFFER_SIZE;
		}

		head->qtd.next = output; 

		byte_count -= bytes_to_transfer;
		prev_head = head;
	}

	//Here we could perform the async schedule while we wait for the 
	//Reclamation bit to be set

	_add_async_schedule(start_head);
	while(_perform_async_schedule())
	{
		//Could add up bytes transferred here
		//_perform_async_schedule();

	}
	_free_qtd_head(start_head);
}



