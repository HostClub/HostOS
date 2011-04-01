/*
** SCCS ID:	@(#)queue.c	1.1	03/31/11
**
** File:	queue.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Queue module
*/

#define	__KERNEL__20103__

#include "headers.h"

#include "queue.h"
#include "process.h"

/*
** PRIVATE DEFINITIONS
*/

// how many qnodes will we create?

#define	N_QNODES	(N_PCBS + N_STACKS + 5)

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// qnodes

static qnode_t _qnodes[ N_QNODES ];
static qnode_t *_qnode_avail;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** status = _qnode_alloc( &qnode )
**
** allocate a qnode, and place its pointer into 'node'
**
** returns the allocation status
*/

static status_t _qnode_alloc( qnode_t **qnode ) {

	// validate our parameter (as best we can)

	if( qnode == NULL ) {
		return( E_BAD_PARAM );
	}

	// verify that there is an available qnode

	if( _qnode_avail == NULL ) {
		return( E_NO_QNODES );
	}

	// remove the qnode and return it via the parameter

	*qnode = _qnode_avail;
	_qnode_avail = _qnode_avail->next;

	return( E_SUCCESS );
}

/*
** status = _qnode_free( qnode )
**
** return 'node' to the list of free qnodes
**
** returns the deallocation status
*/

static status_t _qnode_free( qnode_t *qnode ) {

	if( qnode == NULL ) {
		return( E_BAD_PARAM );
	}

	// add the qnode to the beginning of the free list

	qnode->next = _qnode_avail;
	_qnode_avail = qnode;

	return( E_SUCCESS );

}

/*
** PUBLIC FUNCTIONS
*/

/*
** _q_init()
**
** initialize the queue module
*/

void _q_init( void ) {
	int i;

	// add all the qnodes to the free qnode list

	_qnode_avail = NULL;
	for( i = 0 ; i < N_QNODES; ++i ) {
		_qnodes[i].next = _qnode_avail;
		_qnode_avail = &_qnodes[i];
	}

	c_puts( " queues" );
}

/*
** _q_dump(which,queue)
**
** dump the contents of the specified queue to the console
*/

void _q_dump( char *which, queue_t *queue ) {
	qnode_t *tmp;


	c_printf( "%s: ", which );
	if( queue == NULL ) {
		c_puts( "NULL???" );
		return;
	}

	c_printf( "head %08x tail %08x len %d comp %08x rem %08x\n",
		  queue->head, queue->tail, queue->length,
		  queue->compare, queue->remove );
	
	if( queue->length ) {
		c_puts( " i/s/p: " );
		for( tmp = queue->head; tmp != NULL; tmp = tmp->next ) {
			c_printf( " [%d/%d/0x%x]",
				((pcb_t *)(tmp->data))->pid,
				((pcb_t *)(tmp->data))->state,
				((pcb_t *)(tmp->data))->prio );
		}
		c_puts( "\n" );
	}

}
		  

/*
** _q_reset( queue, compare )
**
** reset 'queue' to its empty state, with 'compare' as its ordering routine
*/

void _q_reset( queue_t *queue, q_compare_t compare, q_remove_t remove ) {

	// sanity check

	if( queue == NULL ) {
		_kpanic( "_q_reset", "null queue", 0 );
	}

	// fill in all the fields

	queue->compare = compare;
	queue->remove = remove;
	queue->length = 0;
	queue->head = NULL;
	queue->tail = NULL;

}

/*
** status = _q_remove( queue, data )
**
** remove the first element from 'queue'; place it into 'data' if
** an element was found
**
** returns the status of the removal attempt
*/

status_t _q_remove( queue_t *queue, void **data ) {
	qnode_t *qnode;

	// sanity check

	if( queue == NULL || data == NULL ) {
		return( E_BAD_PARAM );
	}

	// if no nodes in the queue, we're done

	if( _q_empty(queue) ) {
		return( E_EMPTY );
	}

	// remove the first node from the queue

	qnode = queue->head;
	queue->length -= 1;

	queue->head = qnode->next;

	if( queue->head == NULL ) {
		queue->tail = NULL;
	} else {
		queue->head->prev = NULL;
	}

	// return the data field to the caller

	*data = qnode->data;

	// propogate the qnode deallocation status

	return( _qnode_free( qnode ) );

}

/*
** status = _q_remove_by_key( queue, data, key )
**
** locate an element in 'queue' identified by 'key' by using the
** queue's built-in removal routine; place it into 'data' if one
** is found
**
** returns the status of the removal attempt
*/

