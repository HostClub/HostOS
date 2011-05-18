/*
** SCCS ID:	@(#)user.c	1.1	03/31/11
**
** File:	user.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	User routines.
*/

#include "headers.h"

#include "user.h"


#include "pci.h"
#include "cakesh.h"

/*
** USER PROCESSES
**
** Each is designed to test some facility of the OS:
**
**	User(s)		Tests/Features
**	=======		===============================================
**	A, B, C		Basic operation
**	D		Spawns Z and exits
**	E, F, G		Sleep for different lengths of time
**	H		Doesn't call exit()
**	J		Tries to spawn 2*N_PCBS copies of Y
**	K		Spawns several copies of X
**	L		Spawns several copies of X, waits for each
**	M		Spawns W three times, reporting PIDs
**	N		Spawns several copies of X, reports times & ppid
**	P		Iterates three times, printing system time
**	Q		Tries to execute a bogus system call (bad code)
**	R		Reading and writing
**	S		Loops forever, sleeping 30 seconds at a time
**	T		Loops, fiddles with priority
**
**	W, X, Y, Z	Print characters (spawned by other processes)
**
** Output from user processes is always alphabetic.  Uppercase 
** characters are "expected" output; lowercase are "erroneous"
** output.
**
** More specific information about each user process can be found in
** the header comment for that function (below).
**
** To spawn a specific user process, uncomment its SPAWN_x
** definition in the user.h header file.
*/

/*
** Prototypes for all one-letter user main routines
*/

void user_a( void ); void user_b( void ); void user_c( void );
void user_d( void ); void user_e( void ); void user_f( void );
void user_g( void ); void user_h( void ); void user_i( void );
void user_j( void ); void user_k( void ); void user_l( void );
void user_m( void ); void user_n( void ); void user_o( void );
void user_p( void ); void user_q( void ); void user_r( void );
void user_s( void ); void user_t( void ); void user_u( void );
void user_v( void ); void user_w( void ); void user_x( void );
void user_y( void ); void user_z( void );

/*
** Prototypes for custom user routines
*/

void user_pci_test( void );

/*
** Users A, B, and C are identical, except for the character they
** print out via writec().  Each prints its ID, then loops 30
** times delaying and printing, before exiting.
*/

void user_a( void ) {
	int i, j;
	// uint32_t *ebp;

	c_puts( "User A running\n" );
	// ebp = (uint32_t *) get_ebp();
	// c_printf( "User A returns to %08x\n", *(ebp + 1) );
	writec( 'A' );
	for( i = 0; i < 30; ++i ) {
	// for( i = 0; 1; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'A' );
	}

	c_puts( "User A exiting\n" );
	exit( X_SUCCESS );

}

void user_b( void ) {
	int i, j;

	c_puts( "User B running\n" );
	writec( 'B' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'B' );
	}

	c_puts( "User B exiting\n" );
	exit( X_SUCCESS );

}

void user_c( void ) {
	int i, j;

	c_puts( "User C running\n" );
	writec( 'C' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'C' );
	}

	c_puts( "User C exiting\n" );
	exit( X_SUCCESS );

	writec( 'c' );	/* shouldn't happen! */

}


/*
** User D spawns user Z.
*/

void user_d( void ) {
	int pid;

	c_puts( "User D running\n" );
	writec( 'D' );
	pid = fork();
	if( pid < 0 ) {
		writec( 'd' );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_z );
		writec( 'z' );
		c_puts( "User D, exec of Z failed\n" );
		exit( X_FAILURE );
	}
	writec( 'D' );

	c_puts( "User D exiting\n" );
	exit( X_SUCCESS );

}


/*
** Users E, F, and G test the sleep facility.
**
** User E sleeps for 10 seconds at a time.
*/

void user_e( void ) {
	int i, pid;

	pid = getpid();
	c_printf( "User E (%d) running\n", pid );
	writec( 'E' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( SECONDS_TO_TICKS(10) );
		writec( 'E' );
	}

	c_puts( "User E exiting\n" );
	exit( X_SUCCESS );

}


/*
** User F sleeps for 5 seconds at a time.
*/

void user_f( void ) {
	int i, pid;

	pid = getpid();
	c_printf( "User F (%d) running\n", pid );
	writec( 'F' );
	for( i = 0; i < 5 ; ++i ) {
		sleep( SECONDS_TO_TICKS(5) );
		writec( 'F' );
	}

	c_puts( "User F exiting\n" );
	exit( X_SUCCESS );

}


/*
** User G sleeps for 15 seconds at a time.
*/

