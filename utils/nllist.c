#include "nllist.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Increment list size by one
 * @param[in] list A valid (non-NULL) NLList pointer
 * @returns The new list size after incrementing
 */
static size_t nll_increment_size(NLList *list) {
    return ++list->size;
}

/**
 * Decrement list size by one
 * @param[in] list A valid (non-NULL) NLList pointer
 * @returns The new list size after decrementing
 */
static size_t nll_decrement_size(NLList *list) {
    return --list->size;
}

/** @copydoc nll_create */
NLList *nll_create(void) {
    NLList *cur = malloc(sizeof(NLList));
    if (cur == NULL) {
        fprintf(stderr, "[nll_create] Cannot create a new linked list");
        return NULL;
    }
    cur->head = NULL;
    cur->tail = NULL;
    cur->size = 0;
    return cur;
}

/** @copydoc nll_destroy */
void nll_destroy(NLList *list) {
    if (list == NULL)
        return;

    NLLNode *cur = list->head;
    while (cur != NULL) {
        NLLNode *next = cur->next;
        free(cur);
        cur = next;
    }
    free(list);
}

/**
 * Create a new numeric linked list node
 * @param[in] prev Pointer to previous node (NULL if first node)
 * @param[in] elem Numeric value to store
 * @param[in] next Pointer to next node (NULL if last node)
 * @returns Pointer to newly allocated NLLNode, or NULL on allocation failure
 */
static NLLNode *nll_create_node(NLLNode *prev, size_t elem, NLLNode *next) {
    NLLNode *node = malloc(sizeof(NLLNode));
    if (node == NULL) {
        perror("[nll_create_node] Cannot create a new node");
        return NULL;
    }
    node->prev = prev;
    node->elem = elem;
    node->next = next;
    return node;
}

/**
 * Remove a node from the list and free its memory
 * @param[in] list Pointer to the list
 * @param[in] node Pointer to node to remove
 * @returns The new list size after removal
 */
static size_t nll_remove_node(NLList *list, NLLNode *node) {
    free(node);
    return nll_decrement_size(list);
}

/** @copydoc nll_get */
NLLNode *nll_get(const NLList *list, size_t idx) {
    if (list == NULL || list->size <= idx) {
        fprintf(stderr, "[nll_get] Index %zu not valid because size is %zu\n", idx, list ? list->size : 0);
        return NULL;
    }

    NLLNode *res = NULL;
    if (idx < list->size / 2) {
        // Traverse from head (closer to target)
        res = list->head;
        for (size_t ii = 0; ii < idx; ii++)
            res = res->next;
    } else {
        // Traverse from tail (closer to target)
        res = list->tail;
        for (size_t ii = 0; ii < (list->size - 1 - idx); ii++)
            res = res->prev;
    }

    return res;
}

/** @copydoc nll_add */
NLLNode *nll_add(NLList *list, size_t elem, size_t idx) {
    if (list == NULL || idx > list->size) {
        fprintf(stderr, "[nll_add] Index %zu out of bounds (size: %zu)\n", idx, list ? list->size : 0);
        return NULL;
    }

    // Case 1: Insert at head (idx == 0)
    if (idx == 0) {
        if (list->head == NULL) {
            // List is empty - create first node
            NLLNode *node = nll_create_node(NULL, elem, NULL);
            if (node == NULL)
                return NULL;
            list->head = node;
            list->tail = node;
            nll_increment_size(list);
            return node;
        }

        // List not empty - prepend to head
        NLLNode *node = nll_create_node(NULL, elem, list->head);
        if (node == NULL)
            return NULL;
        list->head->prev = node;
        list->head = node;
        nll_increment_size(list);
        return node;
    }

    // Case 2: Insert at tail (idx == size)
    if (idx == list->size) {
        NLLNode *node = nll_create_node(list->tail, elem, NULL);
        if (node == NULL)
            return NULL;
        list->tail->next = node;
        list->tail = node;
        nll_increment_size(list);
        return node;
    }

    // Case 3: Insert in middle
    NLLNode *cur = nll_get(list, idx);
    if (cur == NULL) {
        fprintf(stderr, "[nll_add] Cannot insert element in the list\n");
        return NULL;
    }

    NLLNode *node = nll_create_node(cur->prev, elem, cur);
    if (node == NULL)
        return NULL;

    cur->prev->next = node;
    cur->prev = node;
    nll_increment_size(list);
    return node;
}

