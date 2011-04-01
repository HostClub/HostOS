/*
** SCCS ID:	@(#)syscall.c	1.1	03/31/11
**
** File:	syscall.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	System call handlers
*/

#define	__KERNEL__20103__

#include "headers.h"

#include "syscall.h"
#include "process.h"
#include "scheduler.h"
#include "sio.h"
#include "system.h"
#include "startup.h"

// need the info_t structure
#include "ulib.h"

#include <x86arch.h>

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

// system call prototype 

typedef void(*syscall_t)(context_t *);

/*
** PRIVATE GLOBAL VARIABLES
*/

// system call jump table

static syscall_t _syscalls[N_SYSCALLS];

/*
** PRIVATE FUNCTIONS
*/

/*
** Second-level syscall handlers
**
** All have this prototype:
**
**      static void _sys_NAME( context_t *context );
*/

/*
** _sys_getpid - retrieve PID
**
** returns:  PID from the currently-executing process
*/

static void _sys_getpid( context_t *context ) {

	context->eax = _current->pid;

}

/*
** _sys_getppid - retrieve PPID
**
** returns:  PPID from the currently-executing process
*/

static void _sys_getppid( context_t *context ) {

	context->eax = _current->ppid;

}

/*
** _sys_getprio - retrieve process priority
**
** returns:  priority value for the currently-executing process
*/

static void _sys_getprio( context_t *context ) {

	context->eax = _current->prio;

}

/*
** _sys_gettime - retrieve system time
**
** returns:  current system time (in ticks)
*/

static void _sys_gettime( context_t *context ) {

	context->eax = _system_time;

}

/*
** _sys_setprio - alter the priority of the current process
**
** returns:  the old priority value
*/

static void _sys_setprio( context_t *context ) {

	context->eax = _current->prio;
	_current->prio = ARG(context,1);

}

/*
** _sys_settime - change the system time
**
** returns:  the old system time
*/

static void _sys_settime( context_t *context ) {

	context->eax = _system_time;
	_system_time = ARG(context,1);

}

/*
** _sys_sleep - put the process to sleep for a specified number of ticks
*/

static void _sys_sleep( context_t *context ) {
	uint_t ticks;
	key_t key;

	// calculate wakeup time

	ticks = ARG(context,1);
	key.u = _system_time + ticks;

	// remember when the process went to sleep

	_current->sleeptime = _system_time;

	// mark the process as sleeping

	_current->state = SLEEPING;

	// add to the sleep queue (ordered by wakeup time)

	if( _q_insert(&_sleep,(void *)_current,key) != E_SUCCESS ) {
		context->eax = 0;
		return;
	}

	// pick a new current process

	_dispatch();

}

/*
** _sys_fork - create a new process
**
** returns:  -1 on error, 0 in child, child PID in parent
*/

