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

/**
 * @brief Create a bump arena
 * This method reserve an heap space for a bump arena
 * @param[in] capacity Reserved bytes
 * @return Bump Arena pointer
 */
BumpArena *bumparena_create(size_t capacity);

/**
 * @brief Deallocate bump arena
 * This method free from heap the bump arena reserved
 * @param[in] arena
 */
void bumparena_destroy(BumpArena *arena);

/**
 * @brief Alloc element into the arena
 * Alloc element into the arena stack.
 * @param[in] arena
 * @param[in] len in bytes
 * @return Return a void* because it's caller responsability to use it properly
 */
void *bumparena_alloc(BumpArena *arena, size_t len);

#endif