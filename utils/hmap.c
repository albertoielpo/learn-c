
#include <stdio.h>
#include <string.h>
#include "hmap.h"

/**
 * @brief n is power of two?
 *
 * Check if a size_t input is a power of two
 *
 * @param[in] n
 * @return 1 if is a power of two else 0
 */
static int is_power_of_two(size_t n)
{
    return (n > 0 && (n & (n - 1)) == 0) ? 1 : 0;
}

/** @copydoc hmap_create */
HMap *hmap_create(size_t capacity)
{
    if (capacity <= 0 || !is_power_of_two(capacity))
    {
        fprintf(stderr, "[hmap_create] Invalid capacity\n");
        return NULL;
    }

    HMap *map = calloc(sizeof(HMap), 1);
    if (map == NULL)
    {
        perror("[hmap_create] Cannot create hmap: out of memory\n");
        return NULL;
    }

    map->len = 0;
    map->capacity = capacity;
    map->entries = calloc(sizeof(HEntry *), capacity);
    if (map->entries == NULL)
    {
        perror("[hmap_create] Cannot create entries: out of memory\n");
        free(map);
        return NULL;
    }
    return map;
}

/**
 * @brief destroy the hash map entry
 *
 * Free the memory of the hash map entry
 *
 * @param[in] entry to destroy
 */
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

    // more efficient (acc % capacity)
    // with the constraint the capacity must be a power of two
    return acc & (capacity - 1);
}

/**
 * @brief Hash map get first element given a key
 *
 * This is an internal method that retrieve the first HEntry which match a key.
 * In the param out_idx set the index position.
 * In case of collision return the first element
 *
 * @param[in] map
 * @param[in] key
 * @param[out] out_idx
 * @return HEntry if found else NULL
 */
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

    // if the element was previously deleted OR the key is a collision then find the next spot
    while (cur->type == HE_TYPE_NULL || strcmp(cur->key, key) != 0)
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
    // element found!
    return cur;
}

/**
 * @brief Get HEntry given the index
 *
 * Return the HEntry given the main array index. In case of collision is returned the first one.
 * Set 1 to the out param needs_grow if input idx is out of bound
 *
 * @param[in] map
 * @param[in] idx
 * @param[out] needs_grow
 * @return Return description
 */
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

/**
 * @brief Hash map grow
 *
 * Increase hash map capacity
 *
 * @param[in] map
 * @return new capacity or 0 in case of error
 */
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

/** @copydoc hmap_add */
int hmap_add(HMap *map, char *key, void *value, HEType type, uint32_t value_size)
{
    if (map->capacity == map->len)
    {
        if (!hmap_grow(map))
            return 0;
    }

    size_t idx = (size_t)-1;
    HEntry *cur = hmap_get_first(map, key, &idx);

    if (idx == (size_t)-1)
    {
        fprintf(stderr, "[hmap_add] An error occured getting idx\n");
        return 0;
    }

    while (cur != NULL)
    {
        // the spot is already occupied
        if (cur->type == HE_TYPE_NULL || strcmp(cur->key, key) == 0)
        {
            // if the key is equals or the previous element is deleted logically then override
            cur->key = key; // reassign the key in case of collision
            cur->value = value;
            cur->type = type;
            cur->value_size = value_size;
            return 1;
        }

        // else is a collision then go to the right one until one free is found
        int needs_grow = 0;
        HEntry *new_cur = hmap_get_by_idx(map, ++idx, &needs_grow);
        if (needs_grow)
        {
            // Grow and RESTART the insertion
            if (!hmap_grow(map))
                return 0;

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
    return 1;
}

/** @copydoc hmap_remove */
int hmap_remove(HMap *map, char *key)
{
    HEntry *entry = hmap_get(map, key);
    if (entry == NULL)
        return 0;
    entry->type = HE_TYPE_NULL;
    return 1;
}

/** @copydoc hmap_print_all */
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
        // else if (entry->type == HE_TYPE_NULL)
        // {
        //     printf("this element is deleted ");
        // }
        printf("}\n");
    }
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 hmap.c
int main(void)
{
    size_t init_capacity = 16;
    HMap *map = hmap_create(init_capacity);
    if (map == NULL)
        return 1;

    hmap_add(map, "a", "pluto", HE_TYPE_STR, 1);
    hmap_add(map, "c", "paperino", HE_TYPE_STR, 1);
    hmap_add(map, "b", "xyz", HE_TYPE_STR, 1);

    int8_t x = 112;
    hmap_add(map, "d", &x, HE_TYPE_INT8, 1);

    int y[] = {1230, 9920, 84530};
    hmap_add(map, "e", &y, HE_TYPE_INT32, 3);
    hmap_remove(map, "a"); // deleted logically

    // 'Q' is 'a' - init_capacity
    hmap_add(map, "Q", &x, HE_TYPE_INT8, 1); // replaced

    hmap_print_all(map);
    hmap_destroy(map);
    return 0;
}