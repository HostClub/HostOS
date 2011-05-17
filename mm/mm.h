/*
 *
 * File: mm.h
 *
 * Author: Myqe Baril
 *
 * Contributor: 
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

/*
 * Prototypes
 */

//used Doug Lea's (GNU library, also nearby ACM holder) malloc as a reference
typedef struct malloc_chunk {
  uint32_t             used;      //0 if free, x space used 
  uint32_t             prev_foot; //size of the previous chunk
  uint32_t             head;      //size of this chunk
  struct malloc_chunk *fnext;     //used if free chunk
  struct malloc_chunk *fprev;
} chunk_t;

typedef struct heap {
  chunk_t *free;
  uint32_t size;

  //TODO: ADD PAGING EXPANDING FLAGS
} heap_t;

uint32_t kalloc(uint32_t size, uint32_t *phys_addr, uint32_t aligned);

#endif

#endif
