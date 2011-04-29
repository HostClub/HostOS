/*
** SCCS ID:	@(#)kalloc.c	1.6	04/13/10
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

#define	__KERNEL__20093__

#include "headers.h"

#include "kalloc.h"
#include "x86arch.h"

/*
** PRIVATE DEFINITIONS
*/

#define	FAST_SEARCH_INCREMENT	4096

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
	( (block)->length + sizeof( (block)->length ) / WORD_SIZE )

/*
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
**
** Description:	Allocate a block of memory from the free list.  The length
**		argument is rounded up to the next integral number of words.
**		More memory may be returned than was requested, but the
**		caller must not depend on this in any way.  If no memory
**		is available, NULL is returned.
*/
void *_kalloc( uint_t desired_length ){
	blockinfo_t	*block;
	blockinfo_t	**pointer;

	/*
	** Convert the length to words (must round the count up to the next
	** integral multiple of the word size).
	*/
	desired_length += WORD_SIZE - 1;
	desired_length >>= LOG2_OF_WORD_SIZE;

	/*
	** Look for the first entry that is large enough.
	*/
	pointer = &_freelist;
	block = _freelist;
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
	    + sizeof( blockinfo_t ) / WORD_SIZE ){
		/*
		** Yes, there is enough room to allocate only a piece
		** of this block.  Construct a new block at the end of
		** this one.
		*/
		blockinfo_t *fragment;
		int	fragment_size;

		fragment_size = sizeof( block->length ) / WORD_SIZE
		    + desired_length;
		fragment = (blockinfo_t *)( &block->info.memory + block->length
		    - fragment_size );
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


/*
** Name:	_kfree
**
** Description:	returns a memory block to the list of available blocks,
**		combining it with adjacent blocks if they're present.
*/
void _kfree( void *address ){
	blockinfo_t *used;
	blockinfo_t *block;
	blockinfo_t *previous;

	/*
	** Don't do anything if the address is NULL.
	*/
	if( address == NULL ){
		return;
	}

	used = (blockinfo_t *)( (char *)address - WORD_SIZE );
	block = _freelist;
	previous = NULL;

	/*
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
		used->info.next = _freelist;
		_freelist = used;
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
			used->info.next = block->info.next;
			used->length += length_of( block );
		}
	}
}


/*
** Name:	_kalloc_init
**
** Description:	Find what memory is present on the system and construct
**		the list of free memory blocks.
*/
void _kalloc_init( void ){
	c_puts("In _kalloc_init\n");
	
	
	extern int _end;
	blockinfo_t	*block;
	blockinfo_t	**pointer = &_freelist;
	volatile int	*addr;
	int	original;
	int	*firstaddr;
	int	increment = 1;
	int	present;
	int	last_present = FALSE;

	
	c_printf("End %x %x\n" , _end , &_end);
	
	//TODO UNCOMMENT THIS
	//for( addr = (volatile int *)&_end;
	  for( addr = (volatile int *)0x0100000; 
	    addr <= (volatile int *)0xf0000000;
	    addr += increment ){
		/*
		** Store something and then read it back to see if there
		** is any memory at this address.
		*/

#define	VALUE	0x5a5aa5a5
		original = *addr;
		*addr = VALUE;
		present = *addr == VALUE;
		*addr = original;

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
			firstaddr = (int *)addr;
		}
		else {
			/*
			** We've found the end of a block: add it to the
			** free list if it isn't the video memory.
			** NOTE: THIS TEST ASSUMES THERE IS NO REAL MEMORY
			** ON EITHER SIDE OF THE VIDEO MEMORY!
			*/
			if( firstaddr == (int *)VIDEO_BASE_ADDR ){
				continue;
			}

			block = (blockinfo_t *)firstaddr;
			block->length = addr - &block->info.memory;
			block->info.next = NULL;
			*pointer = block;
			pointer = &block->info.next;
		}
	}

	c_printf("%x\n" , addr);
	c_puts("Ending _kalloc_init\n");
	
	dump_freelist();

}

/*
** PRIVATE GLOBAL VARIABLES
*/
