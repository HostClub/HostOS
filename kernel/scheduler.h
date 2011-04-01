/*
** SCCS ID:	@(#)scheduler.h	1.1	03/31/11
**
** File:	scheduler.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Scheduler module
*/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "headers.h"

#include "process.h"
#include "queue.h"

/*
** General (C and/or assembly) definitions
*/

#ifndef __ASM__20103__

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
** Globals
*/

// the ready queue

extern queue_t _ready[];

// the current process

extern pcb_t *_current;

/*
** Prototypes
*/

/*
** _schedule( pcb, priority )
**
** put PCB into the ready queue with the specified priority
*/

void _schedule( pcb_t *pcb, uint8_t prio );

/*
** _dispatch( void )
**
** dispatch the next process
*/

void _dispatch( void );

/*
** _sched_init( void )
**
** initialize the scheduler module
*/

void _sched_init( void );

#endif

#endif
