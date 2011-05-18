/*
 *
 * File: paging.h
 *
 * Author: Michael Baril
 *
 * Description: Paging definitions
 *
 */

#ifndef PAGING_H_
#define PAGING_H_

#include "headers.h"
#include "support.h"

#ifndef __ASM__20093__

#define PAGE_SIZE 0x1000

//read the readme of the layout of the pages
typedef struct page {
  uint32_t pres     : 1;
  uint32_t rw       : 1;
  uint32_t user     : 1;
  uint32_t w_thr    : 1; //dont use
  uint32_t cache    : 1; //dont use
  uint32_t acces    : 1; //dont use
  uint32_t dirty    : 1; //dont use
  uint32_t unknown  : 1; //dont use
  uint32_t global   : 1; //dont use
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

} page_dir_t;

/*
 * _paging_init()
 *
 * initializes all paging-related data structures
 */
void _paging_init( void );

/*
 * move_page_dir(page_directory)
 *
 * moves the director into the CR2 reg, starts paging
 */
void move_page_dir(page_dir_t *new); 

/*
 * page_t *get_page(address, make?, page_directory)
 *
 */
page_t *get_page(uint32_t addr, int create, page_dir_t *dir);

/*
 * __page_fault_handler(vector, code)
 *
 * handles page faults (panics)
 */
void __page_fault_handler( int vector, int code );


#endif

#endif
