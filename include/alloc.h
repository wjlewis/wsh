#ifndef __ALLOC_H
#define __ALLOC_H

#include <stdlib.h>

void *arena_alloc(size_t size);

void *arena_realloc(void *ptr, size_t old_size, size_t new_size);

void reset_arena(void);

#endif
