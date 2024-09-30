#include "arena_allocator.h"
#include <stdlib.h>

struct ArenaAllocator {
    char* buffer;
    size_t size;
    size_t used;
};

ArenaAllocator* create_arena(size_t size) {
    ArenaAllocator* arena = malloc(sizeof(ArenaAllocator));
    if (!arena) return NULL;

    arena->buffer = malloc(size);
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }

    arena->size = size;
    arena->used = 0;
    return arena;
}

void* arena_alloc(ArenaAllocator* arena, size_t size) {
    if (arena->used + size > arena->size) {
        return NULL;  // Out of memory
    }

    void* ptr = arena->buffer + arena->used;
    arena->used += size;
    return ptr;
}

void reset_arena(ArenaAllocator* arena) {
    arena->used = 0;
}

void destroy_arena(ArenaAllocator* arena) {
    free(arena->buffer);
    free(arena);
}
