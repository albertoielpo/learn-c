/**
 * @file llist.h
 * @brief Heterogeneous doubly linked list implementation with type tagging
 *
 * A flexible doubly linked list that stores pointers to data of different types.
 * Each node can contain a different data type, tracked via LLNodeType enum.
 *
 * Key Features:
 * - Heterogeneous: Different nodes can store different types
 * - Doubly linked: Efficient bidirectional traversal
 * - Type-tagged: Each node tracks its element type
 * - Array support: Nodes can point to single values or arrays
 * - Optimized access: Uses closest end (head/tail) for retrieval
 *
 * Memory Ownership Models:
 * 1. Non-owning mode: Element data managed externally (stack, static, or external heap)
 *    - Use ll_destroy() and ll_remove() to free only the list/node structure
 *    - Element data remains valid after removal
 *
 * 2. Owning mode: Element data heap-allocated specifically for this list
 *    - Use ll_destroy_deep() and ll_remove_deep() to free both structure and data
 *    - Appropriate for malloc(), strdup(), or similar allocations
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef LLIST_H
#define LLIST_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Supported element types for heterogeneous storage
 *
 * Each node can store a different type, enabling mixed-type lists.
 */
typedef enum
{
    LL_TYPE_STR,   // Null-terminated string (char*)
    LL_TYPE_INT8,  // Pointer to 8-bit signed integer(s) (int8_t*)
    LL_TYPE_INT16, // Pointer to 16-bit signed integer(s) (int16_t*)
    LL_TYPE_INT32, // Pointer to 32-bit signed integer(s) (int32_t*)
    LL_TYPE_INT64  // Pointer to 64-bit signed integer(s) (int64_t*)
} LLNodeType;

/**
 * @brief Node structure for doubly linked list
 *
 * Each node maintains bidirectional links and stores metadata about its element.
 */
typedef struct llnode
{
    struct llnode *prev; // Pointer to previous node (NULL for head)
    void *elem;          // Pointer to element data
    struct llnode *next; // Pointer to next node (NULL for tail)
    LLNodeType type;     // Type of the element

    uint32_t elem_size; // Number of elements: 1 for single value, >1 for array.
                        // For LL_TYPE_STR, size is optional (null-terminated)
} LLNode;

/**
 * @brief Doubly linked list structure
 *
 * Maintains pointers to both ends for efficient operations and tracks list size.
 */
typedef struct
{
    LLNode *head; // Pointer to first node (NULL if empty)
    LLNode *tail; // Pointer to last node (NULL if empty)
    size_t size;  // Current number of nodes in the list
} LList;

/**
 * @brief Create and initialize a new empty linked list
 *
 * Allocates a new list structure with no nodes. The list must be freed
 * with ll_destroy() or ll_destroy_deep() when no longer needed.
 *
 * @return Pointer to newly created LList, or NULL if allocation fails
 *
 * @note The returned list is empty (head and tail are NULL, size is 0)
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * if (!list) {
 *     // Handle allocation failure
 * }
 * @endcode
 */
LList *ll_create(void);

/**
 * @brief Destroy the linked list structure without freeing element data
 *
 * Deallocates all nodes and the list structure, but does NOT free the
 * element data. Use this when:
 * - Elements are stack-allocated
 * - Elements are managed by external code
 * - Element lifetime exceeds the list's lifetime
 *
 * Time complexity: O(n)
 *
 * @param[in] list Pointer to the list to destroy. Can be NULL (no-op).
 *
 * @warning After this call, the list pointer is invalid. Element data
 *          remains valid and must be freed separately if heap-allocated.
 *
 * @see ll_destroy_deep() for lists owning heap-allocated element data
 *
 * Example:
 * @code
 * int value = 42;
 * LList *list = ll_create();
 * ll_append(list, &value, 1, LL_TYPE_INT32);  // Stack data
 * ll_destroy(list);  // Safe: value still valid
 * @endcode
 */
void ll_destroy(LList *list);

/**
 * @brief Destroy the linked list and free all element data
 *
 * Deallocates all nodes, their element data, and the list structure.
 * Use this when:
 * - Elements were allocated with malloc(), calloc(), or strdup()
 * - The list owns its element data
 * - No external references to the elements exist
 *
 * Time complexity: O(n)
 *
 * @param[in] list Pointer to the list to destroy. Can be NULL (no-op).
 *
 * @warning Calling this on a list with non-heap elements (stack variables,
 *          string literals, or externally managed data) will cause undefined
 *          behavior (likely a crash). Use ll_destroy() instead for such cases.
 *
 * @see ll_destroy() for lists with externally managed element data
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * ll_append(list, strdup("hello"), 1, LL_TYPE_STR);  // Heap string
 * int *num = malloc(sizeof(int));
 * *num = 42;
 * ll_append(list, num, 1, LL_TYPE_INT32);  // Heap integer
 * ll_destroy_deep(list);  // Frees all: nodes, "hello", num, and list
 * @endcode
 */
