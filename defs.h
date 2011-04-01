/*
** SCCS ID:	@(#)defs.h	1.1	03/31/11
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
