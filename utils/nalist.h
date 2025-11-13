/**
 * @brief Array List implementation
 *
 * Numeric array list implementation where all elements must be of size_t,
 * This array list stores values.
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef NALIST_H
#define NALIST_H
#include <stdlib.h>

typedef struct
{
    size_t capacity; // max data length
    size_t size;     // current data length
    size_t *data;    // data array pointer which elements are size_t
} NAList;

/**
 * @brief Array list creation
 *
 * Allocate an array list.
 *
 * @param[in] capacity initial capacity
 * @return Array list pointer
 */
NAList *nal_create(size_t capacity);

/**
 * @brief Array list deallocation
 *
 * Deallocate an array list
 *
 * @param[in] list List pointer
 */
void nal_destroy(NAList *list);

/**
 * @brief Append an element at the end of the array list
 *
 * Dynamic array with autogrow feature using realloc
 *
 * @param[in] list List pointer
 * @param[in] ele Element
 * @return 1 if OK or 0 in case of error
 */
int nal_append(NAList *list, size_t ele);

/**
 * @brief Prepend an element at the beginning of the array list
 *
 * Dynamic array with autogrow feature using realloc
 *
 * @param[in] list List pointer
 * @param[in] ele Element
 * @return 1 if OK or 0 in case of error
 */
int nal_prepend(NAList *list, size_t ele);

/**
 * @brief Get al element from the array list
 *
 * Get an element given an index
 *
 * @param[in] list List pointer
 * @param[in] idx Index
 * @param[out] res Result element
 * @return 1 if OK or 0 in case of error
 */
int nal_get(NAList *list, size_t idx, size_t *res);

/**
 * @brief Insert an element at the index of the array list
 *
 * Dynamic array with autogrow feature using realloc
 * and the elements from the same index will be right shifted
 *
 * @param[in] list List pointer
 * @param[in] ele Element
 * @param[in] idx Index
 * @return 1 if OK or 0 in case of error
 */
int nal_insert(NAList *list, size_t ele, size_t idx);

/**
 * @brief Remove an element at the index
 *
 * Dynamic array with autoscale feature using realloc
 * and the elements from the same index will be left shifted
 *
 * @param[in] list List pointer
 * @param[in] idx Index
 * @return 1 OK, 0 Error
 */
int nal_remove(NAList *list, size_t idx);

/**
 * @brief Print all elements
 *
 * Print elements inside the list
 *
 * @param[in] list List pointer
 */
void nal_print(NAList *list);

#endif