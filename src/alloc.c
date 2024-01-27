#include <string.h>
#include <stdlib.h>
#include "alloc.h"

#define ARENA_SIZE 4096

static char arena_data[ARENA_SIZE];

static int arena_offset = 0;

void *arena_alloc(size_t size) {
        if (arena_offset + size >= ARENA_SIZE) {
                return NULL;
        }

        void *ptr = (void *) (arena_data + arena_offset);
        arena_offset += size;
        return ptr;
}

void *arena_realloc(void *ptr, size_t old_size, size_t new_size) {
        void *new_ptr = arena_alloc(new_size);
        if (!new_ptr) {
                return NULL;
        }
        memcpy(new_ptr, ptr, old_size);
        return new_ptr;
}

void reset_arena(void) {
        memset(arena_data, 0, ARENA_SIZE);
        arena_offset = 0;
}
