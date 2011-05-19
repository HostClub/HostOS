/*
 ** SCCS ID:	@(#)ulibc.c	1.1	03/31/11
 **
 ** File:	ulibc.c
 **
 ** Author:	4003-506 class of 20103
 **
 ** Contributor:
 **
 ** Description:	C implementations of some user-level library routines
 */

#define	__KERNEL__20103__

#include "headers.h"

/*
 ** prt_status - print a status value to the console
 **
 ** the 'msg' argument should contain a %s where
 ** the desired status value should be printed
 **
 ** this is identical to prt_status(), but is here so that user
 ** routines which invoke it won't use the same version as the 
 ** kernel uses, which will simplify life for people implementing
 ** full VM.
 */

void prt_status( char *msg, status_t stat ) {
	char *str;

	if( msg == NULL ) {
		return;
	}

	switch( stat ) {

		case E_SUCCESS:
			str = "E_SUCCESS";
			break;

		case E_BAD_PARAM:
			str = "E_BAD_PARAM";
			break;

		case E_NO_QNODES:
			str = "E_NO_QNODES";
			break;

		case E_EMPTY:
			str = "E_EMPTY";
			break;

		case E_NOT_FOUND:
			str = "E_NOT_FOUND";
			break;

		default:
			c_printf( msg, "E_????" );
			c_printf( "Status code %d\n", stat );
			return;

	}

	c_printf( msg, str );

}

void memset(void * loc , int value , int num_bytes)
{
	//Not sure if this is neccesary
	char * byte_loc = loc;

	int i;
	for(i = 0; i < num_bytes; i++)
	{
		byte_loc[i] = value;
	}
}

char * split(char * buffer , char split_on)
{
	char * split_pos = buffer;

	int index = 0;

	while(split_pos[index] != '\0')
	{

		if(split_pos[index] == split_on)
		{
			split_pos[index] = '\0';
			break;
		}

		index++;
	}
	index++;

	return split_pos + index;
}

int strcmp(char * first , char * second)
{
	while(*first != '\0' && *second != '\0')
	{
		if(*first != *second)
		{
			return 0;
		}

		first++;
		second++;
	}

	return *first == '\0' && *second == '\0';
}

