#include <stdio.h>
#include "../utils/bumparena.h"

/**
 * @brief Read entire file contents into a buffer
 *
 * @param[in] filepath Path to the file to read
 * @param[out] res_file_size Pointer to store the file size (can be NULL)
 * @param[in] area Bump arena pointer
 * @return Allocated buffer containing file contents, or NULL on error
 */
static uint8_t *read_entire_file(const char *filepath, size_t *res_file_size, BumpArena *arena)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("[read_entire_file] fopen");
        return NULL;
    }

    // Get file size
    if (fseek(file, 0, SEEK_END) != 0)
    {
        perror("[read_entire_file] fseek");
        fclose(file);
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size < 0)
    {
        perror("[read_entire_file] ftell");
        fclose(file);
        return NULL;
    }

    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    uint8_t *buffer = bumparena_alloc(arena, file_size + 1);
    if (buffer == NULL)
    {
        perror("[read_entire_file] bumparena_alloc");
        fclose(file);
        return NULL;
    }

    // Read entire file
    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != (size_t)file_size)
    {
        if (feof(file))
            fprintf(stderr, "[read_entire_file] Unexpected EOF\n");
        else if (ferror(file))
            perror("[read_entire_file] fread");

        fclose(file);
        return NULL;
    }

    buffer[file_size] = 0;

    fclose(file);

    if (res_file_size != NULL)
        *res_file_size = file_size;

    return buffer;
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 ../utils/bumparena.c how-arena.c
int main(void)
{
    BumpArena *arena = bumparena_create(1024);
    {
        // I would like to reserve some bytes to place some numbers...
        size_t buffer_size = 192;

        uint8_t *my_data = bumparena_alloc(arena, buffer_size);
        for (size_t ii = 0; ii < buffer_size; ii++)
        {
            (*(my_data + ii)) = (ii + 57) & 0xFF; // ensure uint8 size
        }
    }

    // here I would like to read some files
    {
        size_t file_size = 0;
        char *buffer = (char *)read_entire_file("how-arena.c", &file_size, arena);
        if (buffer == NULL)
        {
            bumparena_destroy(arena);
            return 1;
        }
        printf("%s \n", buffer);
        printf("File size: %ld bytes\n", file_size);
    }

    printf("start: %p offset: %p len: %ld capacity: %ld\n", arena->start, arena->offset, arena->len, arena->capacity);

    bumparena_destroy(arena);
    arena = NULL;
    return 0;
}