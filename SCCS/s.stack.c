h58107
s 00103/00000/00000
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
** File:	stack.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Stack-related routines
*/

#define	__KERNEL__20103__

#include "headers.h"

#include "stack.h"
#include "queue.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

static queue_t _stack_avail;
static stack_t _stacks[ N_STACKS ];

/*
** PUBLIC GLOBAL VARIABLES
*/

stack_t _system_stack;	// the OS runtime stack
uint32_t *_system_esp;	// initial ESP for OS stack

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** status = _stack_alloc( &stack )
**
** allocate a stack and place a pointer to it into 'stack'
**
** returns the allocation status
*/

status_t _stack_alloc( stack_t **new ) {
	return( _q_remove( &_stack_avail, (void **) new ) );
}

/*
** status = _stack_free( stack )
**
** return 'stack' to the free stack list
**
** returns the deallocation status
*/

status_t _stack_free( stack_t *old ) {
	key_t key;
	if( old != NULL ) {
		key.u = 0;
		_memclr( (void *)old, sizeof(stack_t) );
		return( _q_append( &_stack_avail, (void *) old, key ) );
	}
	return( E_BAD_PARAM );
}

/*
** _stack_init()
**
** initialize the process module
*/

void _stack_init( void ) {
	status_t stat;
	key_t key;
	int i;

	_q_reset( &_stack_avail, NULL, NULL );

	key.u = 0;

	for( i = 0; i < N_STACKS; ++i ) {
		stat = _stack_free( &_stacks[i] );
		if( stat != E_SUCCESS ) {
			_kpanic( "_stack_init", "_stack_free status %s", stat );
		}
	}

	c_puts( " stack" );
}
E 1
