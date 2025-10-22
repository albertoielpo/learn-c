#ifndef NLINKED_LIST_H
#define NLINKED_LIST_H
#include <stdio.h>
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
 * Create the linked list
 */
NLinkedList *create_nlist();

/**
 * Deallocate linked list and all nodes
 * @param[in] list
 */
void destroy_nlist(NLinkedList *list);

/**
 * Get an element of a valid NLinkedList at index idx
 * @param[in] list
 * @param[in] idx
 * @returns NLLNode if found else NULL
 */
NLLNode *get_nele(NLinkedList *list, size_t idx);

/**
 * Add an element into the list
 * @param[in] list
 * @param[in] elem
 * @param[in] idx
 */
bool add_nele(NLinkedList *list, size_t elem, size_t idx);

/**
 * Remove an element from linked list
 * @param[in] list
 * @param[in] idx
 */
bool remove_nele(NLinkedList *list, size_t idx);

/**
 * Traverse list using head
 * @param[in] list
 */
void print_nlist(const NLinkedList *list);

/**
 * Traverse list using tail
 * @param[in] list
 */
void print_nlist_reverse(const NLinkedList *list);
#endif // NLINKED_LIST_H