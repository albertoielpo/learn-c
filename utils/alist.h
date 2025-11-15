/**
 * @file alist.h
 * @brief Dynamic array list implementation for homogeneous pointer collections
 *
 * A type-safe dynamic array that stores pointers to data of a single type.
 * The list automatically grows when capacity is exceeded and can shrink on removals.
 *
 * Key Features:
 * - Type-safe: All elements must match the ALType specified at creation
 * - Dynamic: Automatically resizes as elements are added or removed
 * - Flexible memory: Supports both shallow (pointer-only) and deep (data+pointer) cleanup
 *
 * Memory Ownership Models:
 * 1. Non-owning mode: Data lifetime managed externally (stack variables, static data)
 *    - Use al_destroy() and al_remove() to free only the list structure
 *    - Original data remains valid after list operations
 *
 * 2. Owning mode: Data heap-allocated specifically for this list
 *    - Use al_destroy_deep() and al_remove_deep() to free both structure and data
 *    - Appropriate when using malloc(), strdup(), or similar allocations
 *
 * @note This list stores pointers, not values. For direct size_t storage, see nalist.h
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef ALIST_H
#define ALIST_H
#include <stdlib.h>

/**
 * @brief Supported element types for type-safe storage
 *
 * Each AList instance is restricted to a single type to ensure homogeneity.
 */
typedef enum
{
    AL_TYPE_STR,   // Null-terminated string (char*)
    AL_TYPE_INT8,  // Pointer to 8-bit signed integer (int8_t*)
    AL_TYPE_INT16, // Pointer to 16-bit signed integer (int16_t*)
    AL_TYPE_INT32, // Pointer to 32-bit signed integer (int32_t*)
    AL_TYPE_INT64  // Pointer to 64-bit signed integer (int64_t*)
} ALType;

/**
 * @brief Array list structure
 *
 * Internal representation of the dynamic array. All fields should be treated
 * as read-only by external code; use the provided functions for modifications.
 */
typedef struct
{
    size_t capacity; // Maximum number of elements before reallocation
    size_t size;     // Current number of elements in the list
    void **data;     // Array of pointers to elements
    ALType type;     // Type constraint for all elements
} AList;

/**
 * @brief Create a new array list with specified capacity and type
 *
 * Allocates a new array list with initial capacity. The list will automatically
 * grow beyond this capacity as needed. All elements added to the list must match
 * the specified type.
 *
 * @param[in] capacity Initial number of elements the list can hold without reallocation.
 *                     Use a reasonable estimate to minimize early reallocations.
 *                     Minimum recommended: 1
 * @param[in] type Type of elements this list will store. Cannot be changed after creation.
 *
 * @return Pointer to the newly created AList, or NULL if allocation fails
 *
 * @note The returned list must be freed with al_destroy() or al_destroy_deep()
 *
 * Example:
 * @code
 * AList *strings = al_create(10, AL_TYPE_STR);
 * if (!strings) {
 *     // Handle allocation failure
 * }
 * @endcode
 */
AList *al_create(size_t capacity, ALType type);

/**
 * @brief Destroy the array list structure without freeing element data
 *
 * Deallocates the list structure and internal pointer array, but does NOT free
 * the elements themselves. Use this when:
 * - Elements are stack-allocated
 * - Elements are managed by external code
 * - Element lifetime exceeds the list's lifetime
 *
 * @param[in] list Pointer to the list to destroy. Can be NULL (no-op).
 *
 * @warning After this call, the list pointer is invalid. The element data
 *          remains valid and must be freed separately if heap-allocated.
 *
 * @see al_destroy_deep() for lists owning heap-allocated data
 *
 * Example:
 * @code
 * int values[] = {1, 2, 3};
 * AList *list = al_create(3, AL_TYPE_INT32);
 * al_append(list, &values[0]);  // Stack data
 * al_destroy(list);  // Safe: values[] still valid
 * @endcode
 */
void al_destroy(AList *list);

/**
 * @brief Destroy the array list and free all element data
 *
 * Deallocates both the list structure and all elements it contains. Use this when:
 * - Elements were allocated with malloc(), calloc(), or strdup()
 * - The list owns its element data
 * - No external references to the elements exist
 *
 * @param[in] list Pointer to the list to destroy. Can be NULL (no-op).
 *
 * @warning Calling this on a list with non-heap elements (stack variables,
 *          string literals, or externally managed data) will cause undefined
 *          behavior (likely a crash). Use al_destroy() instead for such cases.
 *
 * @see al_destroy() for lists with externally managed data
 *
 * Example:
 * @code
 * AList *list = al_create(3, AL_TYPE_STR);
 * al_append(list, strdup("hello"));  // Heap-allocated string
 * al_destroy_deep(list);  // Frees both list and string
 * @endcode
 */
void al_destroy_deep(AList *list);

/**
 * @brief Append an element to the end of the list
 *
 * Adds the element at the end, increasing the list size by 1. If the current
 * capacity is exceeded, the list automatically reallocates with increased capacity
 * (typically doubling).
 *
 * Time complexity: O(1) amortized, O(n) worst case when reallocation occurs
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] ele Pointer to the element to add. The element type must match
 *                the list's ALType. Must not be NULL.
 *
 * @return 1 on success, 0 on failure (NULL input or reallocation failure)
 *
 * @note The list stores the pointer, not a copy of the data. Ensure the pointed-to
 *       data remains valid for the lifetime of the list or until removed.
 *
 * Example:
 * @code
 * AList *list = al_create(2, AL_TYPE_STR);
 * if (!al_append(list, "first")) {
 *     // Handle error
 * }
 * al_append(list, "second");  // List: ["first", "second"]
 * @endcode
 */
