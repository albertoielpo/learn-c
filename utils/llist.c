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
 * @copydoc ll_create
 */
LList *ll_create(void)
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
 * @copydoc ll_destroy
 */
void ll_destroy(LList *list)
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
 * @copydoc ll_get
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
 * @copydoc ll_add
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
        perror("Cannot insert element in the list");
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
 * @copydoc ll_remove
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
 * @copydoc ll_print
 */
void ll_print(const LList *list)
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
 * @copydoc ll_print_reverse
 */
void ll_print_reverse(const LList *list)
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
 * @copydoc ll_is_empty
 */
int ll_is_empty(const LList *list)
{
    return (list == NULL || list->size == 0) ? 1 : 0;
}

/**
 * @copydoc ll_get_size
 */
size_t ll_get_size(const LList *list)
{
    return list == NULL ? 0 : list->size;
}

/**
 * @copydoc ll_append
 */
LLNode *ll_append(LList *list, void *elem, LLNodeType type)
{
    return ll_add(list, elem, type, list->size);
}

/**
 * @copydoc ll_prepend
 */
LLNode *ll_prepend(LList *list, void *elem, LLNodeType type)
{
    return ll_add(list, elem, type, 0);
}

/**
 * @copydoc ll_pop
 */
void *ll_pop(LList *list)
{
    LLNode *node = ll_get(list, list->size - 1);
    if (node == NULL)
    {
        perror("Cannot get element");
        return NULL;
    }
    void *elem = node->elem;
    ll_remove(list, list->size - 1);
    return elem;
}

/**
 * @copydoc ll_get_value
 */
void *ll_get_value(LList *list, size_t idx)
{
    LLNode *node = ll_get(list, idx);
    if (node == NULL)
    {
        perror("Cannot get element");
        return NULL;
    }
    return node->elem;
}

/**
 * @copydoc ll_get_head
 */
LLNode *ll_get_head(LList *list)
{
    return list->head;
}

/**
 * @copydoc ll_get_value_head
 */
void *ll_get_value_head(LList *list)
{
    LLNode *head = ll_get_head(list);
    if (head == NULL)
    {
        perror("Cannot get head");
        return NULL;
    }
    return head->elem;
}

/**
 * @copydoc ll_get_tail
 */
LLNode *ll_get_tail(LList *list)
{
    return list->tail;
}

/**
 * @copydoc ll_get_value_tail
 */
void *ll_get_value_tail(LList *list)
{
    LLNode *tail = ll_get_tail(list);
    if (tail == NULL)
    {
        perror("Cannot get tail");
        return NULL;
    }
    return tail->elem;
}