status_t _q_remove_by_key( queue_t *queue, void **data, key_t key ) {
	qnode_t *qnode;
	status_t stat;

	// verify that our parameters are usable

	if( queue == NULL || data == NULL ) {
		return( E_BAD_PARAM );
	}

	// if the queue is empty, nothing to do

	if( _q_empty(queue) ) {
		return( E_EMPTY );
	}

	// if there is no specialized removal routine, do an ordinary remove

	if( queue->remove == NULL ) {
		return( _q_remove(queue,data) );
	}

	//
	// Traverse the queue, looking for the correct element.
	// Use the built-in removal routine to determine when
	// we have found the correct entry.
	//

	qnode = queue->head;
	while( qnode != NULL && !queue->remove(qnode->key,key) ) {
		qnode = qnode->next;
	}

	// did we find the one we wanted?

	if( qnode == NULL ) {
		return( E_NOT_FOUND );
	}

	// found the correct node - unlink it from the queue

	if( qnode->prev != NULL ) {
		qnode->prev->next = qnode->next;
	} else {
		queue->head = qnode->next;
	}

	if( qnode->next != NULL ) {
		qnode->next->prev = qnode->prev;
	} else {
		queue->tail = qnode->prev;
	}

	queue->length -= 1;

	// return the data from the node

	*data = qnode->data;

	// release the qnode
	//
	// INCONSISTENCY:  _q_remove() just propogates the status

	stat = _qnode_free( qnode );
	if( stat != E_SUCCESS ) {
		_kpanic( "_q_remove_by_key", "qnode free failed, %s", stat );
	}

	return( E_SUCCESS );

}


/*
** status = _q_append( queue, data, key )
**
** adds 'data' to the end of 'queue'; 'key' is copied into the qnode,
** but is otherwise ignored
**
** returns a status value indicating the success or failure of the attempt
*/

status_t _q_append( queue_t *queue, void *data, key_t key ) {
	qnode_t *qnode;
	status_t stat;

	// sanity check

	if( queue == NULL ) {
		// INCONSISTENCY
		_kpanic( "_q_append", "null queue", 0 );
	}

	// first, get a qnode structure

	stat = _qnode_alloc( &qnode );
	if( stat != E_SUCCESS ) {
		return( stat );
	}

	// fill in the necessary information

	qnode->key = key;
	qnode->data = data;

	// add the node to the end of the queue

	if( _q_empty(queue) ) {
		queue->head = qnode;
	} else {
		queue->tail->next = qnode;
	}

	qnode->next = NULL;
	qnode->prev = queue->tail;
	queue->tail = qnode;

	queue->length += 1;

	return( E_SUCCESS );

}

/*
** status = _q_insert( queue, data, key )
**
** inserts 'data' into 'queue', with position determined by 'key'
**
** returns a status value indicating the success or failure of the attempt
*/

status_t _q_insert( queue_t *queue, void *data, key_t key ) {
	qnode_t *qnode;
	qnode_t *curr;
	status_t stat;

	// sanity check

	if( queue == NULL ) {
		return( E_BAD_PARAM );
	}

	// allocate a queue node

	stat = _qnode_alloc( &qnode );
	if( stat != E_SUCCESS ) {
		return( stat );
	}

	// fill in the necessary information

	qnode->key = key;
	qnode->data = data;
	qnode->prev = NULL;
	qnode->next = NULL;

	// if the queue is empty, no need to find the "right" place

	if( _q_empty(queue) ) {
		queue->head = qnode;
		queue->tail = qnode;
		queue->length = 1;
		return( E_SUCCESS );
	}

	// no comparison routine means append

	if( queue->compare == NULL ) {
		queue->tail->next = qnode;
		qnode->prev = queue->tail;
		queue->tail = qnode;
		queue->length += 1;
		return( E_SUCCESS );
	}

	//
	// traverse the queue looking for the correct insertion point,
	// as indicated by the built-in comparison routine
	//

	curr = queue->head;
	while( curr != NULL && queue->compare(curr,qnode) <= 0 ) {
		curr = curr->next;
	}

	// add at the end of the queue

	if( curr == NULL ) {
		queue->tail->next = qnode;
		qnode->prev = queue->tail;
		queue->tail = qnode;
		queue->length += 1;
		return( E_SUCCESS );
	}

	// insert in the middle of the queue

	qnode->next = curr;
	if( curr->prev == NULL ) {
		queue->head = qnode;
	} else {
		curr->prev->next = qnode;
		qnode->prev = curr->prev;
	}
	curr->prev = qnode;

	queue->length += 1;

	return( E_SUCCESS );

}

/*
** n = _compare_uint_asc( n1, n2 );
**
** compares the two supplied qnodes
**
** returns -1 if n1 < n2, 0 if they are equal, and +1 if n1 > n2
*/

int _compare_uint_asc( qnode_t *old, qnode_t *new ) {

	if( old->key.u < new->key.u ) {
		return( -1 );
	}

	if( old->key.u == new->key.u ) {
		return( 0 );
	}

	return( 1 );

}

/*
** n = _remove_if_equal( key_t k1, key_t k2 );
**
** compares the two supplied keys
**
** returns 1 if n1 = n2, 0 if n1 != n2
*/

int _remove_if_equal( key_t k1, key_t k2 ) {

	return( k1.u == k2.u );

}

/*
** n = _remove_if_le_u( key_t k1, key_t k2 );
**
** compares the two supplied keys
**
** returns 1 if n1 <= n2, 0 if n1 > n2
*/

int _remove_if_le_u( key_t k1, key_t k2 ) {

	return( k1.u <= k2.u );

}