int al_append(AList *list, void *ele);

/**
 * @brief Prepend an element to the beginning of the list
 *
 * Inserts the element at index 0, shifting all existing elements one position
 * to the right. If capacity is exceeded, the list automatically reallocates.
 *
 * Time complexity: O(n) due to element shifting
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] ele Pointer to the element to add. Must match the list's ALType
 *                and must not be NULL.
 *
 * @return 1 on success, 0 on failure (NULL input or reallocation failure)
 *
 * @note This operation is more expensive than al_append() due to shifting.
 *       Consider using al_append() if order doesn't matter.
 *
 * Example:
 * @code
 * AList *list = al_create(2, AL_TYPE_STR);
 * al_append(list, "second");
 * al_prepend(list, "first");  // List: ["first", "second"]
 * @endcode
 */
int al_prepend(AList *list, void *ele);

/**
 * @brief Retrieve an element at the specified index
 *
 * Returns a pointer to the element at the given index without removing it.
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] idx Zero-based index of the element to retrieve.
 *                Must be in range [0, list->size - 1].
 *
 * @return Pointer to the element at the index, or NULL if:
 *         - list is NULL
 *         - idx is out of bounds (>= list->size)
 *
 * Example:
 * @code
 * AList *list = al_create(3, AL_TYPE_STR);
 * al_append(list, "hello");
 * char *str = (char*)al_get(list, 0);  // Returns "hello"
 * char *invalid = (char*)al_get(list, 1);  // Returns NULL (out of bounds)
 * @endcode
 */
void *al_get(AList *list, size_t idx);

/**
 * @brief Insert an element at the specified index
 *
 * Inserts the element at the given index, shifting all elements at that position
 * and beyond one position to the right. If capacity is exceeded, the list
 * automatically reallocates.
 *
 * Time complexity: O(n) for shifting elements
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] ele Pointer to the element to insert. Must match the list's ALType
 *                and must not be NULL.
 * @param[in] idx Zero-based index where the element will be inserted.
 *                Must be in range [0, list->size]. Use list->size to append.
 *
 * @return 1 on success, 0 on failure (NULL input, invalid index, or reallocation failure)
 *
 * Example:
 * @code
 * AList *list = al_create(3, AL_TYPE_STR);
 * al_append(list, "first");
 * al_append(list, "third");
 * al_insert(list, "second", 1);  // List: ["first", "second", "third"]
 * @endcode
 */
int al_insert(AList *list, void *ele, size_t idx);

/**
 * @brief Remove an element at the specified index without freeing its data
 *
 * Removes the element at the given index and shifts all subsequent elements
 * one position to the left. The list may automatically shrink if the capacity
 * significantly exceeds the new size.
 *
 * Time complexity: O(n) for shifting elements
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] idx Zero-based index of the element to remove.
 *                Must be in range [0, list->size - 1].
 *
 * @return 1 on success, 0 on failure (NULL list or invalid index)
 *
 * @warning The caller is responsible for freeing the removed element's data
 *          if it was heap-allocated. The element pointer is not returned.
 *          Consider storing it via al_get() before removal if needed.
 *
 * @see al_remove_deep() to automatically free the element data
 *
 * Example:
 * @code
 * AList *list = al_create(3, AL_TYPE_STR);
 * al_append(list, "keep");
 * al_append(list, "remove");
 * void *elem = al_get(list, 1);  // Save reference if needed
 * al_remove(list, 1);  // List: ["keep"], elem still valid
 * @endcode
 */
int al_remove(AList *list, size_t idx);

/**
 * @brief Remove an element at the specified index and free its memory
 *
 * Removes the element at the given index, frees its memory with free(), and
 * shifts all subsequent elements one position to the left. The list may
 * automatically shrink if capacity significantly exceeds the new size.
 *
 * Time complexity: O(n) for shifting elements
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] idx Zero-based index of the element to remove.
 *                Must be in range [0, list->size - 1].
 *
 * @return 1 on success, 0 on failure (NULL list or invalid index)
 *
 * @warning Only use this when the element was allocated with malloc(), calloc(),
 *          strdup(), or similar. Using this on stack-allocated or externally
 *          managed data will cause undefined behavior (likely a crash).
 *
 * @see al_remove() for lists with externally managed element data
 *
 * Example:
 * @code
 * AList *list = al_create(2, AL_TYPE_STR);
 * al_append(list, strdup("heap allocated"));
 * al_remove_deep(list, 0);  // Frees string and removes from list
 * @endcode
 */
int al_remove_deep(AList *list, size_t idx);

/**
 * @brief Print all elements in the list to stdout
 *
 * Prints a formatted representation of all elements based on the list's ALType.
 * Useful for debugging and inspection.
 *
 * Output format varies by type:
 * - AL_TYPE_STR: Prints strings as-is
 * - AL_TYPE_INT8/16/32/64: Prints integer values
 *
 * @param[in] list Pointer to the list to print. Can be NULL (prints nothing).
 *
 * Example:
 * @code
 * AList *list = al_create(2, AL_TYPE_STR);
 * al_append(list, "hello");
 * al_append(list, "world");
 * al_print(list);  // Output depends on implementation
 * @endcode
 */
void al_print(AList *list);

#endif