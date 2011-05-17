#define __KERNEL__20103__

#include "framefunc.h"
#include "paging.h"

uint32_t* frame_loc;
uint32_t total_frames;

#define SIZE 32 //change depending on architecture
#define OFFSET(a) (a%SIZE)
#define INDEX(a) (a/SIZE)
#define FRAME(a) (a/0x1000) //frames aligned on 4KB bounds

static uint32_t find_frame( void ){
  int x, y;
  for(x = 0; x < INDEX(total_frames); x++){
    //if there is no space, exit
    if(frame_loc[x] != 0xFFFFFFFF){
      for(y = 0; y < SIZE; y++){
        //return when find the first bit free
          if( !(frame_loc[x] & (0x1 << y)) ){
            return (x*SIZE+y);
        }
      }
    }
  }
}
/*
static uint32_t test_bit(uint32_t addr, uint32_t *frames){
  //derp test the bit to see if it is set
  return (frames[INDEX(FRAME(addr))] & (0x1 << OFFSET(FRAME(addr))));
}
*/
static void set_bit(uint32_t addr){
  //derp set a bit
  frame_loc[INDEX(FRAME(addr))] |= (0x1 << OFFSET(FRAME(addr)));
}

static void clear_bit (uint32_t addr){
  //derp clear a bit
  frame_loc[INDEX(FRAME(addr))] &= ~(0x1 << OFFSET(FRAME(addr)));
}

//TODO: ADD MORE OPTIONS
void falloc(page_t *p, int write, int kernel){
  //if page is already allocated, function will just return
  //TODO: Add error?
  if(p->frame != 0){
    return;
  }else{
    uint32_t frame = find_frame();
    //TODO: if no free frames, kernel panic
    set_bit(frame * 0x1000);
    p->pres = 1;
    p->rw = write?1:0;
    p->user = kernel?0:1;
    p->frame = frame;
    //TODO: more options
  }

}

void ffree(page_t *p){
  //if it is 0 no frame was allocated
  if(p->frame){
    clear_bit(p->frame);
    p->frame = 0;
  }
}
