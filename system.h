/*
** SCCS ID:	@(#)system.h	1.1	03/31/11
**
** File:	system.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Miscellaneous OS support functions
*/

#ifndef _SYSTEM_H
#define _SYSTEM_H

/*
** General (C and/or assembly) definitions
*/

#ifndef __ASM__20103__

#include "types.h"

#include "process.h"
#include "stack.h"

/*
** Start of C-only definitions
*/

// access function argument 'n' via process context 'c'

#define	ARG(c,n)	(((uint32_t *)((c)+1))[n])

/*
** Prototypes
*/

/*
** _setup_stack - Initialize a process context
**
** usage:
**	context = _setup_stack( stack_t stack, uint32_t entry );
**
** ASSUMES that the stack argument points to a stack which can be
** reinitialized.
*/

context_t *_setup_stack( stack_t *stack, uint32_t entry );

/*
** _zombify - create a "walking dead" process
**
** Called by _dispatch() to deal with a KILLED process, and by
** _sys_exit() to deal with an exiting process.
*/

void _zombify( pcb_t *pcb );

/*
** _init - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/

void _init( void );

#endif

#endif
