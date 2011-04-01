/*
** SCCS ID:	@(#)support.h	1.4	04/03/07
**
** File:	support.h
**
** Author:	K. Reek
**
** Description:	Declarations for functions provided in support.c, and
**		some hardware characteristics needed in the initialization.
**
*/

#ifndef	_SUPPORT_H
#define	_SUPPORT_H

#define	IDT_PADDR	0x2400

/*
** Name:	__panic
**
** Description:	Called when we find an unrecoverable error, this
**		routine disables interrupts, prints a description of
**		the error and then goes into a hard loop to prevent
**		any further processing.
**
** Arguments:	The reason for the panic
*/
void __panic( char *reason );

/*
** Name:	__init_interrupts
**
** Description:	Sets up the Interrupt Descriptor Table and initializes
**		the PIC.
*/
void __init_interrupts( void );

/*
** Name:	__install_isr
**
** Description:	Call this routine to install your interrupt handler for
**		a specific vector.  The old handler is returned so you can
**		reinstall it when you're finished if you want.
** Arguments:	The interrupt vector number, and a pointer to the handler
**		function
** Returns:	A pointer to the old handler function
*/
void ( *__install_isr( int vector, void ( *handler )( int vector, int code ) ) )( int vector, int code );

/*
** Name:	__delay
**
** Description:	Delays execution by counting for the specified number of
**		tenths of a second.  If interrupts are enabled when this
**		is called, they remain enabled and interrupts may occur.
** Arguments:	tenths of a second
*/
void __delay( int tenths );


#endif
