/*
** SCCS ID:	@(#)kalloc.h	1.3	04/13/10
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

#include "headers.h"

/*
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
** This structure keeps track of a single block of memory.  It is also
** used to remember the length of a block that has been allocated.
*/
typedef	struct	BLOCKINFO {
	uint_t	length;		/* Useable length of this block in words */
	union	{
		struct	BLOCKINFO *next;	/* Pointer to next free block */
		int		memory;		/* First word of the memory */
	} info;
} blockinfo_t;

/*
** Globals
*/

extern	blockinfo_t	*_freelist;	/* First free block in the system */

/*
** Prototypes
*/

/*
** Functions that manipulate free memory blocks.
*/

/*
** Name:	_kalloc
**
** Description:	Dynamically allocates a block of memory from the system's
**		available memory.
** Arguments:	Number of bytes desired
** Returns:	A pointer to a block of memory at least that large (or
**		NULL if no block was found)
*/
void	*_kalloc( uint_t desired_length );

/*
** Name:	_kalloc_align
**
** Description:	Dynamically allocates a block of memory from the system's
**		available memory.
** Arguments:	Number of bytes desired
**				Byte alignment
** Returns:	A pointer to a block of memory at least that large (or
**		NULL if no block was found)
*/
void	*_kalloc_align( uint_t desired_length, uint32_t alignment );

/*
** Name:	_kfree
**
** Description:	Frees a previously allocated block of dynamic memory.
** Arguments:	A pointer to the block to be freed
*/
void	_kfree( void *block );		/* return a block to the free list */

/*
** Name:	_kalloc_init
**
** Description:	Scans memory to see what we have available and builds the
**		list of free memory.
*/
void	_kalloc_init( void );

#endif

#endif
