h23073
s 00143/00000/00000
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
** File:	scheduler.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Scheduler module
*/

#define	__KERNEL__20103__

#include "headers.h"

#include "scheduler.h"
#include "system.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

// the ready queue

queue_t _ready[ N_PRIORITIES ];

// the current process

pcb_t *_current;

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/*
** _schedule( pcb, priority )
**
** put PCB into the ready queue with the specified priority
*/

void _schedule( pcb_t *pcb, uint8_t prio ) {
	key_t key;
	status_t stat;

	key.u = 0;

	if( pcb == NULL ) {
		_kpanic( "_schedule", "null PCB", 0 );
	}

	if( prio >= N_PRIORITIES ) {
		c_printf( "** scheduler changed %d prio\n", pcb->pid );
		prio = N_PRIORITIES - 1;
	}

	pcb->state = READY;
	stat = _q_append( &_ready[prio], (void *)pcb, key );
	if( stat != E_SUCCESS ) {
		_kpanic( "_schedule", "append status %s", stat );
	}

}

/*
** _dispatch( void )
**
** dispatch the next process
*/

void _dispatch( void ) {
	int i;
	status_t stat;

	//
	// Loop until we find a process to dispatch.
	//

	while( TRUE ) {

		for( i = 0; i < N_PRIORITIES; ++i ) {
			if( !_q_empty(&_ready[i]) ) {
				break;
			}
		}

		if( i >= N_PRIORITIES ) {
			_kpanic( "_dispatch", "all queues empty", 0 );
		}

		stat = _q_remove( &_ready[i], (void **) &_current );
		if( stat != E_SUCCESS ) {
			_kpanic( "_dispatch", "queue remove status %s", stat );
		}

		if( _current->state == READY ) {
			break;
		}

		if( _current->state == KILLED ) {
			_zombify( _current );
		} else {
			_kpanic( "_dispatch", "non-ready, state(+100k)",
				 _current->state + 100000 );
		}

	}

	_current->state = RUNNING;
}

/*
** _sched_init( void )
**
** initialize the scheduler module
*/

void _sched_init( void ) {
	int i;

	for( i = 0; i < N_PRIORITIES; ++i ) {
		_q_reset( &_ready[i], NULL, NULL );
	}

	c_puts( " scheduler" );

}
E 1
