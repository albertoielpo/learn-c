/**
 * Doubly linked list implementation with void* elements
 * Supports multiple data types through type tagging
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "llist.h"

/**
 * Increment list size by one
 * @param[in] list A valid (non-NULL) LList pointer
 * @returns The new list size after incrementing
 */
static size_t ll_increment_size(LList *list)
{
    return ++list->size;
}

/**
 * Decrement list size by one
 * @param[in] list A valid (non-NULL) LList pointer
 * @returns The new list size after decrementing
 */
static size_t ll_decrement_size(LList *list)
{
    return --list->size;
}

/**
 * Create and initialize a new linked list
 * @returns Pointer to newly allocated LList, or NULL on allocation failure
 */
LList *ll_create_list(void)
{
    LList *cur = malloc(sizeof(LList));
    if (cur == NULL)
    {
        perror("Cannot create a new linked list");
        return NULL;
    }
    cur->head = NULL;
    cur->tail = NULL;
    cur->size = 0;
    return cur;
}

/**
 * Deallocate linked list and all nodes
 * @param[in] list Pointer to the list to destroy (can be NULL)
 * @note Frees all nodes but not the element data they point to
 */
void ll_destroy_list(LList *list)
{
    if (list == NULL)
        return;

    LLNode *cur = list->head;
    while (cur != NULL)
    {
        LLNode *next = cur->next;
        free(cur);
        cur = next;
    }
    free(list);
}

/**
 * Create a new linked list node
 * @param[in] prev Pointer to previous node (NULL if first node)
 * @param[in] elem Pointer to element data
 * @param[in] type Type of the element
 * @param[in] next Pointer to next node (NULL if last node)
 * @returns Pointer to newly allocated LLNode, or NULL on allocation failure
 */
static LLNode *ll_create_node(LLNode *prev, void *elem, LLNodeType type, LLNode *next)
{
    LLNode *node = malloc(sizeof(LLNode));
    if (node == NULL)
    {
        perror("Cannot create a new node");
        return NULL;
    }
    node->prev = prev;
    node->elem = elem;
    node->type = type;
    node->next = next;
    return node;
}

/**
 * Remove a node from the list and free its memory
 * @param[in] list Pointer to the list
 * @param[in] node Pointer to node to remove
 * @returns The new list size after removal
 * @note Does not free the element data, only the node structure
 */
static size_t ll_remove_node(LList *list, LLNode *node)
{
    free(node);
    return ll_decrement_size(list);
}

/**
 * Get node at specified index with optimized traversal
 * @param[in] list Pointer to the list
 * @param[in] idx Zero-based index
 * @returns Pointer to node if found, NULL if index out of bounds
 * @note Traverses from head if idx < size/2, from tail otherwise for O(n/2) worst case
 */
LLNode *ll_get(LList *list, size_t idx)
{
    if (list->size <= idx)
    {
        printf("Index %zu not valid because size is %zu\n", idx, list->size);
        return NULL;
    }

    LLNode *res = NULL;
    if (idx < list->size / 2)
    {
        // Traverse from head (closer to target)
        res = list->head;
        for (size_t ii = 0; ii < idx; ii++)
            res = res->next;
    }
    else
    {
        // Traverse from tail (closer to target)
        res = list->tail;
        for (size_t ii = 0; ii < (list->size - 1 - idx); ii++)
            res = res->prev;
    }

    return res;
}

/**
 * Insert element at specified index
 * @param[in] list Pointer to the list
 * @param[in] elem Pointer to element data
 * @param[in] type Type of the element
 * @param[in] idx Zero-based index (0 = prepend, size = append)
 * @returns Pointer to newly created node, or NULL on failure
 * @note Handles three cases: empty list, insertion at head/tail, insertion in middle
 */