static void _sys_fork( context_t *context ) {
	pcb_t *pcb;
	status_t stat;
	stack_t *stack;
	int offset;

	/*
	** Start by allocating the necessary process data structures.
	** We'll need a PCB and a stack.  If either allocation fails,
	** inform the original process.
	*/

	if( _pcb_alloc( &pcb ) != E_SUCCESS ) {
		context->eax = -1;
		return;
	}

	if( _stack_alloc( &stack ) != E_SUCCESS ) {
		stat = _pcb_free( pcb );
		if( stat != E_SUCCESS ) {
			_kpanic( "_sys_fork", "free pcb status %s", stat );
		}
		context->eax = -1;
		return;
	}

	// Next, set up the child's PCB.

	pcb->pid = _next_pid++;
	pcb->ppid = _current->pid;
	pcb->prio = _current->prio;
	pcb->stack = stack;

	// Duplicate the parent's stack for the child.

	_memcpy( stack, _current->stack, sizeof( stack_t ) );

	/*
	** We duplicated the parent's stack contents, which means that
	** the child's ESP and EBP are still pointing into the parent's
	** stack.  Fix these, and also fix the child's context pointer.
	**
	** First, determine the distance (in bytes) between the two
	** stacks.  This is the adjustment value we must add to the
	** three pointers to correct them.
	*/

	offset = (void *) pcb->stack - (void *) _current->stack;

	// Next, update the pointers.

	pcb->context = (context_t *) ((void *)_current->context + offset);
	pcb->context->esp += offset;
	pcb->context->ebp += offset;

	/*
	** IMPORTANT NOTE
	**
	** We have to change EBP because that's how the compiled code for
	** the user process accesses its local variables.  If we didn't
	** change this, as soon as the child was dispatched, it would
	** start to stomp on the local variables in the parent's stack.
	**
	** The child's ESP and EBP will be set by the context restore when
	** we dispatch it.  However, we don't fix the EBP chain, so if the
	** child doesn't immediately exec() but instead returns, it will
	** switch back over to the parent's stack.
	**
	** None of this would be an issue if we were doing "real" virtual
	** memory, as we would be talking about virtual addresses here rather
	** than physical addresses, and all processes would share the same
	** virtual address space layout.
	*/

	// Set up return values for the parent and child

	pcb->context->eax = 0;
	context->eax = pcb->pid;

	/*
	** Philosophical issue:  should the child run immediately, or
	** should the parent continue?
	**
	** We take the path of least resistance (work), and opt for the
	** latter; we schedule the child, then return into the parent.
	*/

	_schedule( pcb, pcb->prio );

}

/*
** _sys_exec - replace a process with a different program
*/

static void _sys_exec( context_t *context ) {
	uint32_t entry;
	uint32_t prio;

	/*
	** Get the entry point and desired priority from the
	** process stack.  (We do this now so that we can
	** reinitialize the stack in place.)
	*/

	prio = ARG(context,1);
	entry = ARG(context,2);

	/*
	** Next, reset the stack contents.
	*/

	_current->context = _setup_stack( _current->stack, entry );

	/*
	** Set the priority value for this process.
	*/

	_current->prio = prio;

	/*
	** Philosophical question:  let the process continue executing,
	** or schedule it and dispatch another process?
	**
	** We opt for the latter.
	*/

	_schedule( _current, prio );
	_dispatch();
}

/*
** _sys_readc - read one character from the serial i/o
**
** returns:  the character
*/

static void _sys_readc( context_t *context ) {
	key_t key;
	int ch;

	// try to get the next character

	ch = _sio_readc();

	// if there was a character, return it to the process;
	// otherwise, block the process until one comes in

	if( ch >= 0 ) {

		context->eax = ch;
	
	} else {

		// no character; put this process on the
		// serial i/o input queue

		_current->status = BLOCKED;

		key.u = _current->pid;
		ch = _q_insert( &_reading, (void *) _current, key );
		if( ch != E_SUCCESS ) {
			_kpanic( "_sys_read", "insert status %s", ch );
		}

		// select a new current process

		_dispatch();
	
	}

}

/*
** _sys_writec - write a character to the SIO
**
** returns:  status of the write
*/

static void _sys_writec( context_t *context ) {
	int ch = ARG(context,1);

	// this is almost insanely simple, but it does separate
	// the low-level device access fromm the higher-level
	// syscall implementation

	_sio_writec( ch );

	_current->context->eax = E_SUCCESS;

}

/*
** _sys_reads - read a string from the serial i/o into a user buffer
**
** returns:  number of characters returned in the user buffer
*/

static void _sys_reads( context_t *context ) {
	char *buffer;
	int length;

	buffer = (char *) ARG(context,1);
	length = ARG(context,2);

	context->eax = E_UNIMPLEMENTED;

}

/*
** _sys_writes - write a string to the serial i/o
**
** returns:  the status of the write
*/

