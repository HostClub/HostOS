h27742
s 00123/00000/00000
d D 1.1 11/03/31 18:06:24 wrc 1 0
c date and time created 11/03/31 18:06:24 by wrc
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
** File:	clock.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Clock-related definitions and routines
*/

#ifndef _CLOCK_H
#define _CLOCK_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// clock interrupts per second

#define	CLOCK_FREQUENCY		100

#ifndef __ASM__20103__

/*
** Start of C-only definitions
*/

// pseudo-function to convert seconds to ticks

#define	SECONDS_TO_TICKS(n)	((n) * CLOCK_FREQUENCY)

// pseudo-function to convert ticks to floor(seconds)

#define	TICKS_TO_SECONDS(n)	((n) / CLOCK_FREQUENCY)

/*
** Timing constants
*/

// number of milliseconds per clock tick

#define	MS_PER_TICK		(1000/CLOCK_FREQUENCY)

/*
** Timing constants
**
** Approximately every N seconds, we want to dump the queues.
**
** Assuming a clock frequency of 100, "N seconds" will be "N00"
** ticks.  For simplicity, we actually dump after tick counts
** which are powers of 2:
**
**		Seconds		Ticks		Mask
**		=======		=====		=====
**		2.5		256		0x00ff
**		5		512		0x01ff
**		10		1024		0x03ff
**		20		2048		0x07ff
**		40		4096		0x0fff
*/

#define	CLK_SEC_2_5		0x00ff
#define	CLK_SEC_5		0x01ff
#define	CLK_SEC_10		0x03ff
#define	CLK_SEC_20		0x07ff
#define	CLK_SEC_40		0x0fff

/*
** Another set of constants, assuming a frequency of 18 ticks/second.
**
** "N seconds" will be N*18 ticks.  For simplicity, we actually dump
** after tick counts which are powers of 2:
**
**		Seconds		Ticks		Mask
**		=======		=====		=====
**		7		128		0x007f
**		14		256		0x008f
**		28		512		0x01ff
**		56		1024		0x03ff
**		113		2048		0x07ff
**		227		4096		0x0fff
*/

// #define	CLK_SEC_7		0x00ff
// #define	CLK_SEC_14		0x00ff
// #define	CLK_SEC_28		0x01ff
// #define	CLK_SEC_56		0x03ff
// #define	CLK_SEC_113		0x07ff
// #define	CLK_SEC_227		0x0fff

/*
** Types
*/

typedef uint32_t	time_t;

/*
** Globals
*/

extern time_t _system_time;

/*
** _isr_clock - clock ISR
*/

void _isr_clock( int vector, int code );

/*
** _clock_init()
**
** initialize all clock-related variables
*/

void _clock_init( void );

#endif

#endif
E 1
