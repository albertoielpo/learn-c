/**
 * @brief Append-only Arena Allocator
 *
 * BumpArena allocator manages a contiguous block of memory for fast allocations.
 * It is optimized for short-lived data that can be freed all at once, such as
 * request-scoped allocations or temporary data structures. The arena grows
 * automatically as needed but never shrinks, prioritizing simplicity and
 * performance over memory efficiency.
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */

#ifndef BUMPARENA_H
#define BUMPARENA_H
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    size_t capacity; // total capacity (bytes)
    size_t len;      // current occupied capacity (bytes).
    uint8_t *start;  // heap allocated start pointer
    uint8_t *offset; // heap offset pointer
} BumpArena;

BumpArena *bumparena_create(size_t capacity);
void bumparena_destroy(BumpArena *arena);
void *bumparena_alloc(BumpArena *arena, size_t len);

#endif