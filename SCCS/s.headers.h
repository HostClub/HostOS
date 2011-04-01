h47491
s 00036/00000/00000
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
** File:	headers.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Standard includes needed in all C source files
*/

#ifndef _HEADERS_H
#define _HEADERS_H

#ifndef __ASM__20103__

// only pull these in if we're not in assembly language

#include "defs.h"
#include "types.h"

#include "c_io.h"
#include "support.h"

// OS and user code have different library headers

#ifdef __KERNEL__20103__
#include "klib.h"
#else
#include "ulib.h"
#endif

#endif

#endif
E 1
