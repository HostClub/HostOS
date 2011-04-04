/*
** SCCS ID:	@(#)headers.h	1.1	03/31/11
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
