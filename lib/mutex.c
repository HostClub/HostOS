#include "mutex.h"

#define SET 0
#define CLEAR 1

bool_t is_set(mutex_t *mutex) {
	return (*mutex == SET);
}

void test_and_set(mutex_t *mutex) {
	asm("LOCK:\
	         xor %%eax, %%eax;\
	         xchg %%eax, (%%ebx);\
	         and %%eax, %%eax;\
	         jz LOCK;"
	    : "=b"(mutex)
	    :
		: "%eax"
	   );
}

void clear(mutex_t *mutex) {
	*mutex = CLEAR;
}

