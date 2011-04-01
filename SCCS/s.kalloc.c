h47188
s 00064/00038/00248
d D 1.6 10/04/13 15:29:17 wrc 6 5
c cosmetic fixes - naming conventions, source file format
e
s 00022/00003/00264
d D 1.5 02/04/04 09:25:02 kar 5 4
c Class of 4/3 - final debugging of baseline system
e
s 00014/00003/00253
d D 1.4 02/03/28 08:05:39 kar 4 3
c kfree now ignores NULL arguments.
e
s 00001/00001/00255
d D 1.3 02/03/26 14:26:08 kar 3 2
c Fix minor compilation errors
e
s 00015/00015/00241
d D 1.2 02/03/26 13:44:21 kar 2 1
c Revisions after 3/21/02
e
s 00256/00000/00000
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
** File:	kalloc.c
**
** Author:	4003-506 class of 20013
**
** Contributor:
**
** Description:	Functions to perform dynamic memory allocation in the OS.
**		NOTE: these should NOT be called by user processes!
*/

I 2
D 6
#include "x86arch.h"
E 2
#include "types.h"
E 6
I 6
#define	__KERNEL__20093__

#include "headers.h"

E 6
#include "kalloc.h"
I 5
D 6
#include "c_io.h"
E 6
I 6
#include "x86arch.h"
E 6

I 6
/*
** PRIVATE DEFINITIONS
*/

E 6
E 5
#define	FAST_SEARCH_INCREMENT	4096

D 6
Blockinfo	*freelist;

E 6
/*
** Name:	adjacent
**
** Arguments:	addresses of two blocks
**
** Description:	Determines whether the second block immediately follows
**		the first one.
*/
#define	adjacent(first,second)	\
	( &(first)->info.memory + (first)->length == (int *)(second) )

