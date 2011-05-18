/*
 *
 * File: mm.h
 *
 * Author: Myqe Baril
 *
 * Contributor: used malloc as a reference
 *
 * Description:
 *   A memory management system written by me to better understand it and 
 *   for it to be better used with the paging system I have created
 *   Used the GNU library malloc as a reference for the structs and 
 *   optimizations used.
 */ 

#ifndef _MM_H
#define _MM_H

#include "headers.h"

#ifndef __ASM__20093__

/*
 * General (C and/or assembly) definitions
 */

/*
 * Start of C-only definitions
 */

#define MNGR_SIZE 0x1000000
#define MEM_START 0x11F000


/*
 * Prototypes
 */

//used Doug Lea's (GNU library, also nearby ACM holder) malloc as a reference
typedef struct malloc_chunk {
  uint32_t             prev_foot; //size of the previous chunk
  uint32_t             head;      //size of this chunk
  struct malloc_chunk *fnext;     //used if free chunk
  struct malloc_chunk *fprev;
} chunk_t;

//manages the chunks
typedef struct chunk_mngr {
  chunk_t *free;
  uint32_t size;
} chunk_mngr_t;

/*
 * kalloc(size, address, aligned)
 *
 * allocates memory based on the parameters and if the mngr
 * has been initalized yet
 *
 */
uint32_t kalloc(uint32_t size, uint32_t *phys_addr, uint32_t aligned);

/*
 * kfree(ptr)
 *
 * frees the memory at the ptr's location
 */
void kfree( void *p );

/*
 * chunk_mngr_t *_mngr_init(void);
 *
 * inits a new page aligned mngr
 *
 */
chunk_mngr_t *_mngr_init(void);

/*
 * void* _cm_alloc(mngr, size, aligned?);
 *
 * gives a new chunk of memory depending on the options
 *
 */
void *_cm_alloc(chunk_mngr_t *cm, uint32_t size, int p_align);

/*
 * coalesce(head, chunk 1, chunk 2, chunk 3)
 *
 * compresses the chunks depending on where they need to be compressed
 *
 */
void coalesce(uint32_t new_head, chunk_t** m1, chunk_t** m2, chunk_t** m3);

/*
 * _cm_free(ptr, mngr);
 *
 * frees memory in the mngr at the ptr's location for furture use
 */
void _cm_free(void *p, chunk_mngr_t *cm_ptr);

#endif

#endif