void ll_destroy_deep(LList *list);

/**
 * @brief Get node at the specified index
 *
 * Retrieves a node using an optimized traversal strategy: starts from head
 * if the index is in the first half of the list, or from tail if in the
 * second half, minimizing traversal distance.
 *
 * Time complexity: O(n/2) average, O(n) worst case
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] idx Zero-based index of the node to retrieve.
 *                Must be in range [0, list->size - 1].
 *
 * @return Pointer to the node at the index, or NULL if:
 *         - list is NULL
 *         - idx is out of bounds (>= list->size)
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * ll_append(list, "first", 1, LL_TYPE_STR);
 * ll_append(list, "second", 1, LL_TYPE_STR);
 * LLNode *node = ll_get(list, 0);  // Returns node with "first"
 * if (node) {
 *     printf("%s\n", (char*)node->elem);
 * }
 * @endcode
 */
LLNode *ll_get(LList *list, size_t idx);

/**
 * @brief Insert element at the specified index
 *
 * Creates a new node containing the element and inserts it at the given position.
 * All nodes at and after this position shift one position to the right.
 *
 * Time complexity: O(n) for middle insertions, O(1) for head/tail
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] elem Pointer to element data (caller maintains ownership unless using ll_remove_deep).
 *                 Must not be NULL.
 * @param[in] elem_size Number of elements: 1 for single value, >1 for array.
 *                      For LL_TYPE_STR, can be 0 (size determined by null terminator).
 * @param[in] type Type of the element being stored.
 * @param[in] idx Zero-based insertion position. Must be in range [0, list->size].
 *                Use 0 to prepend, list->size to append.
 *
 * @return Pointer to newly created node, or NULL on failure (allocation error or invalid idx)
 *
 * @note The list stores the pointer, not a copy. Ensure the pointed-to data
 *       remains valid for the lifetime of the node.
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * ll_add(list, "first", 1, LL_TYPE_STR, 0);   // Insert at head
 * ll_add(list, "third", 1, LL_TYPE_STR, 1);   // Insert after "first"
 * ll_add(list, "second", 1, LL_TYPE_STR, 1);  // Insert between them
 * // List: "first" -> "second" -> "third"
 * @endcode
 */
LLNode *ll_add(LList *list, void *elem, uint32_t elem_size, LLNodeType type, size_t idx);

/**
 * @brief Remove element at the specified index without freeing its data
 *
 * Removes the node at the given index, reconnecting adjacent nodes to maintain
 * the list structure. The node is freed, but the element data is not.
 *
 * Time complexity: O(n)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] idx Zero-based index of the node to remove.
 *                Must be in range [0, list->size - 1].
 *
 * @return 1 on success, 0 on failure (NULL list or invalid index)
 *
 * @warning The caller is responsible for freeing the removed element's data
 *          if it was heap-allocated. Use ll_get() before removal to save
 *          the element pointer if needed.
 *
 * @see ll_remove_deep() to automatically free the element data
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * int value = 42;
 * ll_append(list, &value, 1, LL_TYPE_INT32);  // Stack data
 * ll_remove(list, 0);  // Safe: value still valid
 * @endcode
 */
int ll_remove(LList *list, size_t idx);

/**
 * @brief Remove element at the specified index and free its memory
 *
 * Removes the node at the given index and frees the element's memory with free(),
 * then reconnects adjacent nodes to maintain the list structure.
 *
 * Time complexity: O(n)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] idx Zero-based index of the node to remove.
 *                Must be in range [0, list->size - 1].
 *
 * @return 1 on success, 0 on failure (NULL list or invalid index)
 *
 * @warning Only use this when the element was allocated with malloc(), calloc(),
 *          strdup(), or similar. Using this on stack-allocated or externally
 *          managed data will cause undefined behavior (likely a crash).
 *
 * @see ll_remove() for lists with externally managed element data
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * ll_append(list, strdup("heap allocated"), 1, LL_TYPE_STR);
 * ll_remove_deep(list, 0);  // Frees string and node
 * @endcode
 */
int ll_remove_deep(LList *list, size_t idx);

/**
 * @brief Print a single node's element data to stdout
 *
 * Prints the node's element based on its type. For arrays (elem_size > 1),
 * prints all elements. For strings, prints as-is.
 *
 * @param[in] cur Pointer to the node to print. Can be NULL (prints error).
 *
 * @note Output format varies by type:
 *       - LL_TYPE_STR: String followed by space and newline
 *       - LL_TYPE_INT*: Space-separated values followed by newline
 */
void ll_print_node(const LLNode *cur);

/**
 * @brief Print all list elements from head to tail
 *
 * Traverses the list forward and prints each node's data on a separate line.
 *
 * Time complexity: O(n)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 *
 * Example output:
 * @code
 * // For a list with ["hello", "world"]
 * hello
 * world
 * @endcode
 */
