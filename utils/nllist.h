#ifndef NLINKED_LIST_H
#define NLINKED_LIST_H

#include <stddef.h> // for size_t

/**
 * Node structure for doubly linked list of numeric values
 */
typedef struct nllnode
{
    struct nllnode *prev; // Pointer to previous node
    size_t elem;          // Numeric element value
    struct nllnode *next; // Pointer to next node
} NLLNode;

/**
 * Doubly linked list structure for numeric values
 */
typedef struct
{
    NLLNode *head; // Pointer to first node
    NLLNode *tail; // Pointer to last node
    size_t size;   // Number of elements in the list
} NLList;

/**
 * Create and initialize a new numeric linked list
 * @returns Pointer to new NLList, or NULL on allocation failure
 */
NLList *nll_create(void);

/**
 * Deallocate linked list and all nodes
 * @param[in] list Pointer to the list to destroy (can be NULL)
 */
void nll_destroy(NLList *list);

/**
 * Get node at specified index
 * @param[in] list Pointer to the list
 * @param[in] idx Zero-based index
 * @returns Pointer to node if found, NULL if index out of bounds
 * @note Uses optimized traversal: from head if idx < size/2, from tail otherwise
 */
NLLNode *nll_get(const NLList *list, size_t idx);

/**
 * Insert element at specified index
 * @param[in] list Pointer to the list
 * @param[in] elem Numeric value to insert
 * @param[in] idx Zero-based index (0 = prepend, size = append)
 * @returns Pointer to newly created node, or NULL on failure
 * @note idx must be in range [0, size]
 */
NLLNode *nll_add(NLList *list, size_t elem, size_t idx);

/**
 * Remove element at specified index
 * @param[in] list Pointer to the list
 * @param[in] idx Zero-based index
 * @returns New size of list after removal, or (size_t)-1 on failure
 */
size_t nll_remove(NLList *list, size_t idx);

/**
 * Print list from head to tail
 * @param[in] list Pointer to the list
 * @note Prints to stdout with space-separated values
 */
void nll_print(const NLList *list);

/**
 * Print list from tail to head
 * @param[in] list Pointer to the list
 * @note Prints to stdout with space-separated values
 */
void nll_print_reverse(const NLList *list);

/**
 * Check if list is empty
 * @param[in] list Pointer to the list
 * @returns true if empty or NULL, false otherwise
 */
int nll_is_empty(const NLList *list);

/**
 * Get size of list
 * @param[in] list Pointer to the list
 * @returns Number of elements in list, 0 if NULL
 */
size_t nll_get_size(const NLList *list);

#endif // NLINKED_LIST_H