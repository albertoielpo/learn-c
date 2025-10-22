/**
 * @author Alberto Ielpo
 * Numeric Linked list implementation
 * In this case the element is a number with size_t as size
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct nllnode
{
    struct nllnode *prev;
    size_t elem;
    struct nllnode *next;
};

// numeric linked list node
typedef struct nllnode NLLNode;

typedef struct
{
    NLLNode *head;
    NLLNode *tail;
    size_t size;
} NLinkedList;

/**
 * Increment list size
 * @param[in] list A not NULL LinkedList
 * @returns true
 */
static bool increment_size(NLinkedList *list)
{
    list->size++;
    return true;
}

/**
 * Decrement list size
 * @param[in] list A not NULL LinkedList
 * @returns true
 */
static bool decrement_size(NLinkedList *list)
{
    list->size--;
    return true;
}

/**
 * Create the linked list
 */
NLinkedList *create_nlist()
{
    NLinkedList *cur = malloc(sizeof(NLinkedList));
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
 * Deallocate linked list and all nodes
 * @param[in] list
 */
void destroy_nlist(NLinkedList *list)
{
    if (list == NULL)
        return;

    NLLNode *cur = list->head;
    while (cur != NULL)
    {
        NLLNode *next = cur->next;
        free(cur);
        cur = next;
    }
    free(list);
}

/**
 * Create a NLinkedList node
 * @param[in] prev prev node
 * @param[in] elem current value
 * @param[in] next next node
 */
static NLLNode *create_nnode(NLLNode *prev, size_t elem, NLLNode *next)
{
    NLLNode *node = malloc(sizeof(NLLNode));
    if (node == NULL)
    {
        perror("Cannot create a new node");
        exit(1);
    }
    node->prev = prev;
    node->elem = elem;
    node->next = next;
    return node;
}

/**
 * Remove a node from a list
 * @param[in] list
 * @param[in] node
 */
static bool remove_nnode(NLinkedList *list, NLLNode *node)
{
    free(node);
    return decrement_size(list);
}

/**
 * Get an element of a valid NLinkedList at index idx
 * @param[in] list
 * @param[in] idx
 * @returns NLLNode if found else NULL
 */
NLLNode *get_nele(NLinkedList *list, size_t idx)
{
    if (list->size <= idx)
    {
        printf("Index %ld not valid because size is %ld\n", idx, list->size);
        return NULL;
    }

    NLLNode *res = NULL;
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
 * Add an element into the list
 * @param[in] list
 * @param[in] elem
 * @param[in] idx
 */
bool add_nele(NLinkedList *list, size_t elem, size_t idx)
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
            NLLNode *node = create_nnode(NULL, elem, NULL);
            list->head = node;
            list->tail = node;
            return increment_size(list);
        }

        // create a new node that points to the head
        NLLNode *node = create_nnode(NULL, elem, list->head);
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
        NLLNode *node = create_nnode(list->tail, elem, NULL);
        list->tail->next = node;
        list->tail = node;
        return increment_size(list);
    }

    // is in the middle
    NLLNode *cur = get_nele(list, idx);
    if (cur == NULL)
    {
        printf("Cannot insert element in the list\n");
        return false;
    }
    NLLNode *node = create_nnode(cur->prev, elem, cur);
    cur->prev->next = node;
    cur->prev = node;
    return increment_size(list);
}

/**
 * Remove an element from linked list
 * @param[in] list
 * @param[in] idx
 */
bool remove_nele(NLinkedList *list, size_t idx)
{
    // is in the middle
    NLLNode *node = get_nele(list, idx);
    if (node == NULL)
    {
        printf("Cannot remove element in the list\n");
        return false;
    }
    NLLNode *prevNode = node->prev;
    NLLNode *nextNode = node->next;

    if (prevNode == NULL)
        list->head = nextNode; // removing the head
    else
        prevNode->next = nextNode;

    if (nextNode == NULL)
        list->tail = prevNode; // removing thetail
    else
        nextNode->prev = prevNode;

    return remove_nnode(list, node);
}

/**
 * Traverse list using head
 * @param[in] list
 */
void print_nlist(const NLinkedList *list)
{
    NLLNode *cur = list->head;
    for (size_t ii = 0; ii < list->size; ii++)
    {
        printf("%ld ", cur->elem);
        cur = cur->next;
    }
}

/**
 * Traverse list using tail
 * @param[in] list
 */
void print_nlist_reverse(const NLinkedList *list)
{
    NLLNode *cur = list->tail;
    for (size_t ii = 0; ii < list->size; ii++)
    {
        printf("%ld ", cur->elem);
        cur = cur->prev;
    }
}