static void _sys_writes( context_t *context ) {
	char *buffer;
	int length;

	buffer = (char *) ARG(context,1);
	length = ARG(context,2);

	context->eax = E_UNIMPLEMENTED;
}

/*
** _sys_kill - terminate a process with extreme prejudice
**
** returns:  termination status
*/

static void _sys_kill( context_t *context ) {
	pid_t pid;
	pcb_t *pcb;

	// PID we're looking for

	pid = ARG(context,1);

	//
	// Traverse the PCB table looking for this PID
	//

	pcb = _pcb_find( pid );

	// did we find it?

	if( pcb == NULL ) {
		context->eax = E_NOT_FOUND;
		return;
	}

	// yes - mark it as killed (_dispatch() will clean it up)

	pcb->state = KILLED;

	// if it was us, we give up the CPU

	if( pcb == _current ) {
		_schedule( pcb, pcb->prio );
		_dispatch();
	}

}

/*
** _sys_wait - block until a child process terminates
**
** returns:  termination information about the child
*/

static void _sys_wait( context_t *context ) {
	pcb_t *pcb, *found;
	status_t stat;
	info_t *info;
	key_t key;
	int i;

	// sanity check

	info = (info_t *)ARG(context,1);
	if( info == NULL ) {
		context->eax = E_BAD_PARAM;
		return;
	}

	//
	// The info structure's PID field indicates what the
	// calling process wants to wait for.  If the PID is
	// zero, wait for any child process; otherwise, wait
	// for the specific child having that PID.
	//

	if( info->pid > 0 ) {

		//
		// Waiting for a specific child.  Begin by
		// finding that child in the system.
		//

		pcb = _pcb_find( info->pid );

		// if not found, return an error

		if( pcb == NULL ) {
			info->status = E_NOT_FOUND;
			return;
		}

	} else {

		//
		// Waiting for any child.  This is a bit more complicated;
		// we need to ensure that there is a child, but the child
		// may be a ZOMBIE or may still be active.  If the former,
		// we want to clean it up immediately; if the latter, we
		// need to block the caller until a child terminates.
		//

		found = NULL;

		// Look for a child process.

		for( i = 0; i < N_PCBS; ++i ) {
			pcb = &_pcbs[i];

			// if we find a child, remember it
			if( pcb->ppid == _current->pid ) {
				found = pcb;
				// if it is a zombie, stop looking
				if( pcb->state == ZOMBIE ) {
					break;
				}
			}
		}

		//
		// Whew!
		//
		// At this point, if we found a child that is a zombie,
		// found and pcb point to the zombie, and i is its index.
		//
		// If we didn't find a zombie but did find a child, found
		// points to it, and i > N_PCBS.
		//
		// If found is null and i > N_PCBS, we did not find a
		// child of the current process.
		//

		if( i > N_PCBS ) {

			// no zombie child - did we find any child?

			if( found == NULL ) {
				// no!
				info->status = E_NOT_FOUND;
				return;
			}

			// yes - it's not a zombie, so we will need to
			// block the calling process

			pcb = found;
		}

	}

	//
	// We found a child process; if it is a zombie, deal with it
	// immediately.  Otherwise, we need to block the caller until
	// this (or another) child terminates.
	//

	if( pcb->state == ZOMBIE ) {

		// remember the PID and termination status

		info->pid = pcb->pid;
		info->status = pcb->status;

		// clean up the zombie - first, remove it from the queue

		key.u = pcb->pid;
		stat = _q_remove_by_key( &_zombie, (void **)&pcb, key );
		if( stat != E_SUCCESS ) {
			_kpanic( "_sys_wait", "zombie not on queue, %s", stat );
		}

		// release the child's stack and pcb structures

		stat = _stack_free( pcb->stack );
		if( stat != E_SUCCESS ) {
			_kpanic( "_sys_wait", "stack free status %s", stat );
		}

		stat = _pcb_free( pcb );
		if( stat != E_SUCCESS ) {
			_kpanic( "_sys_wait", "pcb free status %s", stat );
		}

		// resume execution in the parent

		return;
	}

	//
	// We found at least one child, but none of our children are
	// zombies (yet).  Block this process until that happens.
	//

	key.u = _current->pid;

	_current->state = WAITING;

	stat = _q_insert( &_waiting, (void *)_current, key );
	if( stat != E_SUCCESS ) {
		_kpanic( "_sys_wait", "waiting insert status %s", stat );
	}

	// need a new current process

	_dispatch();

}

