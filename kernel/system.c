/*
** SCCS ID:	@(#)system.c	1.1	03/31/11
**
** File:	system.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Miscellaneous OS support functions
*/

#define	__KERNEL__20103__

#include "headers.h"

#include "paging.h"
#include "system.h"
#include "clock.h"
#include "process.h"
#include "bootstrap.h"
#include "syscall.h"
#include "sio.h"
#include "scheduler.h"
#include "bsp.h"

// need init() address
#include "user.h"

// need the exit() prototype
#include "ulib.h"

#include "pci.h"


/*
** PUBLIC FUNCTIONS
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

context_t *_setup_stack( stack_t *stack, uint32_t entry ) {
	uint32_t *tmp;
	context_t *context;

	/*
	** Set up the initial stack contents for a (new) user process.
	**
	** We reserve two longwords at the bottom of the stack as
	** scratch space.  Above that, we simulate a call to exit() by
	** inserting an exit status value and a dummy return address; we
	** then simulate a call from exit() to the user main routine by
	** pushing the address of exit() as a "return address".  Finally,
	** above that we place an context_t area that is initialized with
	** the standard initial register contents.
	**
	** The low end of the stack will contain these values:
	**
	**	esp ->	?	<- context save area
	**		...	<- context save area
	**		?	<- context save area
	**		exit	<- return address for main()
	**		0	<- dummy return address
	**		status	<- code for exit() call
	**		filler
	**		filler  <- last word in stack
	**
	** When this process is dispatched, the context restore
	** code will pop all the saved context information off
	** the stack, leaving the 
	*/

	/*
	** First, compute a pointer to the 3rd-to-last longword
	** (where the exit code will go).
	*/

	tmp = (uint32_t *)(stack + 1) - 3;

	// Next, fill in the dummy return information

	*tmp-- = X_SUCCESS;		// exit() parameter
	*tmp-- = 0;			// dummy return address
	*tmp   = (uint32_t) exit;	// "return" into exit()

	/*
	** Now, fill in the "saved" context information.  Begin
	** by computing a pointer to the context save area.
	*/

	context = (context_t *)tmp - 1;

	/*
	** Clear the context area to ensure that the "saved"
	** general registers all contain 0.
	*/

	_memclr( (void *) context, sizeof(context_t) );

	/*
	** Initialize all the registers that should be non-zero.
	** First, the segment registers.
	*/

	context->cs  = GDT_CODE;
	context->ss  = GDT_STACK;
	context->ds  = GDT_DATA;
	context->es  = GDT_DATA;
	context->fs  = GDT_DATA;
	context->gs  = GDT_DATA;

	/*
	** EIP must contain the entry point of the user routine;
	** in essence, we're pretending that this is where we
	** were executing when the interrupt arrived.
	*/

	context->eip = entry;

	// EFLAGS must be properly initialized

	context->eflags = DEFAULT_EFLAGS;

	// return the pointer to the new context

	return( context );

}

/*
** _zombify - possibly create a "walking dead" process
**
** Called by _dispatch() to deal with a KILLED process, and by
** _sys_exit() to deal with an exiting process.
*/

void _zombify( pcb_t *pcb ) {
	pcb_t *parent;
	status_t stat;
	pid_t pid;
	key_t key;
	info_t *info;

	// sanity check

	if( pcb == NULL ) {
		_kpanic( "_zombify", "null pcb", 0 );
	}

	// Locate the parent of this process

	parent = _pcb_find( pcb->ppid );

	if( parent == NULL ) {
		c_printf( "** zombify(): pid %d ppid %d\n",
			  pcb->pid, pcb->ppid );
		_kpanic( "_zombify", "no process parent", 0 );
	}

	//
	// Found the parent.  If it's waiting for this process,
	// wake it up, give it this process' status, and clean up.
	//

	if( parent->state == WAITING ) {

		// get the address of the info structure from the
		// parent, and pull out the desired PID

		info = (info_t *) ARG(parent->context,1);
		pid = info->pid;

		// if the parent was waiting for any of its children
		// or was waiting for us specifically, give it our
		// information and terminate this process.
		//
		// if the parent was waiting for another child,
		// turn this process into a zombie.

		if( pid == 0 || pid == _current->pid ) {

			// pull the parent off the waiting queue

			key.u = parent->pid;
			stat = _q_remove_by_key(&_waiting,(void **)&parent,key);
			if( stat != E_SUCCESS ) {
				_kpanic( "_zombify", "wait remove status %s",
					 stat );
			}

			// return our PID and our termination status
			// to the parent

			info->pid = _current->pid;
			info->status = ARG(_current->context,1);

			// clean up this process

			stat = _stack_free( pcb->stack );
			if( stat != E_SUCCESS ) {
				_kpanic( "_zombify", "stack free status %s",
					 stat );
			}

			stat = _pcb_free( pcb );
			if( stat != E_SUCCESS ) {
				_kpanic( "_zombify", "pcb free status %s",
					 stat );
			}

			// schedule the parent; give it a quick dispatch

			_schedule( parent, PRIO_MAXIMUM );

			return;
		}

	}

	//
	// Our parent either wasn't waiting, or was waiting for someone
	// else.  Put this process on the zombie queue until our parent
	// wants us.
	//

	key.u = _current->pid;
	_current->state = ZOMBIE;

	stat = _q_insert( &_zombie, (void *)_current, key );
	if( stat != E_SUCCESS ) {
		_kpanic( "_zombify", "zombie insert status %s", stat );
	}

}


