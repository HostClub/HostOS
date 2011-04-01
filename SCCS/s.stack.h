h20466
s 00081/00000/00000
d D 1.1 11/03/31 18:06:25 wrc 1 0
c date and time created 11/03/31 18:06:25 by wrc
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
** File:	stack.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Stack-related definitions
*/

#ifndef _STACK_H
#define _STACK_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// size of a user stack (in longwords)

#define	STACK_SIZE	1024

#ifndef __ASM__20103__

/*
** Start of C-only definitions
*/

/*
** Types
*/

// user stack

typedef uint32_t	stack_t[STACK_SIZE];

/*
** Globals
*/

extern stack_t _system_stack;
extern uint32_t *_system_esp;

/*
** Prototypes
*/

/*
** _stack_alloc(stack)
**
** allocate a stack structure
**
** returns a pointer to the stack, or NULL on failure
*/

status_t _stack_alloc( stack_t **stack );

/*
** _stack_free(stack)
**
** deallocate a stack structure
**
** returns the status from the _q_insert() call
*/

status_t _stack_free( stack_t *stack );

/*
** _stack_init()
**
** initializes all stack-related data structures
*/

void _stack_init( void );

#endif

#endif
E 1