void user_g( void ) {
	int i, pid;

	pid = getpid();
	c_printf( "User G (%d) running\n", pid );
	writec( 'G' );
	for( i = 0; i < 5; ++i ) {
		sleep( SECONDS_TO_TICKS(15) );
		writec( 'G' );
	}

	c_puts( "User G exiting\n" );
	exit( X_SUCCESS );

}


/*
** User H is like A-C except it only loops 5 times and doesn't
** call exit().
*/

void user_h( void ) {
	int i, j;

	c_puts( "User H running\n" );
	writec( 'H' );
	for( i = 0; i < 5; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'H' );
	}

	c_puts( "User H returning without exiting!\n" );

}


/*
** User J tries to spawn 2*N_PCBS copies of user_y.
*/

void user_j( void ) {
	int i, pid;

	c_puts( "User J running\n" );
	writec( 'J' );

	for( i = 0; i < N_PCBS * 2 ; ++i ) {
		pid = fork();
		if( pid < 0 ) {
			writec( 'j' );
		} else if( pid == 0 ) {
			exec( PRIO_STANDARD, user_y );
			writec( 'y' );
			c_puts( "User J, exec of y failed\n" );
		} else {
			writec( 'J' );
		}
	}

	c_puts( "User J exiting\n" );
	exit( X_SUCCESS );

}


/*
** User K prints, goes into a loop which runs three times, and exits.
** In the loop, it does a spawn of user_x, sleeps 30 seconds, and prints.
*/

void user_k( void ) {
	int i, pid;

	c_puts( "User K running\n" );
	writec( 'K' );

	for( i = 0; i < 3 ; ++i ) {
		pid = fork();
		if( pid < 0 ) {
			writec( 'k' );
		} else if( pid == 0 ) {
			exec( PRIO_STANDARD, user_x );
			writec( 'x' );
			c_puts( "User K, exec of x failed\n" );
		} else {
			sleep( SECONDS_TO_TICKS(30) );
			writec( 'K' );
		}
	}

	c_puts( "User K exiting\n" );
	exit( X_SUCCESS );

}


/*
** User L is like user K, except that it waits for each of the
** spawned children.
*/

void user_l( void ) {
	int i, pid[3];
	time_t time;
	info_t info;

	c_puts( "User L running, " );
	time = gettime();
	c_printf( " initial time %d\n", time );
	writec( 'L' );

	for( i = 0; i < 3 ; ++i ) {
		time = gettime();
		pid[i] = fork();
		if( pid[i] < 0 ) {
			writec( 'l' );
		} else if( pid[i] == 0 ) {
			exec( PRIO_STANDARD, user_x );
			writec( 'x' );
			c_printf( "User L, exec failed at time %d\n", time );
		} else {
			c_printf( "User L, exec succeeded at time %d\n", time );
			sleep( SECONDS_TO_TICKS(30) );
			writec( 'L' );
		}
	}

	// check them in last-to-first order
	for( i = 2; i >= 0 ; --i ) {
		c_printf( "User L waiting for child %d\n", pid[i] );
		info.pid = pid[i];
		wait( &info );
		c_printf( "User L child %d status %d\n", pid[i], info.status );
	}

	time = gettime();
	c_printf( "User L exiting at time %d\n", time );
	exit( X_SUCCESS );

}


/*
** User M iterates spawns three copies of user W at low priority,
** reporting their PIDs.
*/

void user_m( void ) {
	int i, pid[3];

	c_puts( "User M running\n" );
	for( i = 0; i < 3; ++i ) {
		pid[i] = fork();
		if( pid[i] < 0 ) {
			writec( 'm' );
		} else if( pid[i] == 0 ) {
			exec( PRIO_LOW, user_w );
			writec( 'w' );
			c_printf( "User M, exec of W (%d) failed\n", pid[i] );
		} else {
			c_printf( "User M spawned W, PID %d\n", pid[i] );
			writec( 'M' );
		}
	}

	c_puts( "User M exiting\n" );
	exit( X_SUCCESS );

}


/*
** User N is like user L, except that it reports its PID when starting,
** and spawns user X instead of W.
*/

void user_n( void ) {
	int i, pid, ppid;
	time_t time;

	pid = getpid();
	ppid = getppid();
	c_printf( "User N (%d) running, parent %d\n", pid, ppid );
	writec( 'N' );

	for( i = 0; i < 3 ; ++i ) {
		time = gettime();
		pid = fork();
		if( pid < 0 ) {
			writec( 'n' );
		} else if( pid == 0 ) {
			exec( PRIO_STANDARD, user_x );
			writec( 'x' );
			c_printf( "User N, exec of X (%d) failed at %d\n",
				  getpid(), time );
		} else {
			c_printf( "User N, exec succeeded at time %d\n", time );
			sleep( SECONDS_TO_TICKS(30) );
			writec( 'N' );
		}
	}

	c_puts( "User N exiting\n" );
	exit( X_SUCCESS );

}


