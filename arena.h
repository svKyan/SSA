#ifndef ARENA_H_
#define ARENA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// It cannot access __arena and some helper functions, only these

typedef struct {
    size_t location; // Relative
    size_t len;
} ArenaNode;

typedef struct {
    void* begin;
    ArenaNode* nodes;
    size_t nodelen;
    size_t size;
} Arena;

Arena make_arena(size_t len);
void free_arena(Arena* arena);
void print_arena(Arena* arena);
void afree(Arena* arena, void* loc);
void* aalloc(Arena* arena, size_t len);
void* arealloc(Arena* arena, void* loc, size_t len);

void init_global_arena(size_t len);
void gprint_arena();
void gfree(void* loc);
void* galloc(size_t len);
void* grealloc(void* loc, size_t len);
void gdelete();

#endif // ARENA_H_
