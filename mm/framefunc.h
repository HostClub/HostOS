#ifndef FRAME_FUNC_H
#define FRAME_FUNC_H

/*
** Uses a bitmap to allocate and map new frames
** Uses basic bit twiddling operations to set, clear, and test bits
*/ 

//checks bitmap to find the first frame
static uint32_t find_frame(uint32_t *frames, uint32_t space){

//basic function to test a bit
static uint32_t test_bit(uint32_t addr, uint32_t *frames);

//basic function to set a bit
static void set_bit(uint32_t addr, uint32_t **frames);

//basic function to clear a bit
static void clear_bit(uint32_t addr, uint32_t **frames);

//allocates frame
void falloc(page_t *p, uint32_t *f, uint32_t nf, int write, int kernel);

//frees a frame
void ffree(page_t *p, uint32_t *f);

#endif
