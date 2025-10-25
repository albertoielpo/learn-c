/**
 * @brief Array List implementation of an Uint8 array
 *
 * Dynamic Uint8[] implementation with autogrow and autoshrink
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef U8LIST_H
#define U8LIST_H
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    size_t capacity; // max data length
    size_t size;     // current data length
    uint8_t *data;   // array of uint8
} U8List;

/**
 * @brief Array list creation
 *
 * Allocate an array list.
 *
 * @param[in] capacity initial capacity
 * @return Array list pointer
 */
U8List *u8list_create(size_t capacity);

/**
 * @brief Array list deallocation
 *
 * Deallocate an array list
 *
 * @param[in] list List pointer
 */
void u8list_destroy(U8List *list);

/**
 * @brief Insert an element at the index of the array list
 *
 * Dynamic array with autogrow feature using realloc
 * and the elements from the same index will be right shifted
 *
 * @param[in] list List pointer
 * @param[in] ele uint8 element
 * @return element index or -1 in case of error
 */
size_t u8list_insert(U8List *list, uint8_t ele, size_t idx);

/**
 * @brief Append an element at the end of the array list
 *
 * Dynamic array with autogrow feature using realloc
 *
 * @param[in] list List pointer
 * @param[in] ele uint8 element
 * @return element index or -1 in case of error
 */
size_t u8list_append(U8List *list, uint8_t ele);

/**
 * @brief Prepend an element at the beginning of the array list
 *
 * Dynamic array with autogrow feature using realloc
 *
 * @param[in] list List pointer
 * @param[in] ele uint8 element
 * @return element index or -1 in case of error
 */
size_t u8list_prepend(U8List *list, uint8_t ele);

/**
 * @brief Get al element from the array list
 *
 * Get an element given an index
 *
 * @param[in] list List pointer
 * @param[in] idx Index
 * @param[out] res Result pointer
 * @return 0 in case of error, 1 in case of success
 */
int u8list_get(U8List *list, size_t idx, uint8_t *res);

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
int u8list_remove(U8List *list, size_t idx);

/**
 * @brief Print all elements
 *
 * Print elements inside the list
 *
 * @param[in] list List pointer
 */
void u8list_print(U8List *list);

#endif // U8LIST_H