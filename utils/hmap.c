
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// .h
/**
 * @brief Hash map linear probing implementation
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */

typedef enum
{
    HE_TYPE_STR,   // string \0 terminated (char*)
    HE_TYPE_INT8,  //  8-bit signed integer pointer
    HE_TYPE_INT16, // 16-bit signed integer pointer
    HE_TYPE_INT32, // 32-bit signed integer pointer
    HE_TYPE_INT64  // 64-bit signed integer pointer
} HEType;

typedef struct
{
    char *key;
    void *value;
    HEType type;
} HEntry;

typedef struct
{
    HEntry **entries; // entryies pointer array
    size_t len;
    size_t capacity;
} HMap;

HMap *hmap_create(size_t capacity);
void hentry_destroy(HEntry *entry);
void hmap_destroy(HMap *map);
void hmap_add(HMap *map, char *key, void *value, HEType type);
HEntry *hmap_get(HMap *map, char *key);

// .c
HMap *hmap_create(size_t capacity)
{
    if (capacity <= 0)
    {
        fprintf(stderr, "Invalid capacity\n");
        return NULL;
    }

    HMap *map = calloc(sizeof(HMap), 1);
    if (map == NULL)
    {
        perror("Cannot create hmap: out of memory\n");
        return NULL;
    }

    map->len = 0;
    map->capacity = capacity;
    map->entries = calloc(sizeof(HEntry *), capacity);
    if (map->entries == NULL)
    {
        perror("Cannot create entries: out of memory\n");
        free(map);
        return NULL;
    }
    return map;
}

void hentry_destroy(HEntry *entry)
{
    if (entry == NULL)
        return;
    free(entry);
}

void hmap_destroy(HMap *map)
{
    if (map == NULL)
        return;
    if (map->entries == NULL)
    {
        free(map);
        return;
    }

    HEntry **entries = map->entries;
    HEntry *entry = *entries;
    for (size_t ii = 0; ii < map->capacity; ii++)
    {
        if (entry)
            hentry_destroy(entry);
        entries++;
        entry = *entries;
    }

    free(map->entries);
    free(map);
}

/**
 * @brief Hmap hash function
 *
 * Simple hash function algorithm based on sum numeric int8 values modulo capacity
 *
 * @param[in] key
 * @param[in] capacity
 * @return index
 */
static size_t hmap_build_idx(char *key, size_t capacity)
{
    char *pos = key;
    size_t acc = 0;
    while (*pos)
    {
        acc += *pos;
        pos++;
    }
    return acc % capacity;
}

static HEntry *hmap_get_first(HMap *map, char *key, size_t *out_idx)
{
    size_t idx = hmap_build_idx(key, map->capacity);
    (*out_idx) = idx;
    return map->entries[idx];
}

HEntry *hmap_get(HMap *map, char *key)
{
    size_t idx = hmap_build_idx(key, map->capacity);
    HEntry *cur = map->entries[idx];
    if (cur == NULL)
        return NULL;
    while (strcmp(cur->key, key) != 0)
    {
        idx++;

        // check overflow
        if (idx >= map->capacity)
            return NULL;

        cur = map->entries[idx];

        // safety exit
        if (cur == NULL)
            return NULL;
    }
    // found
    return cur;
}

static HEntry *hmap_get_by_idx(HMap *map, size_t idx, int *needs_grow)
{
    if (idx >= map->capacity)
    {
        // index out of bound
        (*needs_grow) = 1;
        return NULL;
    }
    return map->entries[idx];
}

static size_t hmap_grow(HMap *map)
{
    // if capacity if full then resize
    size_t new_capacity = map->capacity * 2;
    void *temp = calloc(sizeof(HEntry *), new_capacity);
    if (!temp)
    {
        perror("Reallocation failed! The old data are still valid");
        return 0;
    }

    // rehashing
    HEntry **new_entries = (HEntry **)temp;
    for (size_t ii = 0; ii < map->capacity; ii++)
    {
        if (map->entries[ii] == NULL)
            continue;
        size_t new_idx = hmap_build_idx(map->entries[ii]->key, new_capacity);
        while (new_entries[new_idx] != NULL)
        {
            new_idx++;
        }
        new_entries[new_idx] = map->entries[ii];
    }

    free(map->entries); // frees old entries

    map->entries = new_entries;   // assign new ones
    map->capacity = new_capacity; // with new capacity
    return map->capacity;
}

void hmap_add(HMap *map, char *key, void *value, HEType type)
{
    if (map->capacity == map->len)
    {
        if (!hmap_grow(map))
        {
            return;
        }
    }

    size_t idx = (size_t)-1;
    HEntry *cur = hmap_get_first(map, key, &idx);

    if (idx == (size_t)-1)
    {
        fprintf(stderr, "An error occured getting idx\n");
        return;
    }

    while (cur != NULL)
    {
        // the spot is already occupied
        if (strcmp(cur->key, key) == 0)
        {
            // if the key is equals then override
            cur->value = value;
            return;
        }

        // else is a collision then go to the right one until one free is found
        int needs_grow = 0;
        HEntry *new_cur = hmap_get_by_idx(map, ++idx, &needs_grow);
        if (needs_grow)
        {
            // Grow and RESTART the insertion
            if (!hmap_grow(map))
                return;

            // Restart from scratch with new capacity
            idx = (size_t)-1;
            cur = hmap_get_first(map, key, &idx);
            continue;
        }
        else
        {
            // increment the cursor
            cur = new_cur;
        }
    }

    // add element in the spot
    HEntry *entry = calloc(sizeof(HEntry), 1);
    entry->key = key;
    entry->value = value;
    entry->type = type;

    map->entries[idx] = entry;
    map->len++;
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 hmap.c
int main(void)
{
    {
        HMap *map = hmap_create(2);
        hmap_add(map, "a", "pluto", HE_TYPE_STR);
        hmap_add(map, "c", "paperino", HE_TYPE_STR);
        hmap_add(map, "b", "xyz", HE_TYPE_STR);

        HEntry *entry = hmap_get(map, "a");
        if (entry != NULL)
            printf("key %s value %s type %d\n", entry->key, (char *)entry->value, entry->type);

        entry = hmap_get(map, "b");
        if (entry != NULL)
            printf("key %s value %s type %d\n", entry->key, (char *)entry->value, entry->type);

        entry = hmap_get(map, "c");
        if (entry != NULL)
            printf("key %s value %s type %d\n", entry->key, (char *)entry->value, entry->type);

        hmap_destroy(map);
    }
    return 0;
}