/*
** _sys_exit - terminate this process
*/

static void _sys_exit( context_t *context ) {
	pcb_t *pcb;

	//
	// When a process terminates, we need to reparent all of
	// the process' children.
	//

	for( pcb = &_pcbs[0]; pcb < &_pcbs[N_PCBS]; ++pcb ) {
		if( pcb->ppid == _current->pid ) {
			// this is one of our children; reparent it to init
			pcb->ppid = PID_INIT;
		}
	}

	//
	// Next, see if our parent is waiting for us.  Either give it
	// our status and clean us up, or turn us into a zombie.
	//

	_zombify( _current );

	// choose a new current process

	_dispatch();

}


/*
** PUBLIC FUNCTIONS
*/

/*
** _isr_syscall()
**
** Common handler for all system calls; selects the correct
** second-level routine to invoke based on contents of EAX.
**
** Any value being returned to the user process by the system
** call should be put into the process' EAX save area in the
** process context by the second-level routine for the call.
*/

void _isr_syscall( int vector, int code ) {
	context_t *context;
	int syscode;

	// sanity check -  make sure there *is* a current process

	if( _current == NULL ) {
		_kpanic( "_isr_syscall", "null _current", 0 );
	}

	// also, make sure it has a context!

	if( _current->context == NULL ) {
		_kpanic( "_isr_syscall", "null _current context", 0 );
	}

	// Retrieve the syscall code from the process' context.

	context = _current->context;
	syscode = context->eax;

	// If the code is invalid, force it to exit()

	if( syscode < 0 || syscode >= N_SYSCALLS ) {
		c_printf( "syscall: code %d, proc %d, forced exit()\n",
			  syscode, _current->pid );
		syscode = SYS_exit;
	}

	// Call the handler for this syscall.

	(*_syscalls[syscode])( context );

	// Tell the PIC we're done.

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );

}


/*
** _syscall_init()
**
** initializes all syscall-related data structures
*/

void _syscall_init( void ) {

	/*
	** Set up the syscall jump table (this ensures that we
	** get the pointers in the correct slots in the table).
	*/

	_syscalls[ SYS_exit    ]  =  _sys_exit;
	_syscalls[ SYS_fork    ]  =  _sys_fork;
	_syscalls[ SYS_exec    ]  =  _sys_exec;
	_syscalls[ SYS_wait    ]  =  _sys_wait;
	_syscalls[ SYS_kill    ]  =  _sys_kill;
	_syscalls[ SYS_sleep   ]  =  _sys_sleep;
	_syscalls[ SYS_readc   ]  =  _sys_readc;
	_syscalls[ SYS_reads   ]  =  _sys_reads;
	_syscalls[ SYS_writec  ]  =  _sys_writec;
	_syscalls[ SYS_writes  ]  =  _sys_writes;
	_syscalls[ SYS_getpid  ]  =  _sys_getpid;
	_syscalls[ SYS_getppid ]  =  _sys_getppid;
	_syscalls[ SYS_getprio ]  =  _sys_getprio;
	_syscalls[ SYS_gettime ]  =  _sys_gettime;
	_syscalls[ SYS_setprio ]  =  _sys_setprio;
	_syscalls[ SYS_settime ]  =  _sys_settime;

	/*
	** Report that we've initialized this module.
	*/

	c_puts( " syscalls" );

}