/*
** Name:	length_of
**
** Arguments:	address of a block
**
** Description:	Computes the total length of a block (including the
**		header).
*/
#define	length_of(block)	\
D 2
	( (block)->length + sizeof( (block)->length ) / WORD_SIZE;
E 2
I 2
	( (block)->length + sizeof( (block)->length ) / WORD_SIZE )
E 2

/*
D 6
** Name:	kalloc
E 6
I 6
** PRIVATE FUNCTIONS
*/

static void dump_freelist( void ){
	blockinfo_t *block;
	c_printf( "&_freelist=%08x\n", &_freelist );
	for( block = _freelist; block != NULL; block = block->info.next ){
		c_printf( "block=%08x length=%08x (ends at %08x) next=%08x\n",
		    block, block->length, block->length * 4 + 4 + (int)block,
		    block->info.next );
	}

}

/*
** PUBLIC GLOBAL VARIABLES
*/

blockinfo_t	*_freelist;

/*
** PUBLIC FUNCTIONS
*/

/*
** Name:	_kalloc
E 6
**
D 2
** Arguments:	desired_length: number of bytes of memory needed
**
E 2
** Description:	Allocate a block of memory from the free list.  The length
**		argument is rounded up to the next integral number of words.
**		More memory may be returned than was requested, but the
**		caller must not depend on this in any way.  If no memory
**		is available, NULL is returned.
*/
D 6
void *kalloc( uint desired_length ){
	Blockinfo	*block;
	Blockinfo	**pointer;
E 6
I 6
void *_kalloc( uint_t desired_length ){
	blockinfo_t	*block;
	blockinfo_t	**pointer;
E 6

	/*
	** Convert the length to words (must round the count up to the next
	** integral multiple of the word size).
	*/
	desired_length += WORD_SIZE - 1;
	desired_length >>= LOG2_OF_WORD_SIZE;

	/*
	** Look for the first entry that is large enough.
	*/
D 6
	pointer = &freelist;
	block = freelist;
E 6
I 6
	pointer = &_freelist;
	block = _freelist;
E 6
	while( block != NULL && block->length < desired_length ){
		pointer = &block->info.next;
		block = *pointer;
	}

	if( block == NULL ){
		/*
		** No blocks were found that are large enough.
		*/
		return NULL;
	}

	/*
	** Found an entry.  See if it is bigger than we need.
	*/
	if( block->length > desired_length
D 6
	    + sizeof( Blockinfo ) / WORD_SIZE ){
E 6
I 6
	    + sizeof( blockinfo_t ) / WORD_SIZE ){
E 6
		/*
		** Yes, there is enough room to allocate only a piece
		** of this block.  Construct a new block at the end of
		** this one.
		*/
D 6
		Blockinfo *fragment;
E 6
I 6
		blockinfo_t *fragment;
E 6
		int	fragment_size;

		fragment_size = sizeof( block->length ) / WORD_SIZE
		    + desired_length;
D 2
		fragment = &block->info.memory + block->length - fragment_size;
E 2
I 2
D 6
		fragment = (Blockinfo *)( &block->info.memory + block->length
E 6
I 6
		fragment = (blockinfo_t *)( &block->info.memory + block->length
E 6
		    - fragment_size );
E 2
		fragment->length = desired_length;
		block->length -= fragment_size;
		block = fragment;
	}
	else {
		/*
		** No, it is just the right size.  Remove it from
		** the list.
		*/
		*pointer = block->info.next;
	}
	return &block->info.memory;
}

I 6

E 6
/*
D 6
** Name:	kfree
E 6
I 6
** Name:	_kfree
E 6
**
D 2
** Arguments:	address: pointer to the useable part of the memory to be freed.
**
E 2
** Description:	returns a memory block to the list of available blocks,
**		combining it with adjacent blocks if they're present.
*/
D 6
void kfree( void *address ){
D 2
	Blockinfo *used = (Blockinfo *)( (char *)memory - WORD_SIZE );
E 2
I 2
D 4
	Blockinfo *used = (Blockinfo *)( (char *)address - WORD_SIZE );
E 2
	Blockinfo *block = freelist;
	Blockinfo *previous = NULL;
E 4
I 4
	Blockinfo *used;
	Blockinfo *block;
	Blockinfo *previous;
E 6
I 6
void _kfree( void *address ){
	blockinfo_t *used;
	blockinfo_t *block;
	blockinfo_t *previous;
E 6
E 4

	/*
I 4
	** Don't do anything if the address is NULL.
	*/
	if( address == NULL ){
		return;
	}

D 6
	used = (Blockinfo *)( (char *)address - WORD_SIZE );
	block = freelist;
E 6
I 6
	used = (blockinfo_t *)( (char *)address - WORD_SIZE );
	block = _freelist;
E 6
	previous = NULL;

	/*
E 4
	** Advance through the list until block and previous
	** straddle the place where the new block should be inserted.
	*/
	while( block != NULL && block < used ){
		previous = block;
		block = block->info.next;
	}

	/*
	** Is the new block going to be the first one in the list?
	*/
	if( previous != NULL ){
		/*
		** No.  See if it is adjacent to the previous block.
		*/
		if( adjacent( previous, used ) ){
			/*
			** Yes, combine them.
			*/
			previous->length += length_of( used );
			used = previous;
		}
		else {
			/*
			** No: insert the new block.
			*/
			used->info.next = previous->info.next;
			previous->info.next = used;
		}
	}
	else {
		/*
		** Yes, it is first.  Must update the list pointer to insert it.
		*/
D 6
		used->info.next = freelist;
		freelist = used;
E 6
I 6
		used->info.next = _freelist;
		_freelist = used;
E 6
	}

	/*
	** Is the new block going to be the last block in the list?
	*/
	if( block != NULL ){
		/*
		** No.  See if it is adjacent to the next block.
		*/
		if( adjacent( used, block ) ){
			/*
			** Yes, combine them.
			*/
D 2
			used->info.next - block.info.next;
E 2
I 2
			used->info.next = block->info.next;
E 2
			used->length += length_of( block );
		}
	}
}

I 5

D 6
void dump_freelist( void ){
	Blockinfo *block;
	c_printf( "&freelist=%08x\n", &freelist );
	for( block = freelist; block != NULL; block = block->info.next ){
		c_printf( "block=%08x length=%08x (ends at %08x) next=%08x\n",
		    block, block->length, block->length * 4 + 4 + (int)block,
		    block->info.next );
	}

}
E 6
E 5
/*
D 2
** Name:	construct_freelist
E 2
I 2
D 6
** Name:	init_freelist
E 6
I 6
** Name:	_kalloc_init
E 6
E 2
**
** Description:	Find what memory is present on the system and construct
**		the list of free memory blocks.
*/
D 2
void construct_freelist( void ){
E 2
I 2
D 6
void init_freelist( void ){
E 6
I 6
void _kalloc_init( void ){
E 6
E 2
	extern	int	_end;
D 6
	Blockinfo	*block;
	Blockinfo	**pointer = &freelist;
E 6
I 6
	blockinfo_t	*block;
	blockinfo_t	**pointer = &_freelist;
E 6
	volatile int	*addr;
I 5
	int	original;
E 5
D 2
	int	*firstword;
E 2
I 2
	int	*firstaddr;
E 2
	int	increment = 1;
	int	present;
	int	last_present = FALSE;

D 2
	for( addr = _end; addr >= _end; addr += increment ){
E 2
I 2
	for( addr = (volatile int *)&_end;
D 5
	    addr >= (volatile int *)&_end;
E 5
I 5
	    addr <= (volatile int *)0xf0000000;
E 5
	    addr += increment ){
E 2
		/*
		** Store something and then read it back to see if there
		** is any memory at this address.
		*/
D 5
		*addr = (int)addr;
		present = *addr == (int)addr;
E 5

I 5
#define	VALUE	0x5a5aa5a5
		original = *addr;
		*addr = VALUE;
		present = *addr == VALUE;
		*addr = original;

E 5
		/*
		** Did the present/absent state just change?
		*/
		if( present == last_present ){
			/*
			** No -- keep looking.
			*/
			continue;
		}

		/*
		** It changed.  Did we find the boundary or not?
		*/
		if( increment != 1 ){
			/*
			** Not yet, must go back and creep up on it slowly.
			*/
			addr -= increment;
			increment = 1;
			continue;
		}

		/*
		** Yes, we found the boundary.
		*/
		increment = FAST_SEARCH_INCREMENT;
		last_present = present;

		if( present ){
			/*
			** Found the beginning of a chunk: save this address.
			*/
D 2
			firstaddr = addr;
E 2
I 2
			firstaddr = (int *)addr;
E 2
		}
		else {
			/*
			** We've found the end of a block: add it to the
			** free list if it isn't the video memory.
			** NOTE: THIS TEST ASSUMES THERE IS NO REAL MEMORY
			** ON EITHER SIDE OF THE VIDEO MEMORY!
			*/
D 2
			if( firstaddr == VIDEO_BASE_ADDR ){
E 2
I 2
			if( firstaddr == (int *)VIDEO_BASE_ADDR ){
E 2
				continue;
			}

D 2
			block = first_addr;
E 2
I 2
D 3
			block = firstaddr;
E 3
I 3
D 6
			block = (Blockinfo *)firstaddr;
E 6
I 6
			block = (blockinfo_t *)firstaddr;
E 6
E 3
E 2
			block->length = addr - &block->info.memory;
			block->info.next = NULL;
			*pointer = block;
			pointer = &block->info.next;
		}
	}
I 6

E 6
I 5
	dump_freelist();
I 6

E 6
E 5
}
I 6

/*
** PRIVATE GLOBAL VARIABLES
*/
E 6
E 1
