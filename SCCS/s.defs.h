h61432
s 00067/00000/00000
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
** File:	defs.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	General definitions
*/

#ifndef _DEFS_H
#define _DEFS_H

#include "headers.h"

/*
** General (C and/or assembly) definitions
*/

// null pointer

#define	NULL		0

// status values

#define	E_SUCCESS	0
#define	E_BAD_PARAM	(-1)
#define	E_NO_QNODES	(-2)
#define	E_EMPTY		(-3)
#define	E_NOT_FOUND	(-4)
#define	E_UNIMPLEMENTED	(-5)

#define N_STATUS	6

// maximum number of simultaneous user processes

#define	N_PCBS		30
#define	N_STACKS	30

// Boolean constants

#define	FALSE		0
#define	TRUE		1

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

/*
** Prototypes
*/

#endif

#endif
E 1
