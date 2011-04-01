/*
** SCCS ID:	@(#)klib.h	1.1	03/31/11
**
** File:	klib.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Kernel library routine definitions
*/

#ifndef _KLIB_H
#define _KLIB_H

#ifndef __ASM__20103__

#include "headers.h"

/*
** Prototypes
*/

/*
** put_char_or_code( ch )
**
** prints the character on the console, unless it is a non-printing
** character, in which case its hex code is printed
*/

void put_char_or_code( int ch );

/*
** _get_ebp - return current contents of EBP at the time of the call
**
** Could be used, e.g., by _kpanic() to print a traceback
*/

uint32_t _get_ebp( void );

/*
** _memclr - initialize all bytes of a block of memory to zero
**
** usage:  _memclr( buffer, length )
*/

void _memclr( void *buffer, uint32_t length );

/*
** _memcpy - copy a block from one place to another
**
** usage:  _memcpy( dest, src, length )
**
** may not correctly deal with overlapping buffers
*/

void _memcpy( void *destination, void *source, uint32_t length );

/*
** _kpanic - kernel-level panic routine
*/

void _kpanic( char *mod, char *msg, status_t code );

/*
** _kprt_status - print a status value to the console
**
** the 'msg' argument should contain a %s where
** the desired status value should be printed
*/

void _kprt_status( char *msg, status_t stat );

#endif

#endif
