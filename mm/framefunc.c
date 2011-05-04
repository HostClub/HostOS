#include "framefunc.h"

#define SIZE 32 //change depending on architecture
#define OFFSET (a%SIZE)
#define INDEX (a/SIZE)
#define FRAME (a/0x1000) //frames aligned on 4KB bounds

static uint32_t find_frame(uint32_t *frames, uint32_t space){
  int x, y;
  for(int x = 0; x < INDEX(space); x++){
    //if there is no space, exit
    if(frames[x] != NULL){
      for(y = 0, y < SIZE; y++){
        //return when find the first bit free
        if( !(frames[i] & (0x1 << y)) ){
          return x*SIZE+y
        }
      }
    }
  }
}

static uint32_t test_bit(uint32_t addr, uint32_t *frames){
  //derp test the bit to see if it is set
  return (frames[INDEX(FRAME(f))] & (0x1 << OFFSET(FRAME(f))));
}

static void set_bit(uint32_t addr, uint32_t **frames){
  //derp set a bit
  *frames[INDEX(FRAME(f))] |= (0x1 << OFFSET(FRAME(f)));
}

static void clear_bit (uint32_t addr, uint32_t **frames){
  //derp clear a bit
  *frames[INDEX(FRAME(f))] &= ~(0x1 << OFFSET(FRAME(f)));
}

//TODO: ADD MORE OPTIONS
void falloc(page_t *p, uint32_t *f, uint32_t nf, int write, int kernel){
  //if page is already allocated, function will just return
  //TODO: Add error?
  if(!p->frame){
    uint32_t frame = find_frame(f, nf);
    //TODO: if no free frames, kernel panic
    set_bit(frame * 0x1000, *f);
    p->pres = 1;
    p->rw = write;
    p->user = kernel;
    p->frame = frame;
    //TODO: more options
  }

}

void ffree(page_t *p, uint32_t *f){
  //if it is 0 no frame was allocated
  if(page->frame){
    clear_bit(page->frame, *f)
    page->frame = 0;
  }
}
