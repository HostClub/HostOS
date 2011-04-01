h61880
s 00002/00002/00059
d D 1.4 07/04/03 14:54:36 wrc 4 3
c changed panic to __panic for consistency
e
s 00002/00002/00059
d D 1.3 06/10/14 13:12:27 wrc 3 2
c updated for sp1/sp2 compatability
e
s 00024/00007/00037
d D 1.2 02/05/16 15:14:54 kar 2 1
c Version 2: improvements based on my experience with 20013 506.
e
s 00044/00000/00000
d D 1.1 02/01/22 16:41:18 kar 1 0
c date and time created 02/01/22 16:41:18 by kar
e
u
U
f e 0
t
T
I 1
/*
** SCCS ID:	%W%	%G%
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
D 2
** Name:	__init
** Description:	Completes the system initialization.
E 2
I 2
D 4
** Name:	panic
E 4
I 4
** Name:	__panic
E 4
**
** Description:	Called when we find an unrecoverable error, this
**		routine disables interrupts, prints a description of
**		the error and then goes into a hard loop to prevent
**		any further processing.
**
** Arguments:	The reason for the panic
E 2
*/
D 2
void init( void );
E 2
I 2
D 4
void panic( char *reason );
E 4
I 4
void __panic( char *reason );
E 4
E 2

/*
I 2
D 3
** Name:	init_interrupts
E 3
I 3
** Name:	__init_interrupts
E 3
**
** Description:	Sets up the Interrupt Descriptor Table and initializes
**		the PIC.
*/
D 3
void init_interrupts( void );
E 3
I 3
void __init_interrupts( void );
E 3

/*
E 2
** Name:	__install_isr
D 2
** Arguments:	The interrupt vector number, and a pointer to the handler
** Returns:	A pointer to the old handler
E 2
I 2
**
E 2
** Description:	Call this routine to install your interrupt handler for
**		a specific vector.  The old handler is returned so you can
D 2
**		reinstall it when you're finished.
E 2
I 2
**		reinstall it when you're finished if you want.
** Arguments:	The interrupt vector number, and a pointer to the handler
**		function
** Returns:	A pointer to the old handler function
E 2
*/
void ( *__install_isr( int vector, void ( *handler )( int vector, int code ) ) )( int vector, int code );

/*
** Name:	__delay
D 2
** Arguments:	tenths of a second
E 2
I 2
**
E 2
** Description:	Delays execution by counting for the specified number of
**		tenths of a second.  If interrupts are enabled when this
**		is called, they remain enabled and interrupts may occur.
I 2
** Arguments:	tenths of a second
E 2
*/
void __delay( int tenths );


#endif
E 1
