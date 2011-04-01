/*
** SCCS ID:	@(#)startup.h	1.5	10/14/06
**
** File:	startup.h
**
** Author:	K. Reek
**
** Contributor:	Warren R. Carithers
**
** Description:	Prototypes for support routines included
**		in startup.S
*/

#ifndef _STARTUP_H_
#define _STARTUP_H_

/*
** Name:	__inb, __inw, __inl
**
** Description:	Read one byte, word, or longword from an input port
** Argument:	The port from which to read
** Returns:	The data we read (extended to an integer)
*/
int __inb( int port );
int __inw( int port );
int __inl( int port );

/*
** Name:	__outb, __outw, __outl
**
** Description:	Write one byte, word, or longword to an output port
** Arguments:	The port to write to, and the value to write (only the
**		low-order bytes are written)
*/
void __outb( int port, int value );
void __outw( int port, int value );
void __outl( int port, int value );

/*
** Name:	__get_flags
**
** Description:	Get the current processor flags
** Returns:	The flags
*/
int __get_flags( void );

/*
** Name:	__pause
**
** Description:	Pause until something happens
*/
void __pause( void );

#endif
