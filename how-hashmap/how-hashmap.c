#define _POSIX_C_SOURCE 199309L
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include "../utils/hmap.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * @brief Generate a random key string
 *
 * @param buffer Output buffer for the key
 * @param length Length of the key to generate
 */
void generate_random_key(char *buffer, size_t length) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (size_t i = 0; i < length - 1; i++) {
        buffer[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    buffer[length - 1] = '\0';
}

/**
 * @brief Get elapsed time in milliseconds
 */
double get_elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1000000.0;
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 ../utils/hmap.c how-hashmap.c
// valgrind ./a.out
int main(void) {
    srand((unsigned int)time(NULL)); // seed

    const size_t test_size = 1000000;
    const size_t test_idx = rand() % test_size;
    printf("test size %ld, lookup index %ld\n", test_size, test_idx);
    printf("==================================================\n");

    char *key_ref[test_size]; // pointer array
    HMap *map = hmap_create(16);

    for (size_t ii = 0; ii < test_size; ii++) {
        char *key = malloc(sizeof(char) * 32);
        key_ref[ii] = key;
        generate_random_key(key, 32);            // 32 chars rnd key
        hmap_add(map, key, key, HE_TYPE_STR, 1); // add same key and value for this test
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    hmap_print(hmap_get(map, key_ref[test_idx]));
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = get_elapsed_ms(start, end);
    printf(ANSI_COLOR_GREEN);
    printf("Time taken hmap: %.2f ms\n", elapsed);
    printf(ANSI_COLOR_RESET);

    // hmap_print_all(map);

    // free the map and all entries
    hmap_destroy(map);

    printf("==================================================\n");

    // comparision with O(n) search, array
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t ii = 0; ii < test_size; ii++) {
        if (strcmp(key_ref[test_idx], key_ref[ii]) == 0) {
            printf("found %s\n", key_ref[ii]);
            break;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = get_elapsed_ms(start, end);
    printf(ANSI_COLOR_YELLOW);
    printf("Time taken array: %.2f ms\n", elapsed);
    printf(ANSI_COLOR_RESET);

    // free the content
    for (size_t ii = 0; ii < test_size; ii++)
        free(key_ref[ii]);

    return 0;
}