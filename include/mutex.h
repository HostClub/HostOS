#ifndef MUTEX_H
#define MUTEX_h

#include "types.h"

typedef int mutex_t;

bool_t is_set(mutex_t *mutex);
void test_and_set(mutex_t *mutex);
void clear(mutex_t *mutex);

#endif

