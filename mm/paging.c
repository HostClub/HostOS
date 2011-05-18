/*
 * File: paging.c
 *
 * Author: Michael Baril
 *
 * Contributor: Osdev.net (move_page_table)
 *
 * Description: Enables paging and sets everything up correctly
 *
 */

#define __KERNEL__20103__

#include "paging.h"
#include "framefunc.h"
#include "mm.h"
#include "support.h"
#include "../drivers/USB/usb_ehci_defs.h"

// 4gb?
//#define PHYS_MEM_SIZE 0x100000000
// 16MB
#define PHYS_MEM_SIZE 0x1000000
#define TABLE_SIZE 1024
#define SIZE 32
#define OFFSET(a) (a%SIZE)
#define INDEX(a) (a/SIZE)
#define INT_PAGE_FAULT 14
#define DEFAULT_BITS 0x7
#define PAGE_ENABLE 0x80000000
#define PAGE_PRESENT(a) (a&0x1)
#define PAGE_RW(a) (a&0x2)
#define PAGE_USER(a) (a&0x4)
#define USB_MEM  0xE0904000
//TODO: More paging errors

#define PAGE_DEBUG

extern uint32_t alloc_addr; //the placement alloc address
extern uint32_t* frame_loc;
extern uint32_t total_frames;
page_dir_t *base_dir = 0;
extern chunk_mngr_t* base_mngr;

void _paging_init(){
  total_frames = PHYS_MEM_SIZE / PAGE_SIZE;

  frame_loc = (uint32_t*)kalloc(INDEX(total_frames), 0, 0);
  _memclr(frame_loc, INDEX(total_frames));

  //page_dir_t *base_dir = (page_dir_t*)kalloc(sizeof(page_dir_t), 0, 1);
  base_dir = (page_dir_t*)kalloc(sizeof(page_dir_t), 0, 1);
  _memclr(base_dir, sizeof(page_dir_t));
  //base_dir->phys_addr = (uint32_t)base_dir->phys_tables;

  //map pages to the kernel mngr
  int i = 0;
  for(i=MEM_START; i < MEM_START + MNGR_SIZE; i += PAGE_SIZE){
    get_page(i, 1, base_dir);
  }

  //identity map the memory
  i=0;
  while( i < alloc_addr + PAGE_SIZE) {
    falloc( get_page (i, 1, base_dir), 0, 0);
    i+=PAGE_SIZE;
  }

  //allocate kernel mngr
  for(i=MEM_START; i < MEM_START + MNGR_SIZE; i += PAGE_SIZE){
    falloc(get_page (i, 1, base_dir), 0, 0);
  }

  __install_isr( INT_PAGE_FAULT, __page_fault_handler);

  //start
  move_page_dir(base_dir);

  //start mngr
  base_mngr = _mngr_init();
}

//basically got this code from the osdev wiki on paging
//wiki.osdev.org/Setting_Up_Paging
void move_page_dir(page_dir_t *new){
  asm volatile("mov %0, %%cr3":: "b"(&new->phys_tables));
  uint32_t cr0;
  asm volatile("mov %%cr0, %0": "=b"(cr0));
  cr0 |= PAGE_ENABLE;
  asm volatile("mov %0, %%cr0":: "b"(cr0));
} 

page_t *get_page(uint32_t addr, int create, page_dir_t *dir){
  addr /= PAGE_SIZE;
  //the table of the addr
  uint32_t table = addr / TABLE_SIZE;
  uint32_t page = addr % TABLE_SIZE;
  
  // if it exists then return the directory
  if(dir->tables[table]){
    return &dir->tables[table]->p[page];
  }else if(create){
    uint32_t phys;
    dir->tables[table] = (page_table_t*)kalloc(sizeof(page_table_t), &phys, 1);
    _memclr(dir->tables[table], PAGE_SIZE);
    dir->phys_tables[table] = phys | 0x7;
    return &dir->tables[table]->p[page];
  }
    return 0;
}

void __page_fault_handler( int vector, int code ){

  //also got this from osdev wiki
  uint32_t addr;
  asm volatile("mov %%cr2, %0" : "=b" (addr));
  
  c_printf("Page fault : ");
  if(PAGE_PRESENT(addr)) {
    c_printf("present ");
  }
  if(PAGE_RW(addr)){
    c_printf("read only ");
  }
  if(PAGE_USER(addr)){
    c_printf("user mode ");
  }

  /*
  //falloc( get_page(*_USBCMD ,1,base_dir), 1, 1);
  if(code == 0){
    page_t* page = get_page(addr,1,base_dir);
    c_printf("\npage: %x\n",page);
    falloc(page, 0, 0);
    c_printf("before I return\n");
    return;
  }
    
  

  if(addr >= USB_MEM){
    c_puts("in USB mem\n");


    return;
  }
  */
  c_printf("\n 0x%x   code: %d\n", addr, code);

  _kpanic("__page_fault_handler", "Page fault :(", 0);
}

