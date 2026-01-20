#include "nalist.h"
#include <stdint.h>
#include <stdio.h>

/** @copydoc nal_create */
NAList *nal_create(size_t capacity) {
    if (capacity == 0) {
        fprintf(stderr, "[nal_create] Invalid capacity\n");
        return NULL;
    }

    NAList *list = malloc(sizeof(NAList));
    if (list == NULL) {
        perror("[nal_create] Cannot create a new array list");
        return NULL;
    }
    list->capacity = capacity;
    list->size = 0;
    list->data = malloc(sizeof(size_t) * capacity);
    if (list->data == NULL) {
        perror("[nal_create] Cannot create a new array list");
        free(list);
        return NULL;
    }
    return list;
}

/** @copydoc nal_destroy */
void nal_destroy(NAList *list) {
    if (list == NULL)
        return;

    if (list->data != NULL)
        free(list->data);

    list->capacity = 0;
    list->size = 0;
    free(list);
}

/**
 * @brief Resize data (grow)
 *
 * Resize data with capacity * 2. If new capacity is equals to old one then is not grown
 *
 * @param[in] list List pointer
 * @return new capacity or 0 in case of error
 */
static size_t nal_grow(NAList *list) {
    // resize with double capacity
    size_t new_capacity = list->capacity * 2;
    void *temp = realloc(list->data, sizeof(size_t) * new_capacity);
    if (!temp) {
        perror("[nal_grow] Reallocation failed! The old data are still valid");
        return 0;
    }
    // capacity logic
    list->data = temp;
    list->capacity = new_capacity;
    return list->capacity;
}

/**
 * @brief Resize data (shrink)
 *
 * Resize data with capacity / 2.
 *
 * @param[in] list List pointer
 * @return new capacity or 0 in case of error
 */
static size_t nal_shrink(NAList *list) {
    // resize with half capacity
    size_t new_capacity = list->capacity / 2;

    // don't shrink
    if (new_capacity < 2)
        return list->capacity;

    void *temp = realloc(list->data, sizeof(size_t) * new_capacity);
    if (!temp) {
        perror("[nal_shrink] Reallocation failed! The old data are still valid");
        return 0;
    }
    // capacity logic
    list->data = temp;
    list->capacity = new_capacity;
    return list->capacity;
}

/** @copydoc nal_insert */
int nal_insert(NAList *list, size_t ele, size_t idx) {
    if (list == NULL) {
        fprintf(stderr, "[nal_insert] List is NULL\n");
        return 0;
    }

    if (idx > list->size) {
        fprintf(stderr, "[nal_insert] Index out of bound\n");
        return 0;
    }

    // check capacity
    if (list->size == list->capacity) {
        // if capacity is full then resize++
        if (!nal_grow(list)) {
            fprintf(stderr, "[nal_insert] Cannot append a new element\n");
            return 0;
        }
    }

    // right shift
    for (size_t ii = list->size; ii > idx; ii--) {
        list->data[ii] = list->data[ii - 1];
    }
    list->data[idx] = ele;
    list->size++;

    return 1;
}

/** @copydoc nal_append */
int nal_append(NAList *list, size_t ele) {
    return nal_insert(list, ele, list->size);
}

/** @copydoc nal_prepend */
int nal_prepend(NAList *list, size_t ele) {
    return nal_insert(list, ele, 0);
}

/** @copydoc nal_get */
int nal_get(NAList *list, size_t idx, size_t *res) {
    if (list == NULL) {
        fprintf(stderr, "[nal_get] List is NULL\n");
        return 0;
    }

    if (res == NULL) {
        fprintf(stderr, "[nal_get] Result pointer is NULL\n");
        return 0;
    }

    if (idx >= list->size) {
        fprintf(stderr, "[nal_get] Index out of bound\n");
        return 0;
    }
    *res = list->data[idx];
    return 1;
}

/** @copydoc nal_remove */
int nal_remove(NAList *list, size_t idx) {
    if (list == NULL) {
        fprintf(stderr, "[nal_remove] List is NULL\n");
        return 0;
    }

    if (idx >= list->size) {
        fprintf(stderr, "[nal_remove] Index out of bound\n");
        return 0;
    }

    // check capacity
    if (list->size == (list->capacity / 2)) {
        // if capacity is double the size then resize--
        if (!nal_shrink(list)) {
            fprintf(stderr, "[nal_remove] Cannot remove the element with index %zu\n", idx);
            return 0;
        }
    }

    // left shift
    for (size_t ii = idx; ii < list->size - 1; ii++) {
        list->data[ii] = list->data[ii + 1];
    }
    list->size--;

    return 1;
}

/** @copydoc nal_print */
void nal_print(NAList *list) {
    if (list == NULL)
        return;
    for (size_t ii = 0; ii < list->size; ii++)
        printf("%zu\n", list->data[ii]);
}