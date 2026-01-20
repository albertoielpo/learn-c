#include "bumparena.h"
#include <stdio.h>

/** @copydoc bumparena_create */
BumpArena *bumparena_create(size_t capacity) {
    BumpArena *arena = malloc(sizeof(BumpArena));
    if (arena == NULL) {
        perror("[bumparena_create] out of memory");
        return NULL;
    }
    arena->capacity = capacity;
    arena->start = malloc(capacity * sizeof(uint8_t));

    if (arena->start == NULL) {
        perror("[bumparena_create] out of memory");
        return NULL;
    }
    arena->offset = arena->start;
    arena->len = 0;
    return arena;
}

/** @copydoc bumparena_destroy */
void bumparena_destroy(BumpArena *arena) {
    if (arena == NULL)
        return;
    arena->capacity = 0;
    arena->offset = NULL;
    if (arena->start != NULL) {
        free(arena->start);
        arena->start = NULL;
    }
    free(arena);
}

/**
 * @brief Resize arena
 *
 * Increase the arena capacity by *2 factor
 *
 * @param[in] arena BumpArena pointer
 * @return 1 if good, 0 in case of error
 */
static int bumparena_grow(BumpArena *arena) {
    size_t new_cap = arena->capacity * 2;
    uint8_t *tmp = realloc(arena->start, new_cap);
    if (tmp == NULL) {
        perror("[bumparena_alloc] cannot realloc. Old data are still valid");
        return 0;
    }
    arena->start = tmp;
    // recalculate the offset because start can be changed
    arena->offset = arena->start + arena->len;
    arena->capacity = new_cap;
    return 1;
}

/** @copydoc bumparena_alloc */
void *bumparena_alloc(BumpArena *arena, size_t len) {
    if (arena == NULL)
        return NULL;

    // arena grows until the len is inside the capacity
    while (arena->len + len >= arena->capacity) {
        if (!bumparena_grow(arena))
            return NULL;
    }

    // result is the pointer where the caller can write data
    uint8_t *result = arena->offset;
    arena->len += len;
    arena->offset = arena->offset + len; // then increment

    return (void *)result;
}