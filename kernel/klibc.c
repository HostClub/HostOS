/*
** SCCS ID:	@(#)klibc.c	1.1	03/31/11
**
** File:	klibc.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	C implementations of some kernel-level library routines
*/

#define	__KERNEL__20103__

#include "headers.h"

/*
** PUBLIC FUNCTIONS
*/

/*
** put_char_or_code( ch )
**
** prints the character on the console, unless it is a non-printing
** character, in which case its hex code is printed
*/

void put_char_or_code( int ch ) {

	if( ch >= ' ' && ch < 0x7f ) {
		c_putchar( ch );
	} else {
		c_printf( "\\x%02x", ch );
	}

}

/*
** _memclr - initialize all bytes of a block of memory to zero
**
** usage:  _memclr( buffer, length )
*/

void _memclr( void *buffer, uint32_t length ) {
	register unsigned char *buf = (unsigned char *)buffer;
	register uint32_t num = length;

	while( num-- ) {
		*buf++ = 0;
	}

}

/*
** _memcpy - copy a block from one place to another
**
** usage:  _memcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/

void _memcpy( void *destination, void *source, uint32_t length ) {
	register unsigned char *dst = (unsigned char *)destination;
	register unsigned char *src = (unsigned char *)source;
	register uint32_t num = length;

	while( num-- ) {
		*dst++ = *src++;
	}

}


/*
** _kpanic(msg)
**
** kernel-level panic routine
**
** usage:  _kpanic( mod, msg, code )
**
** Prefix routine for __panic() - can be expanded to do other things
** (e.g., printing a stack traceback)
*/

void _kpanic( char *mod, char *msg, status_t code ) {

	c_puts( "\n\n***** KERNEL PANIC *****\n\n" );
	c_printf( "Module: %s\n", mod );
	if( msg != NULL ) {
		_kprt_status( msg, code );
	}

	//
	// Pretty simple, actually
	//

	__panic( msg );

}


/*
** _kprt_status - print a status value to the console
**
** the 'msg' argument should contain a %s where
** the desired status value should be printed
**
** this is identical to prt_status(), but is here so that user
** routines which invoke it won't use the same version as the 
** kernel uses, which will simplify life for people implementing
** full VM.
*/

void _kprt_status( char *msg, status_t stat ) {
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
			c_printf( "Code %d\n", stat );
			return;
	
	}

	c_printf( msg, str );

}