/*
** User P iterates three times.  Each iteration sleeps for two seconds,
** then gets and prints the system time.
*/

void user_p( void ) {
	time_t time;
	int i;

	c_printf( "User P running, " );
	time = gettime();
	c_printf( " time %d\n", time );

	writec( 'P' );

	for( i = 0; i < 3; ++i ) {
		sleep( SECONDS_TO_TICKS(2) );
		time = gettime();
		c_printf( "User P reporting time %d\n", time );
		writec( 'P' );
	}

	c_printf( "User P exiting\n" );
	exit( X_SUCCESS );

}


/*
** User Q does a bogus system call
*/

void user_q( void ) {

	c_puts( "User Q running\n" );
	writec( 'Q' );
	bogus();
	c_puts( "User Q exiting!?!?!\n" );
	exit( X_SUCCESS );

}


/*
** User R loops 3 times reading/writing, then exits.
*/

void user_r( void ) {
	int i;
	int ch = '&';

	c_puts( "User R running\n" );
	for( i = 0; i < 3; ++i ) {
		do {
			writec( 'R' );
			ch = readc();
			if( ch == -1 ) {	/* wait a bit */
				sleep( SECONDS_TO_TICKS(1) );
			}
		} while( ch == -1 );
		writec( ch );
	}

	c_puts( "User R exiting\n" );
	exit( X_SUCCESS );

}


/*
** User S sleeps for 30 seconds at a time, and loops forever.
*/

void user_s( void ) {

	c_puts( "User S running\n" );
	writec( 'S' );
	for(;;) {
		sleep( SECONDS_TO_TICKS(30) );
		writec( 'S' );
	}

	c_puts( "User S exiting!?!?!n" );
	exit( X_SUCCESS );

}


/*
** User T changes its priority back and forth several times
*/

void user_t( void ) {
	int priority, prio2;
	int i, j, pid;

	pid = getpid();
	priority = getprio();
	c_printf( "User T (%d) running, initial prio %d\n",
		  pid, priority );
	
	writec( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'T' );
	}

	priority = setprio( PRIO_MAXIMUM );
	if( priority < 0 ) {
		writec( 't' );
		prt_status( "User T, setprio() %s\n", priority );
		exit( X_FAILURE );
	}
	prio2 = getprio();
	c_printf( "User T, prio was %d now %d\n", priority, prio2 );
	
	writec( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'T' );
	}

	priority = setprio( PRIO_LOW );
	if( priority < 0 ) {
		writec( 't' );
		prt_status( "User T, setprio() %s\n", priority );
		exit( X_FAILURE );
	}
	prio2 = getprio();
	c_printf( "User T, prio was %d now %d\n", priority, prio2 );
	
	writec( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'T' );
	}

	priority = setprio( PRIO_STANDARD );
	if( priority < 0 ) {
		writec( 't' );
		prt_status( "User T, setprio() %s\n", priority );
		exit( X_FAILURE );
	}
	prio2 = getprio();
	c_printf( "User T, prio was %d now %d\n", priority, prio2 );
	
	writec( 'T' );
	for( i = 0; i < 30; ++i ) {
		for( j = 0; j < DELAY_STD; ++j )
			continue;
		writec( 'T' );
	}

	c_puts( "User T exiting\n" );
	exit( X_SUCCESS );

}


/*
** User W prints W characters 20 times, sleeping 3 seconds between.
*/

void user_w( void ) {
	int i;
	int pid;

	c_printf( "User W running, " );
	pid = getpid();
	c_printf( " PID %d\n", pid );
	for( i = 0; i < 20 ; ++i ) {
		writec( 'W' );
		sleep( SECONDS_TO_TICKS(3) );
	}

	c_printf( "User W exiting, PID %d\n", pid );
	exit( X_SUCCESS );

}


/*
** User X prints X characters 20 times.  It is spawned multiple
** times, and prints its PID when started and exiting.
*/

void user_x( void ) {
	int i, j;
	int pid;

	c_puts( "User X running, " );
	pid = getpid();
	c_printf( "PID %d, ", pid );

	for( i = 0; i < 20 ; ++i ) {
		writec( 'X' );
		for( j = 0; j < DELAY_STD; ++j )
			continue;
	}

	c_printf( "User X exiting, PID %d\n", pid );
	exit( X_SUCCESS );

}


/*
** User Y prints Y characters 10 times.
*/

