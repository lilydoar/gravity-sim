#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <stddef.h>

typedef struct ArenaAllocator ArenaAllocator;

ArenaAllocator *init_arena(size_t size);
void deinit_arena(ArenaAllocator *arena);
void *arena_alloc(ArenaAllocator *arena, size_t size);
void reset_arena(ArenaAllocator *arena);

#endif // ARENA_ALLOCATOR_H
