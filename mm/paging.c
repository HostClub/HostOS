#define __KERNEL__20103__

#include "paging.h"
#include "framefunc.h"
#include "mm.h"
#include "support.h"

// 4gb?
//#define PHYS_MEM_SIZE 0x100000000
// 16MB
#define PHYS_MEM_SIZE 0x1000000
#define TABLE_SIZE 1024
#define SIZE 32
#define OFFSET(a) (a%SIZE)
#define INDEX(a) (a/SIZE)
#define INT_PAGE_FAULT 14
#define PAGE_ENABLE 0x80000000
#define PAGE_PRESENT(a) (a&0x1)
#define PAGE_RW(a) (a&0x2)
#define PAGE_USER(a) (a&0x4)
//TODO: More paging errors

#define PAGE_DEBUG

extern uint32_t alloc_addr;
extern uint32_t* frame_loc;
extern uint32_t total_frames;
page_dir_t *base_dir = 0;
page_dir_t *cur_dir = 0;
extern heap_t* base_heap;

void _paging_init(){
  c_puts("paging ");
  total_frames = PHYS_MEM_SIZE / PAGE_SIZE;

  c_puts("\ntrying to get frame loc... in kalloc\n");


  frame_loc = (uint32_t*)kalloc(INDEX(total_frames), 0, 0);
  memset(frame_loc, 0, INDEX(total_frames));

  c_printf("frames %d\n", frame_loc);

  c_puts("trying to get mem for the page dir\n");

  //page_dir_t *base_dir = (page_dir_t*)kalloc(sizeof(page_dir_t), 0, 1);
  base_dir = (page_dir_t*)kalloc(sizeof(page_dir_t), 0, 1);
  memset(base_dir, 0, sizeof(page_dir_t));
  //base_dir->phys_addr = (uint32_t)base_dir->phys_tables;
  cur_dir = base_dir;
  c_printf("basedir %d\n", base_dir);

  int i = 0;
  for(i=MEM_START; i < MEM_START + HEAP_SIZE; i += PAGE_SIZE){
    get_page(i, 1, base_dir);
  }

  i=0;
  c_puts("loop fllocing the pages\n");
  while( i < alloc_addr + PAGE_SIZE) {
    falloc( get_page (i, 1, base_dir), 0, 0);
    i+=PAGE_SIZE;
  }

  for(i=MEM_START; i < MEM_START + HEAP_SIZE; i += PAGE_SIZE){
    falloc(get_page (i, 1, base_dir), 0, 0);
  }

  //TODO: page fault  turn on intterupt
  c_puts("install the isr...\n");
  __install_isr( INT_PAGE_FAULT, __page_fault_handler);

  c_puts("turn on paging...\n");
  c_printf("basedir %d\n", base_dir);
  move_page_dir(base_dir);

  c_puts("GOOO...?\n");

  base_heap = _heap_init();
}

//basically got this code from the osdev wiki on paging
//wiki.osdev.org/Setting_Up_Paging
void move_page_dir(page_dir_t *new){
  c_puts("Cross yo fingerz\n");
  cur_dir = new;
  asm volatile("mov %0, %%cr3":: "b"(&new->phys_tables));
  uint32_t cr0;
  c_puts("Hold.....\n");
  asm volatile("mov %%cr0, %0": "=b"(cr0));
  cr0 |= PAGE_ENABLE;
  c_puts("Hold.................\n");
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
    uint32_t tmp;
    dir->tables[table] = (page_table_t*)kalloc(sizeof(page_table_t), &tmp, 1);
    memset(dir->tables[table], 0, PAGE_SIZE);
    dir->phys_tables[table] = tmp | 0x7; //TODO: WHAT BITS???!?!!
    return &dir->tables[table]->p[page];
  }else {
    return 0;
  }
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
  c_printf("\n 0x%x\n", addr);

  _kpanic("__page_fault_handler", "Page fault :(", 0);
}

void memset( void * loc, int value, int n){
  char * byte_loc = loc;

  int i;
  for(i=0; i< n; i++, byte_loc++){
    *byte_loc = value;
  }
  return loc;
}
