h42403
s 00038/00009/00062
d D 1.3 10/04/13 15:29:17 wrc 3 2
c cosmetic fixes - naming conventions, source file format
e
s 00027/00002/00044
d D 1.2 02/03/26 13:44:21 kar 2 1
c Revisions after 3/21/02
e
s 00046/00000/00000
d D 1.1 02/03/21 07:54:54 kar 1 0
c date and time created 02/03/21 07:54:54 by kar
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
** File:	kalloc.h
**
** Author:	4003-506 class of 20013
**
** Contributor:
**
** Description:	Structures and functions to support dynamic memory
**		allocation within the OS.
**
**		The list of free blocks is ordered by address to facilitate
**		combining freed blocks with adjacent blocks that are already
**		free.
**
**		All requests for memory are satisfied with blocks that are
**		an integral number of 4-byte words.  More memory may be
**		provided than was requested if the fragment left over after
**		the allocation would not be large enough to be useable.
*/

#ifndef	_KALLOC_H
#define	_KALLOC_H

I 3
#include "headers.h"

E 3
/*
I 3
** General (C and/or assembly) definitions
*/

#define	WORD_SIZE		sizeof(int)
#define	LOG2_OF_WORD_SIZE	2

#ifndef __ASM__20093__

/*
** Start of C-only definitions
*/

/*
** Types
*/

/*
E 3
** This structure keeps track of a single block of memory.  It is also
** used to remember the length of a block that has been allocated.
*/
typedef	struct	BLOCKINFO {
D 3
	uint	length;		/* Useable length of this block in words */
E 3
I 3
	uint_t	length;		/* Useable length of this block in words */
E 3
	union	{
		struct	BLOCKINFO *next;	/* Pointer to next free block */
		int		memory;		/* First word of the memory */
	} info;
D 3
} Blockinfo;
E 3
I 3
} blockinfo_t;
E 3

D 3
extern	Blockinfo	*freelist;	/* First free block in the system */
E 3
I 3
/*
** Globals
*/
E 3

I 3
extern	blockinfo_t	*_freelist;	/* First free block in the system */

E 3
/*
I 3
** Prototypes
*/

/*
E 3
** Functions that manipulate free memory blocks.
*/
D 2
void	*kalloc( uint desired_length );	/* allocate a block of memory from the free list */
E 2
I 2

/*
D 3
** Name:	kalloc
E 3
I 3
** Name:	_kalloc
E 3
**
** Description:	Dynamically allocates a block of memory from the system's
**		available memory.
** Arguments:	Number of bytes desired
** Returns:	A pointer to a block of memory at least that large (or
**		NULL if no block was found)
*/
D 3
void	*kalloc( uint desired_length );
E 3
I 3
void	*_kalloc( uint_t desired_length );
E 3

/*
D 3
** Name:	kfree
E 3
I 3
** Name:	_kfree
E 3
**
** Description:	Frees a previously allocated block of dynamic memory.
** Arguments:	A pointer to the block to be freed
*/
E 2
D 3
void	kfree( void *block );		/* return a block to the free list */
E 3
I 3
void	_kfree( void *block );		/* return a block to the free list */
E 3
D 2
void	construct_freelist( void );	/* build the freelist */
E 2
I 2

/*
D 3
** Name:	init_freelist
E 3
I 3
** Name:	_kalloc_init
E 3
**
** Description:	Scans memory to see what we have available and builds the
**		list of free memory.
*/
D 3
void	init_freelist( void );
E 3
I 3
void	_kalloc_init( void );
E 3

E 2
#endif
I 3

#endif
E 3
E 1
