#ifndef PAGING_H_
#define PAGING_H_

// need includes

//read the readme of the layout of the pages
typedef struct page {
  uint32_t pres     : 1;
  uint32_t rw       : 1;
  uint32_t user     : 1;
  uint32_t w-thr    : 1;
  uint32_t cache    : 1;
  uint32_t acces    : 1;
  uint32_t dirty    : 1;
  uint32_t unknown  : 1;
  uint32_t global   : 1;
  uint32_t avail    : 3;
  uint32_t frame    : 20;
} page_t;


typedef struct page_table {
  page_t p[1024];
} page_table_t;

typedef struct page_dir {
  
  //pointers t pagetables
  page_table_t *tables[1024];

  //pointers to the page tables, gives phys address
  uint32_t     phys_tables[1024];

  //physical address of the phys_tables.
  //TODO:more
  uint32_t     phys_addr
} page_dir_t;

//init routine for paging
void init_paging();

//loads the page directory into the CR3 reg
void change_page_dir(page_dir_t *new); 

//gets a pointer to the required page
//TODO:more
page_t *get_page(uint32_t addr, int create, page_dir_t *dir);


//TODO:page fault handler

#endif
