
#include "hmap.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief n is power of two?
 *
 * Check if a size_t input is a power of two
 *
 * @param[in] n
 * @return 1 if is a power of two else 0
 */
static int is_power_of_two(size_t n) {
    return (n > 0 && (n & (n - 1)) == 0) ? 1 : 0;
}

/** @copydoc hmap_create */
HMap *hmap_create(size_t capacity) {
    if (capacity <= 0 || !is_power_of_two(capacity)) {
        fprintf(stderr, "[hmap_create] Invalid capacity\n");
        return NULL;
    }

    HMap *map = calloc(1, sizeof(HMap));
    if (map == NULL) {
        perror("[hmap_create] Cannot create hmap: out of memory\n");
        return NULL;
    }

    map->len = 0;
    map->capacity = capacity;
    map->entries = calloc(capacity, sizeof(HEntry *));
    if (map->entries == NULL) {
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
static void hentry_destroy(HEntry *entry) {
    if (entry == NULL)
        return;
    free(entry);
}

/** @copydoc hmap_destroy */
void hmap_destroy(HMap *map) {
    if (map == NULL)
        return;
    if (map->entries == NULL) {
        free(map);
        return;
    }

    for (size_t ii = 0; ii < map->capacity; ii++) {
        if (map->entries[ii] != NULL)
            hentry_destroy(map->entries[ii]);
    }

    free(map->entries);
    free(map);
}

/**
 * @brief Hmap hash function
 *
 * Hash function algorithm
 * See description: https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
 *
 * @param[in] key
 * @param[in] capacity
 * @return index
 */
static size_t hmap_build_idx(const char *key, size_t capacity) {
    size_t hash = FNV_OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (size_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash & (capacity - 1);
}

// basic implementation
// static size_t hmap_build_idx(char *key, size_t capacity)
// {
//     char *pos = key;
//     size_t acc = 0;
//     while (*pos)
//     {
//         acc += *pos;
//         pos++;
//     }

//     // more efficient (acc % capacity)
//     // with the constraint the capacity must be a power of two
//     return acc & (capacity - 1);
// }

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
static HEntry *hmap_get_first(HMap *map, char *key, size_t *out_idx) {
    size_t idx = hmap_build_idx(key, map->capacity);
    (*out_idx) = idx;
    return map->entries[idx];
}

/** @copydoc hmap_get */
HEntry *hmap_get(HMap *map, char *key) {
    if (map == NULL)
        return NULL;

    size_t idx = hmap_build_idx(key, map->capacity);
    size_t start_idx = idx; // Remember where we started
    size_t probes = 0;

    HEntry *cur = map->entries[idx];
    if (cur == NULL)
        return NULL;

    // if the element was previously deleted OR the key is a collision then find the next spot
    while (cur->type == HE_TYPE_NULL || strcmp(cur->key, key) != 0) {
        // Use wraparound instead of giving up
        // Efficient wraparound (capacity is power of 2)
        idx = (idx + 1) & (map->capacity - 1);

        // Prevent infinite loop if we've searched the entire table
        probes++;
        if (idx == start_idx || probes >= map->capacity)
            return NULL; // Searched entire table, not found

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
static HEntry *hmap_get_by_idx(HMap *map, size_t idx, int *needs_grow) {
    if (idx >= map->capacity) {
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
static size_t hmap_grow(HMap *map) {
    // if capacity if full then resize
    size_t new_capacity = map->capacity * 2;
    void *temp = calloc(new_capacity, sizeof(HEntry *));
    if (!temp) {
        perror("[hmap_grow] Reallocation failed! The old data are still valid");
        return 0;
    }

    // rehashing
    HEntry **new_entries = (HEntry **)temp;
    size_t ele_count = map->len;

    for (size_t ii = 0; ii < map->capacity && ele_count > 0; ii++) {
        if (map->entries[ii] == NULL)
            continue;

        // Skip and free tombstones during rehashing
        if (map->entries[ii]->type == HE_TYPE_NULL) {
            hentry_destroy(map->entries[ii]);
            continue;
        }

        size_t new_idx = hmap_build_idx(map->entries[ii]->key, new_capacity);

        // Add bounds checking to prevent buffer overflow
        while (new_entries[new_idx] != NULL) {
            new_idx++;
            if (new_idx >= new_capacity) // Bounds check
                new_idx = 0;             // Wrap around
        }

        new_entries[new_idx] = map->entries[ii];
        ele_count--;
    }

    free(map->entries); // frees old entries

    map->entries = new_entries;   // assign new ones
    map->capacity = new_capacity; // with new capacity

    return map->capacity;
}

/** @copydoc hmap_add */
int hmap_add(HMap *map, char *key, void *value, HEType type, uint32_t value_size) {
    if (map == NULL)
        return 0;

    if (map->len > map->capacity / 2) {
        // hash map grows when capacity is half of the occupied len
        if (!hmap_grow(map))
            return 0;
    }

    size_t idx = (size_t)-1;
    HEntry *cur = hmap_get_first(map, key, &idx);

    if (idx == (size_t)-1) {
        fprintf(stderr, "[hmap_add] An error occured getting idx\n");
        return 0;
    }

    size_t probes = 0;

    while (cur != NULL) {
        // Prevent infinite loop
        if (probes++ >= map->capacity) {
            fprintf(stderr, "[hmap_add] Table full, cannot insert\n");
            return 0;
        }

        // the spot is already occupied
        if (cur->type == HE_TYPE_NULL) {
            // the previous element is deleted logically then override
            cur->key = key; // reassign the key
            cur->value = value;
            cur->type = type;
            cur->value_size = value_size;
            map->len++;
            return 1;
        }

        if (strcmp(cur->key, key) == 0) {
            // if the key is equals
            cur->value = value;
            cur->type = type;
            cur->value_size = value_size;
            return 1;
        }

        // else is a collision then go to the right one until one free is found
        int needs_grow = 0;
        HEntry *new_cur = hmap_get_by_idx(map, ++idx, &needs_grow);
        if (needs_grow) {
            // Grow and RESTART the insertion
            if (!hmap_grow(map))
                return 0;

            // Restart from scratch with new capacity
            idx = (size_t)-1;
            cur = hmap_get_first(map, key, &idx);
            probes = 0;
            continue;
        } else {
            // increment the cursor
            cur = new_cur;
        }
    }

    // add element in the spot
    HEntry *entry = calloc(1, sizeof(HEntry));
    if (entry == NULL) {
        perror("[hmap_add] Cannot allocate entry");
        return 0;
    }

    entry->key = key;
    entry->value = value;
    entry->type = type;
    entry->value_size = value_size;

    map->entries[idx] = entry;
    map->len++;
    return 1;
}

/** @copydoc hmap_remove */
int hmap_remove(HMap *map, char *key) {
    if (map == NULL)
        return 0;

    HEntry *entry = hmap_get(map, key);
    if (entry == NULL)
        return 0;
    entry->type = HE_TYPE_NULL;
    map->len--;
    return 1;
}

/** @copydoc hmap_print */
int hmap_print(HEntry *entry) {
    if (entry == NULL || entry->type == HE_TYPE_NULL)
        return 0;

    printf("{ key:%s, value:", entry->key);

    if (entry->type == HE_TYPE_STR) {
        printf("%s ", (char *)entry->value);
    } else if (entry->type == HE_TYPE_INT8) {
        int8_t *value = (int8_t *)entry->value;
        for (size_t kk = 0; kk < entry->value_size; kk++)
            printf("%d ", value[kk]);
    } else if (entry->type == HE_TYPE_INT16) {
        int16_t *value = (int16_t *)entry->value;
        for (size_t kk = 0; kk < entry->value_size; kk++)
            printf("%d ", value[kk]);
    } else if (entry->type == HE_TYPE_INT32) {
        int32_t *value = (int32_t *)entry->value;
        for (size_t kk = 0; kk < entry->value_size; kk++)
            printf("%d ", value[kk]);
    } else if (entry->type == HE_TYPE_INT64) {
        int64_t *value = (int64_t *)entry->value;
        for (size_t kk = 0; kk < entry->value_size; kk++)
            printf("%ld ", value[kk]);
    }

    printf("}\n");
    return 1;
}

/** @copydoc hmap_print_all */
void hmap_print_all(HMap *map) {
    if (map == NULL)
        return;

    size_t ele_count = map->len;
    for (size_t ii = 0; ii < map->capacity && ele_count > 0; ii++) {
        if (!hmap_print(map->entries[ii]))
            continue;

        ele_count--;
    }
}