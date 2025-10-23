#ifndef LList_H
#define LList_H

#include <stddef.h> // for size_t

/**
 * Enumeration of supported data types for linked list elements
 */
typedef enum
{
    LL_TYPE_CHAR,  // char pointer (string)
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
    LLNodeType type;     // Type of the element
    struct llnode *next; // Pointer to next node
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
LList *ll_create_list(void);

/**
 * Deallocate linked list and all nodes
 * @param[in] list Pointer to the list to destroy (can be NULL)
 * @note Does not free the elements themselves, only the nodes
 */
void ll_destroy_list(LList *list);

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
void ll_print_list(const LList *list);

/**
 * Print list from tail to head
 * @param[in] list Pointer to the list
 * @note Prints to stdout with space-separated values
 */
void ll_print_list_reverse(const LList *list);

/**
 * Check if list is empty
 * @param[in] list Pointer to the list
 * @returns true if empty or NULL, false otherwise
 */
int ll_is_empty(const LList *list);

/**
 * Get size of list
 * @param[in] list Pointer to the list
 * @returns Number of elements in list, 0 if NULL
 */
size_t ll_get_size(const LList *list);

#endif // LList_H