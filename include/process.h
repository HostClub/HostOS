/*
** SCCS ID:	@(#)process.h	1.1	03/31/11
**
** File:	process.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Process-related definitions
*/

#ifndef _PROCESS_H
#define _PROCESS_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// process states

#define	FREE		0
#define	NEW		1
#define	READY		2
#define	RUNNING		3
#define	SLEEPING	4
#define	BLOCKED		5
#define	WAITING		6
#define	KILLED		8
#define	ZOMBIE		9

#define N_STATES	10

// process priorities

#define	PRIO_MAXIMUM	0
#define	PRIO_STANDARD	1
#define	PRIO_LOW	2
#define	PRIO_MINIMUM	3

#define N_PRIORITIES	4

// PID of the initial user process

#define	PID_INIT	1

#ifndef __ASM__20103__

#include "clock.h"
#include "queue.h"
#include "stack.h"

/*
** Start of C-only definitions
*/

/*
** Types
*/

// process context structure
//
// NOTE:  the order of data members here depends on the
// register save code in isr_stubs.S!!!!

typedef struct context {
	uint32_t ss;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t vector;
	uint32_t code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} context_t;

// process id

typedef uint32_t	pid_t;

// process control block
//
// members are ordered by size

typedef struct pcb {
	context_t	*context;	// ptr to process context
	stack_t		*stack;		// ptr to full stack
	pid_t		pid;		// who we are
	pid_t		ppid;		// who created us
	int32_t		status;		// termination status of this process
	time_t		sleeptime;	// when the process started sleeping
	int8_t		state;		// current process state
	uint8_t		prio;		// current process priority
} pcb_t;

/*
** Globals
*/

extern pcb_t _pcbs[];		// all PCBs in the system
extern pcb_t *_current;		// the currently executing process
extern pid_t _next_pid;		// next available PID

// process queues

extern queue_t _sleep;		// processes who called sleep()
extern queue_t _reading;	// processes waiting for input
extern queue_t _waiting;	// processes waiting for children
extern queue_t _zombie;		// terminated processes

/*
** Prototypes
*/

/*
** _pcb_alloc(pcb)
**
** allocate a PCB structure
**
** returns a pointer to the PCB, or NULL on failure
*/

status_t _pcb_alloc( pcb_t **pcb );

/*
** _pcb_free(pcb)
**
** deallocate a PCB structure
**
** returns the status from the _q_insert() call
*/

status_t _pcb_free( pcb_t *pcb );

/*
** _pcb_find(pid)
**
** locate the PCB for the specified PID
**
** returns a pointer to the PCB, or NULL
*/

pcb_t *_pcb_find( pid_t pid );

/*
** _pcb_init()
**
** initializes all process-related data structures
*/

void _pcb_init( void );

#endif

#endif
