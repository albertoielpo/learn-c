/**
 * @brief Stack Arena allocator
 *
 * This util provide an arena allocator with the request purpose usage
 * It is implemented as a stack and is preferred simplicity over memory efficient deallocation
 *
 */

// .h
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    size_t capacity; // total capacity (bytes)
    size_t len;      // current occupied capacity (bytes).
    void *start;     // heap allocated start pointer
    void *offset;    // heap offset pointer
} Starena;

// .c

Starena *starena_create(size_t capacity, int init_zero)
{
    Starena *arena = malloc(sizeof(Starena));
    if (arena == NULL)
    {
        perror("[starena_create] out of memory");
        return NULL;
    }
    arena->capacity = capacity;
    if (init_zero)
        arena->start = calloc(capacity, sizeof(uint8_t));
    else
        arena->start = malloc(capacity * sizeof(uint8_t));

    if (arena->start == NULL)
    {
        perror("[starena_create] out of memory");
        return NULL;
    }
    arena->offset = arena->start;
    arena->len = 0;
    return arena;
}

void starena_destroy(Starena *arena)
{
    if (arena == NULL)
        return;
    arena->capacity = 0;
    arena->offset = NULL;
    if (arena->start != NULL)
    {
        free(arena->start);
        arena->start = NULL;
    }
}

void *starena_push(Starena *arena, size_t len)
{
    if (arena == NULL)
    {
        // TODO
        return NULL;
    }

    if (arena->len + len >= arena->capacity)
    {
        // arena resize
        size_t new_cap = arena->capacity * 2;
        void *tmp = realloc(arena->start, new_cap);
        if (tmp == NULL)
        {
            perror("[starena_push] cannot realloc. Old data are still valid");
            return NULL;
        }
        arena->start = tmp;
        arena->capacity = new_cap;
    }
    arena->len += len; // add length
    arena->offset = ((uint8_t *)arena->offset) + len;

    return arena->offset;
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 starena.c
int main(void)
{
    Starena *arena = starena_create(16, 1);
    {
        // I would like to reserve 16 bytes to place some numbers...
        size_t buffer_size = 32;
        uint8_t *my_data = starena_push(arena, buffer_size);
        for (size_t ii = 0; ii < buffer_size; ii++)
        {
            (*(my_data + ii)) = (ii + 57) & 0xFF; // ensure uint8 size
        }

        for (size_t ii = 0; ii < buffer_size; ii++)
        {
            uint8_t *cur = my_data + ii;
            printf("%d %p\n", *cur, cur);
        }
    }

    printf("start: %p offset: %p len: %ld capacity: %ld\n", arena->start, arena->offset, arena->len, arena->capacity);

    starena_destroy(arena);
    arena = NULL;
    return 0;
}