/**
 * @brief Hash map linear probing implementation
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 *
 * This hash map implementation does not own the data
 */
#ifndef HMAP_H
#define HMAP_H
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#include <stdint.h>
#include <stdlib.h>

typedef enum {
    HE_TYPE_NULL,  // tombstone for element deleted
    HE_TYPE_STR,   // string \0 terminated (char*)
    HE_TYPE_INT8,  //  8-bit signed integer pointer
    HE_TYPE_INT16, // 16-bit signed integer pointer
    HE_TYPE_INT32, // 32-bit signed integer pointer
    HE_TYPE_INT64  // 64-bit signed integer pointer
} HEType;

typedef struct
{
    char *key;           // 8 bytes
    void *value;         // 8 bytes
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

/**
 * @brief Create an hash map
 *
 * Hash map creation given an initial capacity
 *
 * @param[in] capacity must be a power of two
 * @return hash map pointer or NULL
 */
HMap *hmap_create(size_t capacity);

/**
 * @brief Destroy an hash map
 *
 * Hash map destroy method
 *
 * @param[in] map
 */
void hmap_destroy(HMap *map);

/**
 * @brief Add an element to hash map
 *
 * Add an element given the pair key and value
 *
 * @param[in] map
 * @param[in] key
 * @param[in] value
 * @param[in] type value type (HEType)
 * @param[in] value_size 1 in case of single element, > 1 in case of array
 * @return 1 if inserted, 0 in case of error
 */
int hmap_add(HMap *map, char *key, void *value, HEType type, uint32_t value_size);

/**
 * @brief Get an element given the key
 *
 * Hash map get element function
 *
 * @param[in] map
 * @param[in] key
 * @return HEntry or NULL
 */
HEntry *hmap_get(HMap *map, char *key);

/**
 * @brief Remove an element given the key
 *
 * Hash map logical deletion
 *
 * @param[in] map
 * @param[in] key
 * @return 1 if success, 0 in case of error
 */
int hmap_remove(HMap *map, char *key);

/**
 * @brief Print the entry
 *
 * Print pattern
 * { key: *key, value, *value }
 *
 * Example
 * { key:690c9cb47da9ec53f99414de, value:112 }
 *
 * @param[in] entry
 * @return 1 in case of successful print else 0
 */
int hmap_print(HEntry *entry);

/**
 * @brief Print all hashmap entries
 *
 * Print pattern
 * { key: *key, value, *value }
 *
 * Example
 * { key:690c9cb47da9ec53f99414de, value:112 }
 *
 * @param[in] map
 */
void hmap_print_all(HMap *map);

#endif