/** @copydoc nll_remove */
int nll_remove(NLList *list, size_t idx) {
    if (list == NULL) {
        fprintf(stderr, "[nll_remove] Cannot remove element from NULL list\n");
        return 0;
    }

    NLLNode *node = nll_get(list, idx);
    if (node == NULL) {
        fprintf(stderr, "[nll_remove] Cannot remove element from the list\n");
        return 0;
    }

    NLLNode *prevNode = node->prev;
    NLLNode *nextNode = node->next;

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

    nll_remove_node(list, node);
    return 1;
}

/** @copydoc nll_print */
void nll_print(const NLList *list) {
    if (list == NULL) {
        fprintf(stderr, "[nll_print] (empty list)\n");
        return;
    }

    NLLNode *cur = list->head;
    for (size_t ii = 0; ii < list->size; ii++) {
        printf("%zu ", cur->elem);
        cur = cur->next;
    }
    printf("\n");
}

/** @copydoc nll_print_reverse */
void nll_print_reverse(const NLList *list) {
    if (list == NULL) {
        fprintf(stderr, "[nll_print_reverse] (empty list)\n");
        return;
    }

    NLLNode *cur = list->tail;
    for (size_t ii = 0; ii < list->size; ii++) {
        printf("%zu ", cur->elem);
        cur = cur->prev;
    }
    printf("\n");
}

/** @copydoc nll_is_empty */
int nll_is_empty(const NLList *list) {
    return (list == NULL || list->size == 0) ? 1 : 0;
}

/** @copydoc nll_get_size */
size_t nll_get_size(const NLList *list) {
    return list == NULL ? 0 : list->size;
}

/** @copydoc nll_prepend */
NLLNode *nll_prepend(NLList *list, size_t elem) {
    return nll_add(list, elem, 0);
}

/** @copydoc nll_append */
NLLNode *nll_append(NLList *list, size_t elem) {
    return nll_add(list, elem, list->size);
}

/** @copydoc nll_pop */
int nll_pop(NLList *list, size_t *res) {
    NLLNode *node = nll_get(list, list->size - 1);
    if (node == NULL) {
        fprintf(stderr, "[nll_pop] Cannot get element\n");
        return 0;
    }
    *res = node->elem;
    nll_remove(list, list->size - 1);
    return 1;
}

/** @copydoc nll_get_value */
int nll_get_value(NLList *list, size_t idx, size_t *res) {
    NLLNode *node = nll_get(list, idx);
    if (node == NULL) {
        fprintf(stderr, "[nll_get_value] Cannot get element\n");
        return 0;
    }
    *res = node->elem;
    return 1;
}

/** @copydoc nll_get_head */
NLLNode *nll_get_head(NLList *list) {
    return list->head;
}

/** @copydoc nll_get_value_head */
int nll_get_value_head(NLList *list, size_t *res) {
    NLLNode *head = nll_get_head(list);
    if (head == NULL) {
        fprintf(stderr, "[nll_get_value_head] Cannot get head\n");
        return 0;
    }
    *res = head->elem;
    return 1;
}

/** @copydoc nll_get_tail */
NLLNode *nll_get_tail(NLList *list) {
    return list->tail;
}

/** @copydoc nll_get_value_tail */
int nll_get_value_tail(NLList *list, size_t *res) {
    NLLNode *tail = nll_get_tail(list);
    if (tail == NULL) {
        fprintf(stderr, "[nll_get_value_tail] Cannot get tail\n");
        return 0;
    }
    *res = tail->elem;
    return 1;
}