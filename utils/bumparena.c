/**
 * @brief Append-only Arena Allocator
 *
 * BumpArena allocator manages a contiguous block of memory for fast allocations.
 * It is optimized for short-lived data that can be freed all at once, such as
 * request-scoped allocations or temporary data structures. The arena grows
 * automatically as needed but never shrinks, prioritizing simplicity and
 * performance over memory efficiency.
 */

// .h
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    size_t capacity; // total capacity (bytes)
    size_t len;      // current occupied capacity (bytes).
    uint8_t *start;  // heap allocated start pointer
    uint8_t *offset; // heap offset pointer
} BumpArena;

// .c

BumpArena *bumparena_create(size_t capacity)
{
    BumpArena *arena = malloc(sizeof(BumpArena));
    if (arena == NULL)
    {
        perror("[bumparena_create] out of memory");
        return NULL;
    }
    arena->capacity = capacity;
    arena->start = malloc(capacity * sizeof(uint8_t));

    if (arena->start == NULL)
    {
        perror("[bumparena_create] out of memory");
        return NULL;
    }
    arena->offset = arena->start;
    arena->len = 0;
    return arena;
}

void bumparena_destroy(BumpArena *arena)
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
    free(arena);
}

static int bumparena_grow(BumpArena *arena)
{
    size_t new_cap = arena->capacity * 2;
    uint8_t *tmp = realloc(arena->start, new_cap);
    if (tmp == NULL)
    {
        perror("[bumparena_push] cannot realloc. Old data are still valid");
        return 0;
    }
    arena->start = tmp;
    // recalculate the offset because start can be changed
    arena->offset = (arena->start) + arena->len;
    arena->capacity = new_cap;
    return 1;
}

/**
 * @brief Push element into the arena
 *
 * Push element into the arena stack.
 *
 * @param[in] arena
 * @param[in] len in bytes
 * @return Return a void* because it's caller responsability to use it properly
 */
void *bumparena_push(BumpArena *arena, size_t len)
{
    if (arena == NULL)
        return NULL;

    // arena grows until the len is inside the capacity
    while (arena->len + len >= arena->capacity)
    {
        if (!bumparena_grow(arena))
            return NULL;
    }

    // result is the pointer where the caller can write data
    uint8_t *result = arena->offset;
    arena->len += len;
    arena->offset = (arena->offset) + len; // then increment

    return (void *)result;
}

/**
 * TODO: save something of this method in one util
 * @brief Read entire file contents into a buffer
 *
 * @param filepath Path to the file to read
 * @param out_size Pointer to store the file size (can be NULL)
 * @return Allocated buffer containing file contents, or NULL on error
 *         Caller must free() the returned buffer
 */
static uint8_t *read_entire_file(const char *filepath, size_t *out_size, BumpArena *arena)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("fopen");
        return NULL;
    }

    // Get file size
    if (fseek(file, 0, SEEK_END) != 0)
    {
        perror("fseek");
        fclose(file);
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size < 0)
    {
        perror("ftell");
        fclose(file);
        return NULL;
    }

    rewind(file); // Or: fseek(file, 0, SEEK_SET);

    // Allocate buffer
    uint8_t *buffer = bumparena_push(arena, file_size + 1);
    if (buffer == NULL)
    {
        perror("bumparena_push");
        fclose(file);
        return NULL;
    }

    // Read entire file
    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != (size_t)file_size)
    {
        if (feof(file))
            fprintf(stderr, "Unexpected EOF\n");
        else if (ferror(file))
            perror("fread");

        fclose(file);
        return NULL;
    }

    buffer[file_size] = 0;

    fclose(file);

    if (out_size != NULL)
        *out_size = file_size;

    return buffer;
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 bumparena.c
int main(void)
{
    BumpArena *arena = bumparena_create(1024);
    {
        // I would like to reserve 16 bytes to place some numbers...
        size_t buffer_size = 192;

        uint8_t *my_data = bumparena_push(arena, buffer_size);
        for (size_t ii = 0; ii < buffer_size; ii++)
        {
            (*(my_data + ii)) = (ii + 57) & 0xFF; // ensure uint8 size
        }
    }

    // here I would like to read some files
    {
        size_t file_size = 0;
        char *buffer = (char *)read_entire_file("alist.c", &file_size, arena);
        if (buffer == NULL)
        {
            return 1;
        }
        printf("%s \n", buffer);
        printf("Out size: %ld\n", file_size);
    }

    printf("start: %p offset: %p len: %ld capacity: %ld\n", arena->start, arena->offset, arena->len, arena->capacity);

    bumparena_destroy(arena);
    arena = NULL;
    return 0;
}