void user_y( void ) {
	int i, j;

	c_puts( "User Y running\n" );
	for( i = 0; i < 10 ; ++i ) {
		writec( 'Y' );
		for( j = 0; j < DELAY_ALT; ++j )
			continue;
		sleep( SECONDS_TO_TICKS(1) );
	}

	c_puts( "User Y exiting\n" );
	exit( X_SUCCESS );

}


/*
** User Z prints Z characters 10 times.
*/

void user_z( void ) {
	int i, j;

	c_puts( "User Z running\n" );
	for( i = 0; i < 10 ; ++i ) {
		writec( 'Z' );
		for( j = 0; j < DELAY_STD; ++j )
			continue;
	}

	c_puts( "User Z exiting\n" );
	exit( X_SUCCESS );

}

/*
** User test processes
*/

void user_pci_test( void ) {

	c_puts( "User PCI_Test running\n" );
	_pci_init();
	c_puts( "User PCI_Test exiting\n" );

}

/*
** SYSTEM PROCESSES
*/


/*
** Idle process
*/

void idle( void ) {
	int i;
	int pid;
	int priority;

	pid = getpid();
	priority = getprio();
	c_printf( "Idle (%d) started, prio %08x\n", pid, priority );

	writec( '.' );

	for(;;) {
		for( i = 0; i < DELAY_LONG; ++i )
			continue;
		writec( '.' );
	}

	c_puts( "+++ Idle done!?!?!\n" );

	exit( X_FAILURE );
}


/*
** Initial process; it starts the other top-level user processes.
*/

void init( void ) {
	pid_t pid;
	info_t info;

	c_puts( "Init started\n" );

	writec( '$' );

	/*
	** Always start the idle process first
	*/

	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() idle\n" );
	} else if( pid == 0 ) {
		exec( PRIO_MINIMUM, idle );
		c_puts( "init: can't exec idle\n" );
		exit( X_FAILURE );
	}
/*
#ifdef SPAWN_A
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user A\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_a );
		c_puts( "init: can't exec user A\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_B
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user B\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_b );
		c_puts( "init: can't exec user B\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_C
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user C\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_c );
		c_puts( "init: can't exec user C\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_D
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user D\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_d );
		c_puts( "init: can't exec user D\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_E
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user E\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_e );
		c_puts( "init: can't exec user E\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_F
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user F\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_f );
		c_puts( "init: can't exec user F\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_G
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user G\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_g );
		c_puts( "init: can't exec user G\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_H
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user H\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_h );
		c_puts( "init: can't exec user H\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_J
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user J\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_j );
		c_puts( "init: can't exec user J\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_K
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user K\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_k );
		c_puts( "init: can't exec user K\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_L
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user L\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_l );
		c_puts( "init: can't exec user L\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_M
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user M\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_m );
		c_puts( "init: can't exec user M\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_N
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user N\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_n );
		c_puts( "init: can't exec user N\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_P
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user P\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_p );
		c_puts( "init: can't exec user P\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_Q
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user Q\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_q );
		c_puts( "init: can't exec user Q\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_R
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user R\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_r );
		c_puts( "init: can't exec user R\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_S
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user S\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_s );
		c_puts( "init: can't exec user S\n" );
		exit( X_FAILURE );
	}
#endif

#ifdef SPAWN_T
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user T\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_t );
		c_puts( "init: can't exec user T\n" );
		exit( X_FAILURE );
	}
#endif
*/

/*
	
	c_puts("Spawning CAKESH\n");
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() CAKESH\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, cakesh );
		c_puts( "init: can't exec CAKESH\n" );
		exit( X_FAILURE );
	}
*/

#ifdef SPAWN_PCI_TEST
	c_puts("Spawning PCI_TEST\n");
	pid = fork();
	if( pid < 0 ) {
		c_puts( "init: can't fork() user PCI_TEST\n" );
	} else if( pid == 0 ) {
		exec( PRIO_STANDARD, user_pci_test );
		c_puts( "init: can't exec user PCI_TEST\n" );
		exit( X_FAILURE );
	}
#endif



	writec( '!' );

	/*
	** At this point, we go into an infinite loop
	** waiting for our children (direct, or inherited)
	** to exit.
	*/

	for(;;) {
		info.pid = 0;	// wait for anyone
		wait( &info );
		c_printf( "Init: process %d exited, status %d\n",
			  info.pid, info.status );
		// sleep( SECONDS_TO_TICKS(5) );
	}

	/*
	** SHOULD NEVER REACH HERE
	*/

	c_printf( "*** INIT IS EXITING???\n" );
	exit( X_FAILURE );

}
