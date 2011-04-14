#include <paging.h>

// 4gb?
#define PHYS_MEM_SIZE = 0x100000000
#define PAGE_SIZE = 0x1000

void init_paging(){
  total_frames = PHYS_MEM_SIZE / PAGE_SIZE;
  //frames = some kmalloc thing I have to implement
  //#do I have memset?  Could use it here

  //dir = (page_dir_t*) kmallac something I should implement
  //set up map
  //for(int i = 0; i < //something in mem I should implement; i+=PAGE_SIZE){
   
    //allocate a frame ( get a page (i, 1, dir), other info);
  //}

  //need a page fault handler

  //enable
}

void change_page_dir(page_dir_t *new){
  // need to load in the CR3 table, probably use some
  // inline asm calls
} 

page_t *get_page(uint32_t addr, int create, page_dir_t *dir){
  //the table of the addr
  uint32_t table = (addr / PAGE_SIZE) / 1024;
  
  // if it exists then return the directory

  // do I need to create a new one?
     //use kmallac again to allocate a new table
     //CAN I HAZ MEMSET
     //set the correct flags in the tables
     // return it

  //otherwise
    //error?

}
