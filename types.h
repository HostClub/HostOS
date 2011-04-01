/*
** SCCS ID:	@(#)types.h	1.1	03/31/11
**
** File:	types.h
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Basic system-wide data types
*/

#ifndef _TYPES_H
#define _TYPES_H

#ifndef __ASM__20103__

/*
** Types
*/

/*
** generic integer types
*/

typedef unsigned char		uint8_t;
typedef char			int8_t;

typedef unsigned short		uint16_t;
typedef short			int16_t;

typedef unsigned long		uint32_t;
typedef long			int32_t;

typedef unsigned long long	uint64_t;
typedef long long		int64_t;

typedef uint32_t		uint_t;

/*
** hardware-sized types
*/

typedef uint8_t		byte_t;
typedef uint16_t	word_t;
typedef uint32_t	dword_t;
typedef uint64_t	qword_t;

/*
** miscellaneous types
*/

typedef uint8_t		bool_t;

typedef int		status_t;

#endif

#endif
