/**
 * Void Linked list implementation
 * In this case the element is a void*
 */
#include <stdlib.h>
#include <stdint.h>
#include "linked-list.h"

/**
 * Increment list size
 * @param[in] list A not NULL Linkedlist
 * @returns true
 */
static bool increment_size(LinkedList *list)
{
    list->size++;
    return true;
}

/**
 * Decrement list size
 * @param[in] list A not NULL Linkedlist
 * @returns true
 */
static bool decrement_size(LinkedList *list)
{
    list->size--;
    return true;
}

/**
 * @copydoc create_list
 */
LinkedList *create_list()
{
    LinkedList *cur = malloc(sizeof(LinkedList));
    if (cur == NULL)
    {
        perror("Cannot create a new linked list");
        exit(1);
    }
    cur->head = NULL;
    cur->tail = NULL;
    cur->size = 0;
    return cur;
}

/**
 * @copydoc destroy_list
 */
void destroy_list(LinkedList *list)
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
 * Create a linkedlist node
 * @param[in] prev prev node
 * @param[in] elem current value
 * @param[in] type element type
 * @param[in] next next node
 */
static LLNode *create_node(LLNode *prev, void *elem, LLNodeType type, LLNode *next)
{
    LLNode *node = malloc(sizeof(LLNode));
    if (node == NULL)
    {
        perror("Cannot create a new node");
        exit(1);
    }
    node->prev = prev;
    node->elem = elem;
    node->next = next;
    node->type = type;
    return node;
}

/**
 * Remove a node from a list
 * @param[in] list
 * @param[in] node
 */
static bool remove_node(LinkedList *list, LLNode *node)
{
    free(node);
    return decrement_size(list);
}

/**
 * @copydoc get_ele
 */
LLNode *get_ele(LinkedList *list, size_t idx)
{
    if (list->size <= idx)
    {
        printf("Index %ld not valid because size is %ld\n", idx, list->size);
        return NULL;
    }

    LLNode *res = NULL;
    if (idx < list->size / 2)
    {
        // traverse from head
        res = list->head;
        for (size_t ii = 0; ii < idx; ii++)
            res = res->next;
    }
    else
    {
        // traverse from tail
        res = list->tail;
        for (size_t ii = 0; ii < (list->size - 1 - idx); ii++)
            res = res->prev;
    }

    return res;
}

/**
 * @copydoc add_ele
 */
bool add_ele(LinkedList *list, void *elem, char type, size_t idx)
{
    if (idx > list->size)
    {
        printf("Index %ld out of bounds (size: %ld)\n", idx, list->size);
        return false;
    }

    if (idx == 0)
    {
        // add to head
        if (list->head == NULL)
        {
            // list is empty create a new node
            LLNode *node = create_node(NULL, elem, type, NULL);
            list->head = node;
            list->tail = node;
            return increment_size(list);
        }

        // create a new node that points to the head
        LLNode *node = create_node(NULL, elem, type, list->head);
        // reach the head and assign prev to the new node
        list->head->prev = node;
        // change the head with the new node
        list->head = node;
        return increment_size(list);
    }

    // idx is not 0
    if (idx == list->size)
    {
        // is the last element
        LLNode *node = create_node(list->tail, elem, type, NULL);
        list->tail->next = node;
        list->tail = node;
        return increment_size(list);
    }

    // is in the middle
    LLNode *cur = get_ele(list, idx);
    if (cur == NULL)
    {
        printf("Cannot insert element in the list\n");
        return false;
    }
    LLNode *node = create_node(cur->prev, elem, type, cur);
    cur->prev->next = node;
    cur->prev = node;
    return increment_size(list);
}

/**
 * @copydoc remove_ele
 */
bool remove_ele(LinkedList *list, size_t idx)
{
    // is in the middle
    LLNode *node = get_ele(list, idx);
    if (node == NULL)
    {
        printf("Cannot remove element in the list\n");
        return false;
    }
    LLNode *prevNode = node->prev;
    LLNode *nextNode = node->next;

    if (prevNode == NULL)
        list->head = nextNode; // removing the head
    else
        prevNode->next = nextNode;

    if (nextNode == NULL)
        list->tail = prevNode; // removing thetail
    else
        nextNode->prev = prevNode;

    return remove_node(list, node);
}

/**
 * @copydoc print_list
 */
void print_list(const LinkedList *list)
{
    LLNode *cur = list->head;
    for (size_t ii = 0; ii < list->size; ii++)
    {
        if (cur->type == TYPE_CHAR)
        {
            char *ele = (char *)cur->elem;
            printf("%s ", ele);
        }
        else if (cur->type == TYPE_INT16)
        {
            int16_t *ele = (int16_t *)cur->elem;
            printf("%d ", *ele);
        }
        else if (cur->type == TYPE_INT32)
        {
            int32_t *ele = (int32_t *)cur->elem;
            printf("%d ", *ele);
        }
        else if (cur->type == TYPE_INT64)
        {
            int64_t *ele = (int64_t *)cur->elem;
            printf("%ld ", *ele);
        }

        cur = cur->next;
    }
}

/**
 * @copydoc print_list_reverse
 */
void print_list_reverse(const LinkedList *list)
{
    LLNode *cur = list->tail;
    for (size_t ii = 0; ii < list->size; ii++)
    {
        if (cur->type == 'c')
        {
            char *ele = (char *)cur->elem;
            printf("%s ", ele);
        }
        else if (cur->type == 's')
        {
            uint16_t *ele = (uint16_t *)cur->elem;
            printf("%u ", *ele);
        }
        else if (cur->type == 'i')
        {
            uint32_t *ele = (uint32_t *)cur->elem;
            printf("%u ", *ele);
        }
        else if (cur->type == 'l')
        {
            uint64_t *ele = (uint64_t *)cur->elem;
            printf("%lu ", *ele);
        }
        cur = cur->prev;
    }
}