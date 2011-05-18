/*
 * File: framefunc.h
 *
 * Author: Michael Baril
 *
 * Description: bitmap functions for page frames
 *
 */
#ifndef FRAME_FUNC_H
#define FRAME_FUNC_H

#include "headers.h"
#include "paging.h"

#ifndef __ASM__20093__

/*
** Uses a bitmap to allocate and map new frames
** Uses basic bit twiddling operations to set, clear, and test bits
*/ 

/*
 * falloc(page, write, kernal)
 *
 * allocates a frame with the settings given.
 *
 */
void falloc(page_t *p, int write, int kernel);

/*
 * ffree(page)
 *
 * frees a frame with the settings given.
 */
void ffree(page_t *p);

#endif

#endif
