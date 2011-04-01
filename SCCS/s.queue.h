h62922
s 00188/00000/00000
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
** File:	queue.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Queue-related definitions
*/

#ifndef _QUEUE_H
#define _QUEUE_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __ASM__20103__

/*
** Start of C-only definitions
*/

// number of queue nodes we'll need

#define	N_QNODES	(N_PCBS + N_STACKS + 5)

// pseudo-function:  is a queue empty?

#define _q_empty(q)	((q)->length < 1)

/*
** Types
**
** Our queues are generic, self-ordering queues.  The queue itself
** is a doubly-linked list of queue nodes, which in turn point to
** the information queued up in this position.  Each queue has an
** associated ordering routine which is used by the insertion 
** routine to order the queue elements.  This allows us to have
** different sorting criteria for different queues, but manage them
** with one set of functions.
*/

// queue ordering key

typedef union key {
	uint32_t	u;	// unsigned integer
	int32_t		i;	// signed integer
	char		c[4];	// four-byte string
} key_t;

// internal queue node

typedef struct qnode {
	key_t		key;	// sorting key
	void		*data;	// information queued in this position
	struct qnode	*prev;	// predecessor in the queue
	struct qnode	*next;	// successor in the queue
} qnode_t;

// per-queue processing routines

typedef int (*q_compare_t)(qnode_t *, qnode_t *);
typedef int (*q_remove_t)(key_t, key_t);

// the queue itself

typedef struct queue {
	q_compare_t	compare;	// ptr to comparison routine
	q_remove_t	remove;		// ptr to removal routine
	uint_t		length;		// element count
	qnode_t		*head;		// first node in queue
	qnode_t		*tail;		// last node in queue
} queue_t;

/*
** Globals
*/

/*
** Prototypes
*/

/*
** _q_init()
**
** initialize the queue module
*/

void _q_init( void );

/*
** _q_dump(which,queue)
**
** dump the contents of the specified queue to the console
*/

void _q_dump( char *which, queue_t *queue );

/*
** _q_reset( queue, compare )
**
** reset 'queue' to its empty state, with 'compare' as its ordering routine
*/

void _q_reset( queue_t *queue, q_compare_t compare, q_remove_t remove );

/*
** status = _q_remove( queue, data )
**
** remove the first element from 'queue'; place it into 'data' if
** an element is found
**
** returns the status of the removal attempt
*/

status_t _q_remove( queue_t *queue, void **data );

/*
** status = _q_remove_by_key( queue, data, key )
**
** locate an element in 'queue' identified by 'key' by using the
** queue's built-in removal routine; place it into 'data' if one
** is found
**
** returns the status of the removal attempt
*/

status_t _q_remove_by_key( queue_t *queue, void **data, key_t key );

/*
** status = _q_append( queue, data, key )
**
** adds 'data' to the end of 'queue'; 'key' is copied into the qnode,
** but is otherwise ignored
**
** returns a status value indicating the success or failure of the attempt
*/

status_t _q_insert( queue_t *queue, void *data, key_t key );

/*
** status = _q_insert( queue, data, key )
**
** inserts 'data' into 'queue', with position determined by 'key'
**
** returns a status value indicating the success or failure of the attempt
*/

status_t _q_append( queue_t *queue, void *data, key_t key );

/*
** n = _compare_uint_asc( qnode_t *n1, qnode_t *n2 );
**
** compares the two supplied qnodes
**
** returns -1 if n1 < n2, 0 if they are equal, and +1 if n1 > n2
*/

int _compare_uint_asc( qnode_t *n1, qnode_t *n2 );

/*
** n = _remove_if_equal( key_t k1, key_t k2 );
**
** compares the two supplied keys
**
** returns 1 if n1 = n2, 0 if n1 != n2
*/

int _remove_if_equal( key_t k1, key_t k2 );

/*
** n = _remove_if_le_u( key_t k1, key_t k2 );
**
** compares the two supplied keys
**
** returns 1 if n1 <= n2, 0 if n1 > n2
*/

int _remove_if_le_u( key_t k1, key_t k2 );

#endif

#endif
E 1
