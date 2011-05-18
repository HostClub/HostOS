/*
 * File: mm.c
 *
 * Author: Michael Baril
 *
 * Contributor:
 *
 * Description: ...
 *
 *
 */

#define __KERNEL__20103__

#include "headers.h"

#include "mm.h"
#include "x86arch.h"

#define MEM_START ((void*) 0x10FFEF)
#define HEAP_SIZE  0x1000000  //16MB
#define FIRST_CHUNK(h)  (((uint32_t)(h)) + sizeof(heap_t))
#define NEXT_CHUNK(c)  (((uint32_t)(c)) + sizeof(chunk_t) + ((c)->head))
#define PREV_CHUNK(c)  (((uint32_t)(c)) - ((c)->prev_foot) - sizeof(chunk_t))
#define HEAP_END(h)  (((uint32_t)(h)) + HEAP_SIZE - 2*(sizeof(chunk_t))

extern int _end;
//uint32_t alloc_addr = (uint32_t)&_end;
uint32_t alloc_addr = (uint32_t)MEM_START;
heap_t *base_heap = 0;
//extern page_dir_t *base_dir;


uint32_t kalloc(uint32_t size, uint32_t *phys_addr, uint32_t align){
  //if(base_heap){
    if( (alloc_addr & 0xFFFFF000) && align == 1){
      alloc_addr &= 0xFFFFF000;
      alloc_addr += 0x1000;
    }

    if(phys_addr){
      *phys_addr = alloc_addr;
    }
    uint32_t ret_addr = alloc_addr;
    alloc_addr += size;
    return ret_addr;
  /*}else{
    uint32_t *addr = _halloc(size, align, base_heap);
    if(phys_addr){
      page_t *p = get_page(addr, 0, base_dir);
      *phys = page->frame * PAGE_SIZE + (addr & 0xFFF);
    }
    return addr;
  }*/

}

void kfree(void *p){
  _hfree(p,base_heap);
}

heap_t *_heap_init( void ){
  heap_t *h = (heap_t*)kalloc(sizeof(heap_t),0,0) ;
  chunk_t *free = (chunk_t*)FIRST_CHUNK(h);
  h->free = free;
  h->size = HEAP_SIZE - 2*sizeof(chunk_t) - sizeof(heap_t);
  /*if(h->size & 0xFFFFF000 != 0){
    h->size &= 0xFFFFF000;
    h->size += 0x1000;
  }
  */
  free->fnext = 0;
  free->fprev = 0;
  free->head = h->size;

  free->prev_foot = 0;
  free->used = 0;
  return h;
}

void *_halloc(heap_t *h, uint32_t size, int p_align){
  chunk_t *free = h->free;
  //TODO: Stuff
  if( !free ){
    return 0;
  }
  if( size < sizeof(chunk_t) ) {
    size += sizeof(chunk_t);
  }
  while(free){
    //split the block into 2 blocks, one with the leftover one with enough
    if( free->head > size ){
      chunk_t* new = (chunk_t*)((int32_t)free + size + sizeof(chunk_t));
      new->head = free->head - size - sizeof(chunk_t); 
      new->prev_foot = size;
      new->used = size;
      new->fnext = free->fnext; 
      new->fprev = free->fprev; 
      if(free->fprev){
        free->fprev->fnext = new;
      }if(free->fnext){
        free->fnext->fprev = new;
      }if(free == h->free){
        h->free = new;
      }
      free->head  = size;
      free->fnext = 0;
      free->fprev = 0;
      return (void*)((uint32_t)free + sizeof(chunk_t));


    }else if (free->head == size){
      if(free->fnext){
        free->fnext->fprev = free-> fprev;
      }
      if(free->fprev){
        free->fprev->fnext = free->fnext;
      }
      if(free==h->free){
        h->free = free->fnext;
      }
      free->fnext = 0;
      free->fprev = 0;
      return (void*)((uint32_t)free + sizeof(chunk_t));
    }
    free=free->fnext;
   
  }
  return 0;
}

void _hfree( void *p, heap_t *h_ptr ){
  heap_t *h = h_ptr;
  chunk_t *chunk = (chunk_t*)((uint32_t)p - (sizeof(chunk_t)));
  chunk_t *free = h->free;
  chunk_t *last = h->free;
  if ( !p ){
    return;
    //TODO ERROR;
  }
  if( !h->free ){
    h->free = chunk;
    chunk->fnext = 0;
    chunk->fprev = 0;
  }else if(chunk < h->free){
    h->free->fprev = chunk;
    chunk->fnext = h->free;
    h->free = chunk;
    chunk->fprev = 0;
  }else
    while(free){
      if(free > chunk){
        chunk->fnext = free;
        chunk->fprev = last;
        last->fnext = chunk;
        free->fprev = chunk;
        break;
      }
      last = free;
      free = free->fnext;
    }
  if(!free){
    last->fnext = chunk;
    chunk->fprev = free;
    chunk->fnext = 0;
  }
  chunk_t *prev = (chunk_t*)PREV_CHUNK(chunk);
  chunk_t *next = (chunk_t*)NEXT_CHUNK(chunk);
  if(prev == chunk->fprev && next == chunk->fnext){
    prev->head = prev->head + (chunk->head + sizeof(chunk_t)) +
        (next->head + sizeof(chunk_t));
    prev->fnext = next->fnext;
    if(next->fnext){
      next->fnext->fprev = prev;
    }
    next = (chunk_t*)((uint32_t)prev + sizeof(chunk_t) + prev->head);
    next->prev_foot = prev->head;
  }else if(prev == chunk->fprev){
    prev->head = prev->head + chunk->head + sizeof(chunk_t);
    prev->fnext = chunk->fnext;
    if(chunk->fnext){
      chunk->fnext->fprev = prev;
    }
    next = (chunk_t*)((uint32_t)prev + prev->head + sizeof(chunk_t));
    next->prev_foot = prev->head;
  }else if(next == chunk->fnext){
    chunk->head = chunk->head + next->head + sizeof(chunk_t);
    chunk->fnext = next->fnext;
    if(chunk->fnext){
      chunk->fnext->fprev = chunk;
    }
    next = (chunk_t*)((uint32_t)chunk + chunk->head + sizeof(chunk_t));

    next->prev_foot = chunk->head;
  }
  return;
}


/*
void _kalloc_init(void){
  chunk_t *chunk;
  volatile int *addr;
  int orig;
  int *first;
  int increment = 1;
  int present;
  int last_present = FALSE;

  for(addr  = (volatile int *)&_end; 
      addr <= (volatile int *)0xf0000000;
      addr += increment ){
#define DUMMY 0x5a5aa5a5
    original = *addr;
    *addr = DUMMY;
    present = *addr == VALUE;
    *addr = original;

    if(present == last_present){
      continue;
    }
    if(increment != 1){
      addr -= increment;
      increment = 1;
      continue;
    }
    increment = FAST_SEARCH_INCREMENT;
    last_present = present;

    if(present){
      first = (int *)addr;
      //page align it...
      if(first & 0xfffff000 != 0){
        first &= 0xfffff000;
        first += 0x1000;
      }
    }else{
      if(first == (int *)VIDEO_BASE_ADDR){
        continue;
      }

      chunk = (chunk_t *)first;
      chunk->used = 0;
      chunk->head = //TODO: figure out
        chunk->fnext = 0;
      chunk->fprev = 0;
      chunk->prev_foot = 0;
    }
  }
}
*/

