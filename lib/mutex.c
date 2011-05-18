#include "mutex.h"

#define SET 0
#define CLEAR 1

bool_t mutex_is_set(mutex_t *mutex) {
	return (*mutex == SET);
}

void mutex_test_and_set(mutex_t *mutex) {
	asm("LOCK:\
	         xchg %%eax, (%%ebx);\
	         and %%eax, %%eax;\
	         jz LOCK;"
	    :
	    : "b"(mutex), "a"(SET)
	   );
}

void mutex_clear(mutex_t *mutex) {
	*mutex = CLEAR;
}