void ll_print(const LList *list);

/**
 * @brief Print all list elements from tail to head
 *
 * Traverses the list backward and prints each node's data on a separate line.
 *
 * Time complexity: O(n)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 *
 * Example output:
 * @code
 * // For a list with ["hello", "world"]
 * world
 * hello
 * @endcode
 */
void ll_print_reverse(const LList *list);

/**
 * @brief Check if the list is empty
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Can be NULL.
 *
 * @return 1 if the list is empty or NULL, 0 if it contains elements
 */
int ll_is_empty(const LList *list);

/**
 * @brief Get the number of elements in the list
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Can be NULL.
 *
 * @return Number of elements in the list, or 0 if list is NULL
 */
size_t ll_get_size(const LList *list);

/**
 * @brief Append element to the end of the list
 *
 * Convenience function that adds an element at the tail position.
 * Equivalent to ll_add(list, elem, elem_size, type, list->size).
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] elem Pointer to element data. Must not be NULL.
 * @param[in] elem_size Number of elements (1 for single value, >1 for array).
 * @param[in] type Type of the element.
 *
 * @return Pointer to newly created node, or NULL on allocation failure
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * ll_append(list, "first", 1, LL_TYPE_STR);
 * ll_append(list, "second", 1, LL_TYPE_STR);
 * // List: "first" -> "second"
 * @endcode
 */
LLNode *ll_append(LList *list, void *elem, uint32_t elem_size, LLNodeType type);

/**
 * @brief Prepend element to the beginning of the list
 *
 * Convenience function that adds an element at the head position.
 * Equivalent to ll_add(list, elem, elem_size, type, 0).
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] elem Pointer to element data. Must not be NULL.
 * @param[in] elem_size Number of elements (1 for single value, >1 for array).
 * @param[in] type Type of the element.
 *
 * @return Pointer to newly created node, or NULL on allocation failure
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * ll_append(list, "second", 1, LL_TYPE_STR);
 * ll_prepend(list, "first", 1, LL_TYPE_STR);
 * // List: "first" -> "second"
 * @endcode
 */
LLNode *ll_prepend(LList *list, void *elem, uint32_t elem_size, LLNodeType type);

/**
 * @brief Pop the last element from the list
 *
 * Removes the tail node and copies its data into the provided result node.
 * The element pointer is transferred to res->elem, but the element data
 * itself is not freed.
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Must not be NULL and not empty.
 * @param[out] res Pointer to a node structure where the popped node's data
 *                 will be stored. Must not be NULL. The prev and next pointers
 *                 are set to NULL, while elem, type, and elem_size are copied.
 *
 * @return 1 on success, 0 on failure (empty list or NULL inputs)
 *
 * @warning The caller is responsible for freeing res->elem if it was heap-allocated.
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * ll_append(list, strdup("last"), 1, LL_TYPE_STR);
 * LLNode popped;
 * if (ll_pop(list, &popped)) {
 *     printf("%s\n", (char*)popped.elem);
 *     free(popped.elem);  // Caller must free heap data
 * }
 * @endcode
 */
int ll_pop(LList *list, LLNode *res);

/**
 * @brief Get the first node in the list
 *
 * Direct access to the head node without traversal.
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 *
 * @return Pointer to the head node, or NULL if the list is empty
 */
LLNode *ll_get_head(LList *list);

/**
 * @brief Get the last node in the list
 *
 * Direct access to the tail node without traversal.
 *
 * Time complexity: O(1)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 *
 * @return Pointer to the tail node, or NULL if the list is empty
 */
LLNode *ll_get_tail(LList *list);

/**
 * @brief Replace a node's element data at the specified index
 *
 * Updates an existing node's element pointer, size, and type. Does not free
 * the old element data - caller must handle that separately if needed.
 *
 * Time complexity: O(n)
 *
 * @param[in] list Pointer to the list. Must not be NULL.
 * @param[in] elem Pointer to new element data. Must not be NULL.
 * @param[in] elem_size New element size (1 for single value, >1 for array).
 * @param[in] type New type of the element.
 * @param[in] idx Zero-based index of the node to change.
 *                Must be in range [0, list->size - 1].
 *
 * @return Pointer to the modified node, or NULL on failure (invalid index)
 *
 * @warning The old element data is not freed. Save a reference to it before
 *          calling this function if you need to free it.
 *
 * Example:
 * @code
 * LList *list = ll_create();
 * char *old_str = strdup("old");
 * ll_append(list, old_str, 1, LL_TYPE_STR);
 *
 * char *new_str = strdup("new");
 * LLNode *node = ll_change(list, new_str, 1, LL_TYPE_STR, 0);
 * free(old_str);  // Caller must free old data
 * @endcode
 */
LLNode *ll_change(LList *list, void *elem, uint32_t elem_size, LLNodeType type, size_t idx);

#endif // LLIST_H