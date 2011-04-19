/*
** SCCS ID:	@(#)clock.c	1.1	03/31/11
**
** File:	clock.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Clock-related routines
*/

#define	__KERNEL__20103__

#include "headers.h"

#include "clock.h"
#include "process.h"
#include "queue.h"
#include "scheduler.h"
#include "sio.h"
#include "startup.h"

#include <x86arch.h>

/*
** PRIVATE GLOBAL VARIABLES
*/

// pinwheel control variables

static uint32_t _pinwheel;
static uint32_t _pindex;

/*
** PUBLIC GLOBAL VARIABLES
*/

// current system time

time_t _system_time;

/*
** PUBLIC FUNCTIONS
*/

/*
** _isr_clock - clock ISR
*/

void _isr_clock( int vector, int code ) {
	pcb_t *pcb;
	status_t stat;
	key_t key;

	// increment system time

	++_system_time;

	// spin the pinwheel

	++_pinwheel;
	if( _pinwheel == (CLOCK_FREQUENCY / 10) ) {
		_pinwheel = 0;
		++_pindex;
		c_putchar_at( 79, 0, "|/-\\"[ _pindex & 3 ] );
	}

	// wake up any sleepers whose time has come
	//
	// we give awakened processes a priority boost for their
	// first dispatch after being awakened

	key.u = _system_time;
	stat = _q_remove_by_key( &_sleep, (void *) &pcb, key );
	while( stat == E_SUCCESS ) {
		_schedule( pcb, PRIO_MAXIMUM );
		stat = _q_remove_by_key( &_sleep, (void *) &pcb, key );
	}

	// no explicit quantum - we preempt after each clock tick

	_schedule( _current, _current->prio );

	/*
	** Approximately every 20 seconds, dump the queues,
	** and print the contents of the SIO buffers
	*/

	/*if( (_system_time & CLK_SEC_20) == 0 ) {
		c_printf( "Queue contents @%08x\n", _system_time );
		_q_dump( "ready[0]", &_ready[0] );
		_q_dump( "ready[1]", &_ready[1] );
		_q_dump( "ready[2]", &_ready[2] );
		_q_dump( "ready[3]", &_ready[3] );
		_q_dump( "sleep   ", &_sleep );
		_q_dump( "reading ", &_reading );
		_q_dump( "waiting ", &_waiting );
		_q_dump( "zombie  ", &_zombie );
		_sio_dump();
	}*/

	// select a new current process

	_dispatch();

	// tell the PIC that we're done

	__outb( PIC_MASTER_CMD_PORT, PIC_EOI );

}

/*
** _clock_init()
**
** initialize all clock-related variables
*/

void _clock_init( void ) {
	uint32_t divisor;

	// start the pinwheel

	_pinwheel = _pindex = 0;

	// return the system to the dawn of time

	_system_time = 0;

	// set the clock to tick at CLOCK_FREQUENCY Hz.

	divisor = TIMER_FREQUENCY / CLOCK_FREQUENCY;
	__outb( TIMER_CONTROL_PORT, TIMER_0_LOAD | TIMER_0_SQUARE );
	__outb( TIMER_0_PORT, divisor & 0xff );        // LSB of divisor
	__outb( TIMER_0_PORT, (divisor >> 8) & 0xff ); // MSB of divisor

	// announce that we have initialized the clock module

	c_puts( " clock" );

}