void _ap_init( void ) {
	//*((int *)0x9000) = 11;
	//c_io_init();
	c_puts( "AP INIT" );

	while (1);
}

/*
** _init - system initialization routine
**
** Called by the startup code immediately before returning into the
** first user process.
*/

void _init( void ) {
	pcb_t *pcb;
	context_t *context;
	status_t stat;

	/*
	** BOILERPLATE CODE - taken from basic framework
	**
	** Initialize interrupt stuff.
	*/

	__init_interrupts();	// IDT and PIC initialization

	/*
	** Console I/O system.
	*/

	c_io_init();
	c_setscroll( 0, 7, 99, 99 );
	c_puts_at( 0, 6, "================================================================================" );

	/*
	** 20103-SPECIFIC CODE STARTS HERE
	*/

	/*
	** Initialize various OS modules
	*/

	c_puts( "Starting module init: " );

	_q_init();		// must be first
	_pcb_init();
	_stack_init();
	_sio_init();
	_syscall_init();
	_sched_init();
	_clock_init();
	_pci_init();
	_paging_init();

	c_puts( "\n" );

	c_puts(" Ending init\n");
	initSMP();

	//__panic("SSSSSSSSHHHHIIIIIIIIITTT");
	/*
	** Create the initial system ESP
	**
	** This will be the address of the next-to-last
	** longword in the system stack.
	*/

	_system_esp = ((uint32_t *) ( (&_system_stack) + 1)) - 2;

	/*
	** Install the ISRs
	*/

	__install_isr( INT_VEC_TIMER, _isr_clock );
	__install_isr( INT_VEC_SYSCALL, _isr_syscall );
	__install_isr( INT_VEC_SERIAL_PORT_1, _isr_sio );

	/*
	** Create the initial process
	**
	** Code mostly stolen from _sys_fork() and _sys_exec();
	** if either of those routines change, SO MUST THIS!!!
	**
	** First, get a PCB and a stack
	*/

	stat = _pcb_alloc( &pcb );
	if( stat != E_SUCCESS ) {
		_kpanic( "_init", "first pcb alloc status %s", stat );
	}

	stat = _stack_alloc( &(pcb->stack) );
	if( stat != E_SUCCESS ) {
		_kpanic( "_init", "first stack alloc status %s", stat );
	}

	/*
	** Next, set up various PCB fields
	*/

	pcb->pid  = PID_INIT;
	pcb->ppid = PID_INIT;
	pcb->prio = PRIO_MAXIMUM;

	/*
	** Set up the initial process context.
	*/

	context = _setup_stack( pcb->stack, (uint32_t) init );

	// Finally, set up the process' ESP

	context->esp = (uint32_t) context;

	// Make it the "current" process

	_current = pcb;
	_current->context = context;

	/*
	** Starting up the idle routine is the responsibility
	** of the initial user process.
	*/

	/*
	** Turn on the SIO receiver (the transmitter will be turned
	** on/off as characters are being sent)
	*/

	_sio_enable( SIO_RX );

	/*
	** END OF 20103-SPECIFIC CODE
	**
	** Finally, report that we're all done.
	*/

	c_puts( "System initialization complete.\n" );

}