LLNode *ll_add(LList *list, void *elem, LLNodeType type, size_t idx)
{
    if (idx > list->size)
    {
        printf("Index %zu out of bounds (size: %zu)\n", idx, list->size);
        return NULL;
    }

    // Insert at head (idx == 0)
    if (idx == 0)
    {
        if (list->head == NULL)
        {
            // List is empty - create first node
            LLNode *node = ll_create_node(NULL, elem, type, NULL);
            if (node == NULL)
                return NULL;
            list->head = node;
            list->tail = node;
            ll_increment_size(list);
            return node;
        }

        // List not empty - prepend to head
        LLNode *node = ll_create_node(NULL, elem, type, list->head);
        if (node == NULL)
            return NULL;
        list->head->prev = node;
        list->head = node;
        ll_increment_size(list);
        return node;
    }

    // Insert at tail (idx == size)
    if (idx == list->size)
    {
        LLNode *node = ll_create_node(list->tail, elem, type, NULL);
        if (node == NULL)
            return NULL;
        list->tail->next = node;
        list->tail = node;
        ll_increment_size(list);
        return node;
    }

    // Insert in middle
    LLNode *cur = ll_get(list, idx);
    if (cur == NULL)
    {
        printf("Cannot insert element in the list\n");
        return NULL;
    }

    LLNode *node = ll_create_node(cur->prev, elem, type, cur);
    if (node == NULL)
        return NULL;

    cur->prev->next = node;
    cur->prev = node;
    ll_increment_size(list);
    return node;
}

/**
 * Remove element at specified index
 * @param[in] list Pointer to the list
 * @param[in] idx Zero-based index
 * @returns New size of list after removal, or (size_t)-1 on failure
 * @note Handles edge cases: removing head, tail, or middle node
 */
size_t ll_remove(LList *list, size_t idx)
{
    LLNode *node = ll_get(list, idx);
    if (node == NULL)
    {
        printf("Cannot remove element from the list\n");
        return (size_t)-1;
    }

    LLNode *prevNode = node->prev;
    LLNode *nextNode = node->next;

    // Update head if removing first node
    if (prevNode == NULL)
        list->head = nextNode;
    else
        prevNode->next = nextNode;

    // Update tail if removing last node
    if (nextNode == NULL)
        list->tail = prevNode;
    else
        nextNode->prev = prevNode;

    return ll_remove_node(list, node);
}

/**
 * Print list from head to tail
 * @param[in] list Pointer to the list
 * @note Prints elements space-separated to stdout, format depends on type
 */
void ll_print_list(const LList *list)
{
    LLNode *cur = list->head;
    for (size_t ii = 0; ii < list->size; ii++)
    {
        if (cur->type == LL_TYPE_CHAR)
        {
            char *ele = (char *)cur->elem;
            printf("%s ", ele);
        }
        else if (cur->type == LL_TYPE_INT16)
        {
            int16_t *ele = (int16_t *)cur->elem;
            printf("%d ", *ele);
        }
        else if (cur->type == LL_TYPE_INT32)
        {
            int32_t *ele = (int32_t *)cur->elem;
            printf("%d ", *ele);
        }
        else if (cur->type == LL_TYPE_INT64)
        {
            int64_t *ele = (int64_t *)cur->elem;
            printf("%ld ", *ele);
        }

        cur = cur->next;
    }
    printf("\n");
}

/**
 * Print list from tail to head
 * @param[in] list Pointer to the list
 * @note Prints elements space-separated to stdout, format depends on type
 */
void ll_print_list_reverse(const LList *list)
{
    LLNode *cur = list->tail;
    for (size_t ii = 0; ii < list->size; ii++)
    {
        if (cur->type == LL_TYPE_CHAR)
        {
            char *ele = (char *)cur->elem;
            printf("%s ", ele);
        }
        else if (cur->type == LL_TYPE_INT16)
        {
            int16_t *ele = (int16_t *)cur->elem;
            printf("%d ", *ele);
        }
        else if (cur->type == LL_TYPE_INT32)
        {
            int32_t *ele = (int32_t *)cur->elem;
            printf("%d ", *ele);
        }
        else if (cur->type == LL_TYPE_INT64)
        {
            int64_t *ele = (int64_t *)cur->elem;
            printf("%ld ", *ele);
        }

        cur = cur->prev;
    }
    printf("\n");
}

/**
 * Check if list is empty
 * @param[in] list Pointer to the list
 * @returns true if empty or NULL, false otherwise
 */
int ll_is_empty(const LList *list)
{
    return (list == NULL || list->size == 0) ? 1 : 0;
}

/**
 * Get size of list
 * @param[in] list Pointer to the list
 * @returns Number of elements in list, 0 if NULL
 */
size_t ll_get_size(const LList *list)
{
    return list == NULL ? 0 : list->size;
}