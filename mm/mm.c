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
#include "paging.h"

//MEM START AND HEAP START HAVE TO BE PAGE ALIGNED
#define CHUNK_SIZE     8 //size of just the allocated chunk 
#define FIRST_CHUNK(h)  (((uint32_t)(h)) + sizeof(heap_t))
#define NEXT_CHUNK(c)  (((uint32_t)(c)) + CHUNK_SIZE  + ((c)->head))
#define PREV_CHUNK(c)  (((uint32_t)(c)) - ((c)->prev_foot) - CHUNK_SIZE)
#define HEAP_END(h)  ((((uint32_t)(h)) + HEAP_SIZE) - 2*(CHUNK_SIZE))
#define ALIGN(x)     (((uint32_t)x & 0xFFFFF000) + 0x1000)

extern int _end;
//uint32_t alloc_addr = (uint32_t)&_end;
//end isnt working.....
//use this address!
uint32_t alloc_addr = (uint32_t)MEM_START;
heap_t *base_heap = 0;
extern page_dir_t *base_dir;



uint32_t kalloc(uint32_t size, uint32_t *phys_addr, uint32_t align){
  if(base_heap == 0){
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
  }else{
    uint32_t addr = (uint32_t)_halloc(base_heap, size, align);
    if(phys_addr){
      page_t *p = get_page(addr, 0, base_dir);
      *phys_addr = p->frame * PAGE_SIZE + (addr & 0xFFF);
    }
    return addr;
  }
  
}

void kfree(void *p){
  _hfree(p,base_heap);
}

heap_t *_heap_init( void ){
  heap_t *h = (heap_t*)kalloc(HEAP_SIZE,0,0) ;
  chunk_t *free = (chunk_t*)FIRST_CHUNK(h);
  free = (chunk_t *)ALIGN(free);
  h->free = free;
  uint32_t size = HEAP_SIZE - 2*CHUNK_SIZE - sizeof(heap_t);
  h->size = ALIGN(size);
  free->fnext = 0;
  free->fprev = 0;
  free->head = h->size;
  free->prev_foot = 0;
  return h;
}

void *_halloc(heap_t *h, uint32_t size, int p_align){
  chunk_t *free = h->free;
  if( !free ){
    c_puts("not free....\n");
    return 0;
  }
  if( size < sizeof(chunk_t)-CHUNK_SIZE ) {
    size = sizeof(chunk_t)-CHUNK_SIZE;
  }
  while(free){
    if( free->head > size ){
      chunk_t* new = (chunk_t*)((int32_t)free + size + CHUNK_SIZE);
      if(p_align){
        new = (chunk_t*) (((uint32_t)new + 0x1000) - ((uint32_t)new&0xFFF));
        //TODO: MORE?!?
      }
      new->head = size - free->head - CHUNK_SIZE; 
      new->prev_foot = size;
      new->fnext = free->fnext; 
      new->fprev = free->fprev; 
      if(free->fprev){
        free->fprev->fnext = new;
      }if(free->fnext){
        free->fnext->fprev = new;
      }if(free == h->free){
        h->free = new;
        c_printf("h->free %x\n",h->free);
      }
      free->head  = size;
      free->fnext = 0;
      free->fprev = 0;
      c_printf("prev %x, next %x\n", new->fprev, new->fnext);
      return (void*)((uint32_t)free + CHUNK_SIZE);


    }else if (free->head == size){
      c_printf("OHHH IT IS THE SAME SIZE!!\n");
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
      return (void*)((uint32_t)free + CHUNK_SIZE);
    }
    free=free->fnext;
    c_printf("next block...");
   
  }
  return 0;
}

void coalesce(uint32_t new_head, chunk_t** m1, chunk_t** m2, chunk_t** m3){
  (*m1)->head = new_head;
  (*m1)->fnext = (*m2)->fnext;
  if((*m2)->fnext){
    (*m2)->fnext->fprev = (*m1);
  }
  (*m3) = (chunk_t*)((uint32_t)(*m1) + (*m1)->head + CHUNK_SIZE);
  (*m3)->prev_foot = (*m1)->head;
}

void _hfree( void *p, heap_t *h_ptr ){
  heap_t *h = h_ptr;
  chunk_t *chunk = (chunk_t*)((uint32_t)p - (CHUNK_SIZE));
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
    c_printf("hfree, 0x%x : No chunks;\n", h);  
  }else if(chunk < h->free){
    h->free->fprev = chunk;
    chunk->fnext = h->free;
    h->free = chunk;
    chunk->fprev = 0;
    c_printf("hfree, chunk->fnext: %x\n", chunk->fnext);
  }else{
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
      c_printf("hfree, last free chunk\n", h);
      last->fnext = chunk;
      chunk->fprev = free;
      chunk->fnext = 0;
    }
  }
  chunk_t *prev = (chunk_t*)PREV_CHUNK(chunk);
  chunk_t *next = (chunk_t*)NEXT_CHUNK(chunk);
  uint32_t new_head = 0;
  c_puts("about to merge?\n");
  c_printf("next %x %x\n",prev, chunk->fprev);
  c_printf("next %x %x\n",next, chunk->fnext);
  if(prev == chunk->fprev && next == chunk->fnext){
    c_printf("hfree, merge both\n");
    new_head = prev->head + (chunk->head + CHUNK_SIZE) +
      (next->head + CHUNK_SIZE);
    coalesce(new_head, &prev, &next, &next); 
  }else if(next == chunk->fnext){
    c_printf("hfree, merge next\n");
    new_head = chunk->head + next->head + CHUNK_SIZE;
    coalesce(new_head, &chunk, &next, &next);
  }else if(prev == chunk->fprev){
    c_printf("hfree, merge prev\n");
    new_head = prev->head + chunk->head + CHUNK_SIZE;
    coalesce(new_head, &prev, &chunk, &next);
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
chunk->head = //TODO: figure out
chunk->fnext = 0;
chunk->fprev = 0;
chunk->prev_foot = 0;
}
}
}
 */

