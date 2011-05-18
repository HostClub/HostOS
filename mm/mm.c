/*
 * File: mm.c
 *
 * Author: Michael Baril
 *
 * Contributor: used malloc as a reference
 *
 * Description: First Fit dynamic allocation.
 *
 */

#define __KERNEL__20103__

#include "headers.h"

#include "mm.h"
#include "x86arch.h"
#include "paging.h"

//MEM START AND mngr START HAVE TO BE PAGE ALIGNED
#define CHUNK_SIZE     8 //size of just the allocated chunk 

//the first free chunk
#define FIRST_CHUNK(h)  (((uint32_t)(h)) + sizeof(chunk_mngr_t))
//the previous free chunk
#define PREV_CHUNK(c)  (((uint32_t)(c)) - ((c)->prev_foot) - CHUNK_SIZE)
//the next free chunk
#define NEXT_CHUNK(c)  (((uint32_t)(c)) + CHUNK_SIZE  + ((c)->head))
//how2 align the start of memory
#define ALIGN_START(x)     (((uint32_t)x & 0xFFFFF000) + 0x1000)
//aligning addresses
#define ALIGN(x) (((uint32_t)x + 0x1000) - ((uint32_t)x&0xFFF))

extern int _end;
//uint32_t alloc_addr = (uint32_t)&_end;
//end isnt working.....
//use this address!
uint32_t alloc_addr = (uint32_t)MEM_START;
chunk_mngr_t *base_mngr = 0;
extern page_dir_t *base_dir;



uint32_t kalloc(uint32_t size, uint32_t *phys_addr, uint32_t align){
  //before the mngr is set up
  if(base_mngr == 0){
    if( (alloc_addr & 0xFFFFF000) && align == 1){
      alloc_addr = ALIGN_START(alloc_addr);
    }

    if(phys_addr){
      *phys_addr = alloc_addr;
    }

    uint32_t ret_addr = alloc_addr;
    alloc_addr += size;
    return ret_addr;

  //after the mngr is all ready to go
  }else{
    uint32_t addr = (uint32_t)_cm_alloc(base_mngr, size, align);
    if(phys_addr){
      page_t *p = get_page(addr, 0, base_dir);
      *phys_addr = p->frame * PAGE_SIZE + (addr & 0xFFF);
    }
    return addr;
  }
  
}

void kfree(void *p){
  _cm_free(p,base_mngr);
}

chunk_mngr_t *_mngr_init( void ){
  chunk_mngr_t *cm = (chunk_mngr_t*)kalloc(MNGR_SIZE,0,0) ;
  chunk_t *next = (chunk_t*)FIRST_CHUNK(cm);
  next = (chunk_t *)ALIGN_START(next);
  cm->free = next;
  uint32_t size = MNGR_SIZE - 2*CHUNK_SIZE - sizeof(chunk_mngr_t);
  cm->size = ALIGN_START(size);
  next->head = cm->size;
  next->prev_foot = 0;
  next->fprev = 0;
  next->fnext = 0;
  return cm;
}

void *_cm_alloc(chunk_mngr_t *cm, uint32_t size, int p_align){
  chunk_t *free = cm->free;
  if( !free ){
    c_puts("not free....\n");
    return 0;
  }
  //if it is all allocated only using 8bits instead of the sizeof(chunk_t)
  if( size < sizeof(chunk_t)-CHUNK_SIZE ) {
    size = sizeof(chunk_t)-CHUNK_SIZE;
  }
  while(free){
    if( free->head > size ){
      //make a new chunk and put the data into the old one
      chunk_t* new = (chunk_t*)((int32_t)free + size + CHUNK_SIZE);
      if(p_align){
        new = (chunk_t*) (((uint32_t)new + 0x1000) - ((uint32_t)new&0xFFF));
        size = 0x1000 - (size & 0xFFF);
      }
      new->head = free->head - size - CHUNK_SIZE; 
      new->prev_foot = size;
      new->fprev = free->fprev; 
      new->fnext = free->fnext; 
      
      if(free->fnext){
        free->fnext->fprev = new;
      }if(free->fprev){
        free->fprev->fnext = new;
      }if(free == cm->free){
        cm->free = new;
      }
      
      free->head  = size;
      free->fprev = 0;
      free->fnext = 0;
      return (void*)((uint32_t)free + CHUNK_SIZE);


    }else if (free->head == size && !p_align){
      //if it fits perfectly but is not page aligned
      if(free->fnext){
        free->fnext->fprev = free-> fprev;
      }if(free->fprev){
        free->fprev->fnext = free->fnext;
      }if(free==cm->free){
        cm->free = free->fnext;
      }
      free->fprev = 0;
      free->fnext = 0;
      return (void*)((uint32_t)free + CHUNK_SIZE);
     
    }
    //Check the next free chunk
    free=free->fnext;
  }
  return 0;
}

//merges chunks
void coalesce(uint32_t new_head, chunk_t** m1, chunk_t** m2, chunk_t** m3){
  (*m1)->head = new_head;
  (*m1)->fnext = (*m2)->fnext;
  if((*m2)->fnext){
    (*m2)->fnext->fprev = (*m1);

  }
  (*m3) = (chunk_t*)((uint32_t)(*m1) + (*m1)->head + CHUNK_SIZE);
  (*m3)->prev_foot = (*m1)->head;
}

void _cm_free( void *p, chunk_mngr_t *cm_ptr ){
  chunk_mngr_t *cm = cm_ptr;
  chunk_t *chunk = (chunk_t*)((uint32_t)p - (CHUNK_SIZE));
  chunk_t *free = cm->free;
  chunk_t *last = cm->free;
  if ( !p ){
    return;
  }
  //no chunks
  if( !cm->free ){
    cm->free = chunk;
    chunk->fprev = 0;
    chunk->fnext = 0;
  //first free chunk
  }else if(chunk < cm->free){
    cm->free->fprev = chunk;
    chunk->fnext = cm->free;
    cm->free = chunk;
    chunk->fprev = 0;
  //in the midle of chunks
  }else{
    while(free){
      if(free > chunk){
        chunk->fprev = last;
        chunk->fnext = free;
        free->fprev = chunk;
        last->fnext = chunk;
        break;
      }
      last = free;
      free = free->fnext;
    }
    if(!free){
      chunk->fprev = free;
      chunk->fnext = 0;
      last->fnext = chunk;
    }
  }


  chunk_t *prev = (chunk_t*)PREV_CHUNK(chunk);
  chunk_t *next = (chunk_t*)NEXT_CHUNK(chunk);

  //check to see if we are aligned
  if(!(((uint32_t)chunk->fprev % 0x1000) ||
       ((uint32_t)chunk->fnext % 0x1000)) &&
      ((ALIGN(prev) == (uint32_t)chunk->fprev) || 
       (ALIGN(next) == (uint32_t)chunk->fnext))){
     prev = (chunk_t*)ALIGN(prev);
     next = (chunk_t*)ALIGN(next);
  }
  uint32_t new_head = 0;

  //merge both
  if(prev == chunk->fprev && next == chunk->fnext){
    new_head = prev->head + (chunk->head + CHUNK_SIZE) +
      (next->head + CHUNK_SIZE);
    coalesce(new_head, &prev, &next, &next); 
  
  //merge next
  }else if(next == chunk->fnext){
    new_head = chunk->head + next->head + CHUNK_SIZE;
    coalesce(new_head, &chunk, &next, &next);
  
  //merg prev
  }else if(prev == chunk->fprev){
    new_head = prev->head + chunk->head + CHUNK_SIZE;
    coalesce(new_head, &prev, &chunk, &next);
  }
  return;
}
