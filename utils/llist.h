/**
 * @brief Linked list implementation
 *
 * Heterogeneous Doubly linked list implementation with void* elements
 * Supports multiple data types through type tagging
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef LLIST_H
#define LLIST_H

#include <stddef.h>

/**
 * Enumeration of supported data types for linked list elements
 */
typedef enum
{
    LL_TYPE_STR,   // string \0 terminated (char*)
    LL_TYPE_INT8,  //  8-bit signed integer pointer
    LL_TYPE_INT16, // 16-bit signed integer pointer
    LL_TYPE_INT32, // 32-bit signed integer pointer
    LL_TYPE_INT64  // 64-bit signed integer pointer
} LLNodeType;

/**
 * Node structure for doubly linked list
 */
typedef struct llnode
{
    struct llnode *prev; // Pointer to previous node
    void *elem;          // Pointer to element data
    struct llnode *next; // Pointer to next node
    LLNodeType type;     // Type of the element (4 bytes)
                         // 4 bytes padding
} LLNode;

/**
 * Doubly linked list structure
 */
typedef struct
{
    LLNode *head; // Pointer to first node
    LLNode *tail; // Pointer to last node
    size_t size;  // Number of elements in the list
} LList;

/**
 * Create and initialize a new linked list
 * @returns Pointer to new LList, or NULL on allocation failure
 */
LList *ll_create(void);

/**
 * Deallocate linked list and all nodes
 * @param[in] list Pointer to the list to destroy (can be NULL)
 * @note Does not free the elements themselves, only the nodes
 */
void ll_destroy(LList *list);

/**
 * Get node at specified index
 * @param[in] list Pointer to the list
 * @param[in] idx Zero-based index
 * @returns Pointer to node if found, NULL if index out of bounds
 * @note Uses optimized traversal: from head if idx < size/2, from tail otherwise
 */
LLNode *ll_get(LList *list, size_t idx);

/**
 * Insert element at specified index
 * @param[in] list Pointer to the list
 * @param[in] elem Pointer to element data (caller maintains ownership)
 * @param[in] type Type of the element
 * @param[in] idx Zero-based index (0 = prepend, size = append)
 * @returns Pointer to newly created node, or NULL on failure
 * @note idx must be in range [0, size]
 */
LLNode *ll_add(LList *list, void *elem, LLNodeType type, size_t idx);

/**
 * Remove element at specified index
 * @param[in] list Pointer to the list
 * @param[in] idx Zero-based index
 * @returns New size of list after removal, or (size_t)-1 on failure
 * @note Does not free the element data, only the node
 */
size_t ll_remove(LList *list, size_t idx);

/**
 * Print list from head to tail
 * @param[in] list Pointer to the list
 * @note Prints to stdout with space-separated values
 */
void ll_print(const LList *list);

/**
 * Print list from tail to head
 * @param[in] list Pointer to the list
 * @note Prints to stdout with space-separated values
 */
void ll_print_reverse(const LList *list);

/**
 * Check if list is empty
 * @param[in] list Pointer to the list
 * @returns 1 if empty or 0 otherwise
 */
int ll_is_empty(const LList *list);

/**
 * Get size of list
 * @param[in] list Pointer to the list
 * @returns Number of elements in list, 0 if NULL
 */
size_t ll_get_size(const LList *list);

/**
 * Append element at the tail
 * @param[in] list Pointer to the list
 * @param[in] elem Pointer to element data
 * @param[in] type Type of the element
 * @returns Pointer to newly created node, or NULL on failure
 */
LLNode *ll_append(LList *list, void *elem, LLNodeType type);

/**
 * Prepend element at the head
 * @param[in] list Pointer to the list
 * @param[in] elem Pointer to element data
 * @param[in] type Type of the element
 * @returns Pointer to newly created node, or NULL on failure
 */
LLNode *ll_prepend(LList *list, void *elem, LLNodeType type);

/**
 * Pop last element from the list
 * @param[in] list Pointer to the list
 * @returns Pointer to the popped element. Make sure to know the type to do a proper cast
 */
void *ll_pop(LList *list);

/**
 * Get an element from the list given the index
 * @param[in] list Pointer to the list
 * @param[in] idx Index
 * @returns Pointer to the element. Make sure to know the type to do a proper cast
 */
void *ll_get_value(LList *list, size_t idx);

/**
 * Get head node from the list
 * @param[in] list Pointer to the list
 * @returns Pointer to the head node
 */
LLNode *ll_get_head(LList *list);

/**
 * Get head element from the list
 * @param[in] list Pointer to the list
 * @returns Pointer to the head element. Make sure to know the type to do a proper cast
 */
void *ll_get_value_head(LList *list);

/**
 * Get tail node from the list
 * @param[in] list Pointer to the list
 * @returns Pointer to the tail node
 */
LLNode *ll_get_tail(LList *list);

/**
 * Get tail element from the list
 * @param[in] list Pointer to the list
 * @returns Pointer to the tail element. Make sure to know the type to do a proper cast
 */
void *ll_get_value_tail(LList *list);

#endif // LLIST_H