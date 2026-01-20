#include "llist.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Increment list size by one
 *
 * Internal helper to atomically increment the list size.
 *
 * @param[in] list A valid (non-NULL) LList pointer
 * @return The new list size after incrementing
 */
static size_t ll_increment_size(LList *list) {
    return ++list->size;
}

/**
 * @brief Decrement list size by one
 *
 * Internal helper to atomically decrement the list size.
 *
 * @param[in] list A valid (non-NULL) LList pointer
 * @return The new list size after decrementing
 */
static size_t ll_decrement_size(LList *list) {
    return --list->size;
}

/**
 * @copydoc ll_create
 */
LList *ll_create(void) {
    LList *cur = malloc(sizeof(LList));
    if (cur == NULL) {
        perror("[ll_create] Cannot create a new linked list");
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
void ll_destroy(LList *list) {
    if (list == NULL)
        return;

    LLNode *cur = list->head;
    while (cur != NULL) {
        LLNode *next = cur->next;
        free(cur);
        cur = next;
    }
    free(list);
}

/**
 * @copydoc ll_destroy_deep
 */
void ll_destroy_deep(LList *list) {
    if (list == NULL)
        return;

    LLNode *cur = list->head;
    while (cur != NULL) {
        LLNode *next = cur->next;

        // Free the element data if it exists
        if (cur->elem != NULL)
            free(cur->elem);

        // Free the node itself
        free(cur);
        cur = next;
    }

    // Free the list structure
    free(list);
}

/**
 * @brief Create a new linked list node
 *
 * Internal helper to allocate and initialize a new node with the given data.
 * Links the node to its neighbors in the list.
 *
 * @param[in] prev Pointer to previous node (NULL if first node)
 * @param[in] elem Pointer to element data
 * @param[in] elem_size Number of elements (1 for single value, >1 for array)
 * @param[in] type Type of the element
 * @param[in] next Pointer to next node (NULL if last node)
 *
 * @return Pointer to newly allocated LLNode, or NULL on allocation failure
 */
static LLNode *ll_create_node(LLNode *prev, void *elem, uint32_t elem_size, LLNodeType type, LLNode *next) {
    LLNode *node = malloc(sizeof(LLNode));
    if (node == NULL) {
        perror("[ll_create_node] Cannot create a new node");
        return NULL;
    }
    // elem_size is unsigned, no checks needed

    node->prev = prev;
    node->elem = elem;
    node->elem_size = elem_size;
    node->type = type;
    node->next = next;
    return node;
}

/**
 * @brief Internal removal implementation with optional deep cleanup
 *
 * Handles node removal logic with optional element data freeing.
 * Unlinks the node, optionally frees element data, frees the node, and updates size.
 *
 * @param[in] list Pointer to the list
 * @param[in] idx Index of node to remove
 * @param[in] deep If 1, free element data; if 0, keep element data
 *
 * @return 1 on success, 0 on failure
 */
static int ll_remove_internal(LList *list, size_t idx, int deep) {
    LLNode *node = ll_get(list, idx);
    if (node == NULL) {
        fprintf(stderr, "[ll_remove%s] Cannot remove element from the list\n",
                deep ? "_deep" : "");
        return 0;
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

    // Optionally free the element data
    if (deep && node->elem != NULL)
        free(node->elem);

    // Free the node and decrement size
    free(node);
    ll_decrement_size(list);

    return 1;
}

/**
 * @copydoc ll_get
 */
LLNode *ll_get(LList *list, size_t idx) {
    if (list->size <= idx) {
        fprintf(stderr, "[ll_get] Index %zu not valid because size is %zu\n", idx, list->size);
        return NULL;
    }

    LLNode *res = NULL;
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

/**
 * @copydoc ll_add
 */
LLNode *ll_add(LList *list, void *elem, uint32_t elem_size, LLNodeType type, size_t idx) {
    if (idx > list->size) {
        fprintf(stderr, "[ll_add] Index %zu out of bounds (size: %zu)\n", idx, list->size);
        return NULL;
    }

    // Insert at head (idx == 0)
    if (idx == 0) {
        if (list->head == NULL) {
            // List is empty - create first node
            LLNode *node = ll_create_node(NULL, elem, elem_size, type, NULL);
            if (node == NULL)
                return NULL;
            list->head = node;
            list->tail = node;
            ll_increment_size(list);
            return node;
        }

        // List not empty - prepend to head
        LLNode *node = ll_create_node(NULL, elem, elem_size, type, list->head);
        if (node == NULL)
            return NULL;
        list->head->prev = node;
        list->head = node;
        ll_increment_size(list);
        return node;
    }

    // Insert at tail (idx == size)
    if (idx == list->size) {
        LLNode *node = ll_create_node(list->tail, elem, elem_size, type, NULL);
        if (node == NULL)
            return NULL;
        list->tail->next = node;
        list->tail = node;
        ll_increment_size(list);
        return node;
    }

    // Insert in middle
    LLNode *cur = ll_get(list, idx);
    if (cur == NULL) {
        fprintf(stderr, "[ll_add] Cannot insert element in the list\n");
        return NULL;
    }

    LLNode *node = ll_create_node(cur->prev, elem, elem_size, type, cur);
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
int ll_remove(LList *list, size_t idx) {
    return ll_remove_internal(list, idx, 0);
}

/**
 * @copydoc ll_remove_deep
 */
int ll_remove_deep(LList *list, size_t idx) {
    return ll_remove_internal(list, idx, 1);
}

/** @copydoc ll_print_node */
void ll_print_node(const LLNode *cur) {
    if (cur == NULL || cur->elem == NULL) {
        fprintf(stderr, "[ll_print_node] Invalid node\n");
        return;
    }

    if (cur->elem_size == 0) {
        fprintf(stderr, "[ll_print_node] Empty node\n");
        return;
    }

    if (cur->type == LL_TYPE_STR) {
        char *ele = (char *)cur->elem;
        printf("%s ", ele);
    } else if (cur->type == LL_TYPE_INT8) {
        int8_t *ele = (int8_t *)cur->elem;
        for (size_t kk = 0; kk < cur->elem_size; kk++)
            printf("%d ", ele[kk]);
    } else if (cur->type == LL_TYPE_INT16) {
        int16_t *ele = (int16_t *)cur->elem;
        for (size_t kk = 0; kk < cur->elem_size; kk++)
            printf("%d ", ele[kk]);
    } else if (cur->type == LL_TYPE_INT32) {
        int32_t *ele = (int32_t *)cur->elem;
        for (size_t kk = 0; kk < cur->elem_size; kk++)
            printf("%d ", ele[kk]);
    } else if (cur->type == LL_TYPE_INT64) {
        int64_t *ele = (int64_t *)cur->elem;
        for (size_t kk = 0; kk < cur->elem_size; kk++)
            printf("%ld ", ele[kk]);
    }
    printf("\n");
}

/**
 * @copydoc ll_print
 */
void ll_print(const LList *list) {
    LLNode *cur = list->head;
    for (size_t ii = 0; ii < list->size; ii++) {
        ll_print_node(cur);
        cur = cur->next;
    }
}

/**
 * @copydoc ll_print_reverse
 */
void ll_print_reverse(const LList *list) {
    LLNode *cur = list->tail;
    for (size_t ii = 0; ii < list->size; ii++) {
        ll_print_node(cur);
        cur = cur->prev;
    }
}

/**
 * @copydoc ll_is_empty
 */
int ll_is_empty(const LList *list) {
    return (list == NULL || list->size == 0) ? 1 : 0;
}

/**
 * @copydoc ll_get_size
 */
size_t ll_get_size(const LList *list) {
    return list == NULL ? 0 : list->size;
}

/**
 * @copydoc ll_append
 */
LLNode *ll_append(LList *list, void *elem, uint32_t elem_size, LLNodeType type) {
    return ll_add(list, elem, elem_size, type, list->size);
}

/**
 * @copydoc ll_prepend
 */
LLNode *ll_prepend(LList *list, void *elem, uint32_t elem_size, LLNodeType type) {
    return ll_add(list, elem, elem_size, type, 0);
}

/**
 * @copydoc ll_pop
 */
int ll_pop(LList *list, LLNode *res) {
    LLNode *node = ll_get(list, list->size - 1);
    if (node == NULL) {
        fprintf(stderr, "[ll_pop] Cannot pop element\n");
        return 0;
    }

    res->prev = NULL;
    res->elem = node->elem;
    res->next = NULL;
    res->type = node->type;
    res->elem_size = node->elem_size;

    ll_remove(list, list->size - 1);
    return 1;
}

/**
 * @copydoc ll_get_head
 */
LLNode *ll_get_head(LList *list) {
    return list->head;
}

/**
 * @copydoc ll_get_tail
 */
LLNode *ll_get_tail(LList *list) {
    return list->tail;
}

/**
 * @copydoc ll_change
 */
LLNode *ll_change(LList *list, void *elem, uint32_t elem_size, LLNodeType type, size_t idx) {
    // idx and elem_size are unsigned, no need to check
    LLNode *node = ll_get(list, idx);
    if (node == NULL) {
        fprintf(stderr, "[ll_change] Node at index %ld not found", idx);
        return NULL;
    }
    node->elem = elem;
    node->elem_size = elem_size;
    node->type = type;
    return node;
}