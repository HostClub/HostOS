/*
** SCCS ID:	@(#)sio.h	1.1        03/31/11
**
** File:	sio.h
**
** Author:	Warren R. Carithers
**
** Contributor:
**
** Description:	SIO definitions
*/

#ifndef _SIO_H
#define _SIO_H

/*
** General (C and/or assembly) definitions
*/

// sio interrupt settings

#define	SIO_TX		0x01
#define	SIO_RX		0x02
#define	SIO_BOTH	(SIO_TX | SIO_RX)

#ifndef __ASM__20103__

/*
** Start of C-only definitions
*/

/*
** _isr_sio(vector, code)
**
** SIO interrupt service routine
*/

void _isr_sio( int vector, int code );

/*
** _sio_init()
**
** Initialize the UART chip.
*/

void _sio_init( void );

/*
** _sio_enable()
**
** enable/disable SIO interrupts
**
** usage:       old = _sio_enable( which )
**
** enables interrupts according to the 'which' parameter
**
** returns the prior settings
*/

uint8_t _sio_enable( uint8_t which );

/*
** _sio_disable()
**
** disable/disable SIO interrupts
**
** usage:       old = _sio_disable( which )
**
** disables interrupts according to the 'which' parameter
**
** returns the prior settings
*/

uint8_t _sio_disable( uint8_t which );

/*
** _sio_readc()
**
** get the next input character
**
** usage:	ch = _sio_readc()
**
** returns the next character, or -1 if no character is available
*/

int _sio_readc( void );

/*
** _sio_reads()
**
** get an input line, up to a specific number of characters
**
** usage:	num = _sio_reads(buffer,length)
**
** returns the number of characters put into the buffer, or 0 if no
** characters are available
*/

int _sio_reads( char *buffer, int length );

/*
** _sio_writec( ch )
**
** write a character to the serial output
**
** usage:	_sio_writec( ch )
*/

void _sio_writec( int ch );

/*
** _sio_writes( ch )
**
** write a buffer of characters to the serial output
**
** usage:	_sio_writes( char *buffer, int length )
**
** returns the count of bytes transferred
*/

int _sio_writes( char *buffer, int length );

/*
** _sio_dump()
**
** dump the contents of the SIO buffers
*/

void _sio_dump( void );

#endif

#endif
