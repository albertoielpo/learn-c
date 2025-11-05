
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
    HEType type;         // 4 bytes
    uint32_t value_size; // value_size size: 1 or >1 in case of pointer to an array.
                         // With type HE_TYPE_STR is not mandatory to indicate the size because is \0 terminated
                         // Is uint32_t (4 bytes) to fit in the padding
} HEntry;

typedef struct
{
    HEntry **entries; // entries pointer array
    size_t len;
    size_t capacity;
} HMap;

// TODO: doc
HMap *hmap_create(size_t capacity);

// TODO: doc
void hmap_destroy(HMap *map);

// TODO: doc
void hmap_add(HMap *map, char *key, void *value, HEType type, uint32_t value_size);

// TODO: doc
HEntry *hmap_get(HMap *map, char *key);

// TODO: doc
void hmap_remove(HMap *map, char *key);

/**
 * @brief Print all hashmap entries
 *
 * Print all key values
 *
 * @param[in] map
 */
void hmap_print_all(HMap *map);

// .c

/** @copydoc hmap_create */
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

// TODO: doc
static void hentry_destroy(HEntry *entry)
{
    if (entry == NULL)
        return;
    free(entry);
}

/** @copydoc hmap_destroy */
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
    size_t idx = acc % capacity;
    // printf("[DEBUG] key %s index %ld\n", key, idx);
    return idx;
}

// TODO: doc
static HEntry *hmap_get_first(HMap *map, char *key, size_t *out_idx)
{
    size_t idx = hmap_build_idx(key, map->capacity);
    (*out_idx) = idx;
    return map->entries[idx];
}

/** @copydoc hmap_get */
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

// TODO: doc
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

// TODO: doc
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

    // printf("[DEBUG] new capacity is %ld\n", new_capacity);

    return map->capacity;
}

/** @copydoc hmap_add */
void hmap_add(HMap *map, char *key, void *value, HEType type, uint32_t value_size)
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
    entry->value_size = value_size;

    map->entries[idx] = entry;
    map->len++;
}

void hmap_print_all(HMap *map)
{
    for (size_t ii = 0; ii < map->capacity; ii++)
    {
        HEntry *entry = map->entries[ii];
        if (entry == NULL)
            continue;

        printf("{ key:%s, value:", entry->key);

        if (entry->type == HE_TYPE_STR)
        {
            printf("%s ", (char *)entry->value);
        }
        else if (entry->type == HE_TYPE_INT8)
        {
            int8_t *value = (int8_t *)entry->value;
            for (size_t kk = 0; kk < entry->value_size; kk++)
                printf("%d ", value[kk]);
        }
        else if (entry->type == HE_TYPE_INT16)
        {
            int16_t *value = (int16_t *)entry->value;
            for (size_t kk = 0; kk < entry->value_size; kk++)
                printf("%d ", value[kk]);
        }
        else if (entry->type == HE_TYPE_INT32)
        {
            int32_t *value = (int32_t *)entry->value;
            for (size_t kk = 0; kk < entry->value_size; kk++)
                printf("%d ", value[kk]);
        }
        else if (entry->type == HE_TYPE_INT64)
        {
            int64_t *value = (int64_t *)entry->value;
            for (size_t kk = 0; kk < entry->value_size; kk++)
                printf("%ld ", value[kk]);
        }
        printf("}\n");
    }
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 hmap.c
int main(void)
{
    HMap *map = hmap_create(2);
    hmap_add(map, "a", "pluto", HE_TYPE_STR, 1);
    hmap_add(map, "c", "paperino", HE_TYPE_STR, 1);
    hmap_add(map, "b", "xyz", HE_TYPE_STR, 1);

    int8_t x = 112;
    hmap_add(map, "d", &x, HE_TYPE_INT8, 1);

    int y[] = {1230, 9920, 84530};
    hmap_add(map, "e", &y, HE_TYPE_INT32, 3);

    hmap_print_all(map);
    hmap_destroy(map);
    return 0;
}