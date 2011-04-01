h24948
s 00088/00000/00000
d D 1.1 11/03/31 18:06:26 wrc 1 0
c date and time created 11/03/31 18:06:26 by wrc
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
** File:	user.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	User routines
*/

#ifndef _USER_H
#define _USER_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// delay loop counts

#define	DELAY_LONG	100000000
#define	DELAY_STD	  2500000
#define	DELAY_ALT	  4500000

#ifndef __ASM__20103__

/*
** Start of C-only definitions
*/

/*
** User process controls.
**
** The value field of these definitions contains a list of the
** system calls this user process uses.
**
** To spawn a specific user process from the initial process,
** uncomment its entry in this list.
*/

#define	SPAWN_A		// writec, exit
#define	SPAWN_B		// writec, exit
#define	SPAWN_C		// writec, exit
#define	SPAWN_D		// writec, fork, exec, exit
#define	SPAWN_E		// writec, getpid, sleep, exit
#define	SPAWN_F		// writec, getpid, sleep, exit
#define	SPAWN_G		// writec, getpid, sleep, exit
#define	SPAWN_H		// writec
// no user i
#define	SPAWN_J		// writec, fork, exec, exit
#define	SPAWN_K		// writec, sleep, fork, exec, exit
#define	SPAWN_L		// writec, sleep, fork, exec, gettime, wait, exit
#define	SPAWN_M		// writec, fork, exec, exit
#define	SPAWN_N		// writec, sleep, fork, exec, gettime, getpid, getppid, exit
// no user O
#define	SPAWN_P		// writec, sleep, gettime
#define	SPAWN_Q		// writec, bogus, exit
#define	SPAWN_R		// writec, readc, sleep, exit
#define	SPAWN_S		// writec, sleep, forever
#define	SPAWN_T		// writec, getprio, setprio, exit
// no user U
// no user V

/*
** Users W-Z are spawned from other processes; they should
** should never be spawned directly.
*/

// user W:  write, sleep, getpid, exit
// user X:  write, getpid, exit
// user Y:  write, sleep, exit
// user Z:  write, exit


/*
** Prototypes for externally-visible routines
*/

void idle( void );

void init( void );

#endif

#endif
E 1
