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

void checkCPUs( void );

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
** CPU Check; Checks for CPUID
*/

#define WRITE_EFLAGS(x) asm( "push  %%eax;\
                              popfl;"\
                            :\
                            : "a"(x)\
                           )

#define READ_EFLAGS(x) asm( "pushfl;\
                             pop  %%eax;"\
                            : "=a"(x)\
                            :\
                          )

#define CPUID_D(c, d) asm( "cpuid;"\
                          : "=a"(*(uint32_t *)d), "=b"(*(uint32_t *)(d + 4)), "=c"(*(uint32_t *)(d + 8)), "=d"(*(uint32_t *)(d + 12))\
                          : "a"(c)\
                         )

#define CPUID_D_C(c, r, d) asm( "cpuid;"\
                               : "=a"(*(uint32_t *)d), "=b"(*(uint32_t *)(d + 4)), "=c"(*(uint32_t *)(d + 8)), "=d"(*(uint32_t *)(d + 12))\
                               : "a"(c), "c"(r)\
                              )

#define CPUID_R(c, r) asm( "cpuid;"\
                           : "=a"(r)\
                           : "a"(c)\
                         )

//Swap the third and fourth registers (to undo the backward compatibility)
#define UNSCRAMBLE(d) (*(uint32_t *)((d) + 8)) ^= (*(uint32_t *)((d) + 12));\
                      (*(uint32_t *)((d) + 12)) ^= (*(uint32_t *)((d) + 8));\
                      (*(uint32_t *)((d) + 8)) ^= (*(uint32_t *)((d) + 12))

#define VENDOR_ID      0x00000000
#define FEATURE_INFO   0x00000001
#define D_CACHE_PARAM  0x00000004
#define EXT_FUNCS      0x80000000
#define EXT_FEATURES   0x80000001
#define BRAND_STRING1  0x80000002
#define BRAND_STRING2  0x80000003
#define BRAND_STRING3  0x80000004

#define SIG_I7    0x106A5
#define SIG_CORE2 0x1067A


void checkCPUs() {
	uint32_t id;
	int i;
	unsigned char data[17];
	uint32_t maxCpuIdOp;

	READ_EFLAGS( id );
	WRITE_EFLAGS( id | 0x200000 );
	READ_EFLAGS( id );
	id = (id >> 21) & 1;

	if( !id ) {
		c_puts( "CPUID not supported!!!\n" );
		return;
	}


	CPUID_D(VENDOR_ID, data);
	UNSCRAMBLE(data);
	maxCpuIdOp = (uint32_t)data;
	data[16] = 0;

	c_printf( "Max CPUID Op: %d\n", maxCpuIdOp );
	c_printf( "CPU Vendor: '%s'\n", (data + 4) );

	CPUID_R(EXT_FUNCS, id);
	if (id >= BRAND_STRING3) {
		//Supports extended functions
		CPUID_D(BRAND_STRING1, data);
		c_printf("%s", data);
		CPUID_D(BRAND_STRING2, data);
		c_printf("%s", data);
		CPUID_D(BRAND_STRING3, data);
		c_printf("%s\n", data);

		CPUID_D(EXT_FEATURES, data);
		c_printf("Extended Features (ECX):  0x%x\n", *((uint32_t *)(data + 8)));
		c_printf("Extended Features (EDX):  0x%x\n", *((uint32_t *)(data + 12)));
	} else {
		//Doesn't support extended functions
		CPUID_D(FEATURE_INFO, data);
		UNSCRAMBLE(data);
		id = *((uint32_t *)data) & 0x0FFF3FFF;

		c_printf("Signature:       0x%x (", id);
		switch (id) {
			case SIG_I7:
				c_puts("Core i7 Processor)\n");
				break;
			case SIG_CORE2:
				c_puts("Core 2 Extreme Processor)\n");
				break;
			default:
				c_puts("Unknown Processor)\n");
		}
	}

	CPUID_D(FEATURE_INFO, data);
	UNSCRAMBLE(data);
	id = *((uint32_t *)data) & 0x0FFF3FFF;

	char steppingID =     (id & 0x0000000F) >> 0;
	char modelNumber =    (id & 0x000000F0) >> 4;
	char familyCode =     (id & 0x00000F00) >> 8;
	char type =           (id & 0x00003000) >> 12;
	char extendedModel =  (id & 0x000F0000) >> 16;
	char extendedFamily = (id & 0x0FF00000) >> 20;

	c_printf("Features (ECX): 0x%x\n", *((uint32_t *)data + 8));
	c_printf("Features (EDX): 0x%x\n", *((uint32_t *)data + 12));

	c_printf("Stepping ID:     %d\n", steppingID);
	c_printf("Model Number:    %d\n", modelNumber);
	c_printf("Family Code:     %d\n", familyCode);
	c_printf("Type:            %d\n", type);
	c_printf("Extended Model:  %d\n", extendedModel);
	c_printf("Extended Family: %d\n", extendedFamily);

	id = *((uint32_t *)(data + 4));
	c_printf("\nLogical Processors: %d\n", (id & 0x00FF0000) >> 16);


	// Enumerate the deterministic cache parameters
	CPUID_D_C(D_CACHE_PARAM, 0, data);
	UNSCRAMBLE(data);

	id = *((uint32_t *)(data));
	c_printf("APIC IDs:           %d\n", 1 + ((id & 0xFC000000) >> 26));

	i = 0;
	while ((id & 0x0F) != 0) {
		char sharingThreads = ((id & 0x03FFC000) >> 14) + 1;
		char level = (id & 0xF0) >> 4;
		char type = (id & 0x0F);

		c_printf("Cache Number: %d   ", i);
		c_printf("  Threads sharing this cache: %d   ", sharingThreads);
		c_printf("  Level: %d   ", level);
		c_printf("  Type: ");
		switch (type) {
			case 1:
				c_printf("Data");
				break;
			case 2:
				c_printf("Instruction");
				break;
			case 3:
				c_printf("Unified");
				break;
			default:
				c_printf("INVALID");
		}

		c_putchar('\n');

		i++;
		CPUID_D_C(D_CACHE_PARAM, i, data);
		id = *((uint32_t *)(data));
	}
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

	checkCPUs();

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
