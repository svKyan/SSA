#include "arena.h"

Arena make_arena(size_t len) {
    Arena arena = { malloc(len), NULL, 0, len };
    if (arena.begin == NULL) {
        fprintf(stderr, "Could not allocate memory");
        exit(1);
    }
    return arena;
}

void free_arena(Arena* arena) {
    free(arena->begin);
    free(arena->nodes);
    arena->begin = NULL;
    arena->nodes = NULL;
    arena->nodelen = 0;
    arena->size = 0;
}

void print_arena(Arena* arena) {
    printf("Arena of size %lu at %p\n", arena->size, arena->begin);
    for (int i = 0; i < arena->nodelen; ++i)
        printf("Allocation at %p of size %lu\n", arena->begin + arena->nodes[i].location, arena->nodes[i].len);
}

size_t get_new_node_location(Arena* arena, size_t loc) {
    for (int i = 0; i < arena->nodelen; ++i)
        if (arena->nodes[i].location > loc)
            return i;
    return arena->nodelen;
}

void add_node_in_place(Arena* arena, size_t loc, size_t len) {
    if (arena == NULL) {
        fprintf(stderr, "Arena is null");
        exit(1);
    }

    ArenaNode new_node = { loc, len };
    if (arena->nodelen == 0) {
        arena->nodes = malloc(sizeof(*arena->nodes));
        if (arena->nodes == NULL) {
            fprintf(stderr, "Could not allocate memory");
            exit(1);
        }
        arena->nodelen = 1;
        arena->nodes[0] = new_node;
        return;
    }

    ArenaNode* temp = realloc(arena->nodes, (arena->nodelen + 1) * sizeof(*arena->nodes)); 
    if (temp == NULL) {
        fprintf(stderr, "Could not allocate memory");
        exit(1);
    }
    arena->nodes = temp;
    size_t newloc = get_new_node_location(arena, loc);
    for (int i = arena->nodelen; i > newloc; --i)
        arena->nodes[i] = arena->nodes[i - 1];
    arena->nodes[newloc] = new_node;
    arena->nodelen++;
}

void afree(Arena* arena, void* loc) {
    if (arena == NULL) {
        fprintf(stderr, "Arena is null");
        exit(1);
    }
    if (arena->nodelen == 0) {
        fprintf(stderr, "Arena at %p with size %lu has no nodes", arena->begin, arena->size);
        exit(1);
    }

    if (arena->nodelen == 1) {
        free(arena->nodes);
        arena->nodes = NULL;
        arena->nodelen = 0;
        return;
    }

    int oldloc = -1;
    for (int i = 0; i < arena->nodelen; ++i)
        if (arena->nodes[i].location == loc - arena->begin) {
            oldloc = i;
            break;
        }
    if (oldloc == -1) {
        fprintf(stderr, "Node is not in arena");
        exit(1);
    }
    for (int i = oldloc; i < arena->nodelen - 1; ++i)
        arena->nodes[i] = arena->nodes[i + 1];
    
    ArenaNode* temp = realloc(arena->nodes, (arena->nodelen - 1) * sizeof(*arena->nodes));
    if (temp == NULL) {
        fprintf(stderr, "Could not allocate memory");
        exit(1);
    }
    arena->nodes = temp;
    arena->nodelen--;
}

bool arena_fits(Arena* arena, size_t loc, size_t len) {
    if (loc == 0 && len < arena->nodes[0].location)
        return true;
    for (int i = 0; i < arena->nodelen - 1; ++i)
        if (arena->nodes[i].location + arena->nodes[i].len <= loc && loc + len <= arena->nodes[i+1].location)
            return true;
    return arena->nodes[arena->nodelen - 1].location + arena->nodes[arena->nodelen - 1].len <= len;
}

void* aalloc(Arena* arena, size_t len) {
    size_t loc = 0;
    for (int i = 0; i < arena->nodelen; ++i) {
        if (arena_fits(arena, loc, len))
            break;
        loc = arena->nodes[i].location + arena->nodes[i].len;
    }
    if (loc + len > arena->size) {
        fprintf(stderr, "Arena at %p ran out of space. tried to allocate %lu bytes, but there were only %lu left\n", arena->begin, len, arena->size - loc);
        exit(1);
    }
    add_node_in_place(arena, loc, len);
    return arena->begin + loc; 
}

void* arealloc(Arena* arena, void* loc, size_t len) {
    if (!(arena->begin <= loc && loc <= arena->begin + arena->size)) {
        fprintf(stderr, "Pointer outside of given arena");
        exit(1);
    }
    afree(arena, loc);
    void* new_loc = aalloc(arena, len);
    if (new_loc != loc)
        memcpy(new_loc, loc, len);
    return new_loc;
}

static Arena __arena;

void init_global_arena(size_t len) {
    __arena = make_arena(len);
}

void gprint_arena() {
    print_arena(&__arena);
}

void gfree(void* loc) {
    afree(&__arena, loc);
}

void* galloc(size_t len) {
    return aalloc(&__arena, len);
}

void* grealloc(void* loc, size_t len) {
    return arealloc(&__arena, loc, len);
}

void gdelete() {
    free_arena(&__arena);
}
