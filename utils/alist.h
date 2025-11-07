/**
 * @brief Array List implementation
 *
 * Homogeneous Array list implementation
 * All elements must be of the same type defined as ALType
 *
 * This Array list implementation does not own the data
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef ALIST_H
#define ALIST_H
#include <stdlib.h>

typedef enum
{
    AL_TYPE_STR,   // string \0 terminated (char*)
    AL_TYPE_INT8,  //  8-bit signed integer pointer
    AL_TYPE_INT16, // 16-bit signed integer pointer
    AL_TYPE_INT32, // 32-bit signed integer pointer
    AL_TYPE_INT64  // 64-bit signed integer pointer
} ALType;

typedef struct
{
    size_t capacity; // max data length
    size_t size;     // current data length
    void **data;     // data pointer array
    ALType type;     // Type of the element (4 bytes)
                     // 4 bytes padding
} AList;

/**
 * @brief Array list creation
 *
 * Allocate an array list. All elements must have the same type
 *
 * @param[in] capacity initial capacity
 * @param[in] type element type
 * @return Array list pointer
 */
AList *al_create(size_t capacity, ALType type);

/**
 * @brief Array list deallocation
 *
 * Deallocate an array list
 *
 * @param[in] list List pointer
 */
void al_destroy(AList *list);

/**
 * @brief Append an element at the end of the array list
 *
 * Dynamic array with autogrow feature using realloc
 *
 * @param[in] list List pointer
 * @param[in] ele Pointer to the element
 * @return element index or -1 in case of error
 */
size_t al_append(AList *list, void *ele);

/**
 * @brief Prepend an element at the beginning of the array list
 *
 * Dynamic array with autogrow feature using realloc
 *
 * @param[in] list List pointer
 * @param[in] ele Pointer to the element
 * @return element index or -1 in case of error
 */
size_t al_prepend(AList *list, void *ele);

/**
 * @brief Get al element from the array list
 *
 * Get an element given an index
 *
 * @param[in] list List pointer
 * @param[in] idx Index
 * @return element pointer or NULL in case of error
 */
void *al_get(AList *list, size_t idx);

/**
 * @brief Insert an element at the index of the array list
 *
 * Dynamic array with autogrow feature using realloc
 * and the elements from the same index will be right shifted
 *
 * @param[in] list List pointer
 * @param[in] ele Pointer to the element
 * @return element index or -1 in case of error
 */
size_t al_insert(AList *list, void *ele, size_t idx);

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
int al_remove(AList *list, size_t idx);

/**
 * @brief Print all elements
 *
 * Print elements inside the list
 *
 * @param[in] list List pointer
 */
void al_print(AList *list);

#endif