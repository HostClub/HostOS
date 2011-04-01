h39547
s 00007/00000/00047
d D 1.5 06/10/14 12:44:48 wrc 5 4
c added __pause()
e
s 00001/00001/00046
d D 1.4 05/03/15 15:36:39 wrc 4 3
c (wrc) minor tweaks
e
s 00029/00011/00018
d D 1.3 02/05/16 15:14:53 kar 3 2
c Version 2: improvements based on my experience with 20013 506.
e
s 00009/00004/00020
d D 1.2 02/01/22 16:41:05 kar 2 1
c Add interrupt stuff, clean up code
e
s 00024/00000/00000
d D 1.1 02/01/15 16:18:25 kar 1 0
c date and time created 02/01/15 16:18:25 by kar
e
u
U
f e 0
t
T
I 1
/*
D 2
** SCCS ID: %W%        %G%
E 2
I 2
D 3
** SCCS ID:		%W%	%G%
E 3
I 3
** SCCS ID:	%W%	%G%
E 3
E 2
**
D 3
** File:		startup.h
E 3
I 3
** File:	startup.h
E 3
**
D 3
** Author:		K. Reek
E 3
I 3
** Author:	K. Reek
E 3
**
D 3
** Contributors:	
E 3
I 3
D 4
** Contributor:	
E 4
I 4
** Contributor:	Warren R. Carithers
E 4
E 3
**
D 3
** Description:		Prototypes for support routines included
**			in startup.S
E 3
I 3
** Description:	Prototypes for support routines included
**		in startup.S
E 3
*/

#ifndef _STARTUP_H_
#define _STARTUP_H_

/*
D 2
** inport, outport: reading and writing single bytes from input
E 2
I 2
D 3
** __inport, __outport: reading and writing single bytes from input
E 2
**	and output ports.
E 3
I 3
** Name:	__inb, __inw, __inl
**
** Description:	Read one byte, word, or longword from an input port
** Argument:	The port from which to read
** Returns:	The data we read (extended to an integer)
E 3
*/
D 2
int inport( int port );
void outport( int port, int value );
E 2
I 2
D 3
int __inport( int port );
void __outport( int port, int value );
E 3
I 3
int __inb( int port );
int __inw( int port );
int __inl( int port );
E 3
E 2

I 2
/*
D 3
** __get_flags: return the current processor flags
E 3
I 3
** Name:	__outb, __outw, __outl
**
** Description:	Write one byte, word, or longword to an output port
** Arguments:	The port to write to, and the value to write (only the
**		low-order bytes are written)
E 3
*/
I 3
void __outb( int port, int value );
void __outw( int port, int value );
void __outl( int port, int value );

/*
** Name:	__get_flags
**
** Description:	Get the current processor flags
** Returns:	The flags
*/
E 3
int __get_flags( void );

I 5
/*
** Name:	__pause
**
** Description:	Pause until something happens
*/
void __pause( void );

E 5
E 2
#endif
E 1
