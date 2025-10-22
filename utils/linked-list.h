#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdio.h>
#include <stdbool.h>

typedef enum
{
    TYPE_CHAR,  // char pointer (string)
    TYPE_INT16, // 16 bits pointer
    TYPE_INT32, // 32 bits pointer
    TYPE_INT64  // 64 bits pointer
} LLNodeType;

struct llnode
{
    struct llnode *prev;
    void *elem;
    LLNodeType type;
    struct llnode *next;
};

typedef struct llnode LLNode;

typedef struct
{
    LLNode *head;
    LLNode *tail;
    size_t size;
} LinkedList;

/**
 * Create the linked list
 */
LinkedList *create_list();

/**
 * Deallocate linked list and all nodes
 * @param[in] list
 */
void destroy_list(LinkedList *list);

/**
 * Get an element of a valid LinkedList at index idx
 * @param[in] list
 * @param[in] idx
 * @returns LLNode if found else NULL
 */
LLNode *get_ele(LinkedList *list, size_t idx);

/**
 * Add an element into the list
 * @param[in] list
 * @param[in] elem
 * @param[in] type
 * @param[in] idx
 */
bool add_ele(LinkedList *list, void *elem, char type, size_t idx);

/**
 * Remove an element from a list
 * @param[in] list
 * @param[in] idx
 */
bool remove_ele(LinkedList *list, size_t idx);

/**
 * Traverse list using head
 * @param[in] list
 */
void print_list(const LinkedList *list);

/**
 * Traverse list using tail
 * @param[in] list
 */
void print_list_reverse(const LinkedList *list);
#endif // LINKEDLIST_H