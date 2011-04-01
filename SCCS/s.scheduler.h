h08026
s 00077/00000/00000
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
E 1
