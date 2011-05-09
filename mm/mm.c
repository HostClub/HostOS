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

extern uint32_t _end;
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
