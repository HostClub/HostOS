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

extern int _end;
uint32_t alloc_addr = (uint32_t)&_end;

uint32_t kalloc(uint32_t size, uint32_t *phys_addr, uint32_t align){
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

}

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

void *_kalloc(void *v, uint32_t size, int p_align){
  heap_t *h = (heap_t *)v;
  chunk_t *free = h->free;
  //TODO: Stuff
  while(free){
    if( free->size > size ){
      chunk_t* new = (chunk_t*)((int32_t)free + size + sizeof(chunk_t));
      uint32_t head = free->size - size - sizeof(chunk_t); 
      uint32_t prev_foot = size;
      chunk_t* fnext = free->fnext; 
      chunk_t* fprev = free->fprev; 
      if(free->fprev){
        free->fprev->fnext = new;
      }if(free->fnext){
        free->fnext->fprebv = new;
      }if(free == h->free){
        h->free = new;
      }
      free->head  = size;
      free->fnext = 0;
      free->fprev = 0;
      return (void*)((uint32_t)free + sizeof(chunk_t));




