#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <stddef.h>

typedef struct ArenaAllocator ArenaAllocator;

ArenaAllocator* create_arena(size_t size);
void* arena_alloc(ArenaAllocator* arena, size_t size);
void reset_arena(ArenaAllocator* arena);
void destroy_arena(ArenaAllocator* arena);

#endif // ARENA_ALLOCATOR_H
