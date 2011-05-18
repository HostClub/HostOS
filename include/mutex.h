#ifndef MUTEX_H
#define MUTEX_h

#include "types.h"

typedef int mutex_t;

bool_t mutex_is_set(mutex_t *mutex);
void mutex_test_and_set(mutex_t *mutex);
void mutex_clear(mutex_t *mutex);

#endif

