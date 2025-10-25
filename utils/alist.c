#include <stdio.h>
#include <stdint.h>
#include "alist.h"

/** @copydoc al_create */
AList *al_create(size_t capacity, ALType type)
{
    if (capacity <= 0)
    {
        printf("Invalid capacity\n");
        return NULL;
    }

    AList *list = malloc(sizeof(AList));
    if (list == NULL)
    {
        perror("Cannot create a new array list");
        return NULL;
    }
    list->capacity = capacity;
    list->size = 0;
    list->type = type;
    list->data = malloc(sizeof(void *) * capacity);
    return list;
}

/** @copydoc al_destroy */
void al_destroy(AList *list)
{
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
static size_t al_grow(AList *list)
{
    // resize with double capacity
    size_t new_capacity = list->capacity * 2;
    void *temp = realloc(list->data, sizeof(void *) * new_capacity);
    if (!temp)
    {
        perror("Reallocation failed! The old data are still valid");
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
static size_t al_shrink(AList *list)
{
    // resize with double capacity
    size_t new_capacity = list->capacity / 2;
    void *temp = realloc(list->data, sizeof(void *) * new_capacity);
    if (!temp)
    {
        perror("Reallocation failed! The old data are still valid");
        return 0;
    }
    // capacity logic
    list->data = temp;
    list->capacity = new_capacity;
    return list->capacity;
}

/** @copydoc al_insert */
size_t al_insert(AList *list, void *ele, size_t idx)
{
    if (idx > list->size)
    {
        printf("Index out of bound\n");
        return -1;
    }

    // check capacity
    if (list->size == list->capacity)
    {
        // if capacity is full then resize++
        if (!al_grow(list))
        {
            printf("Cannot append a new element\n");
            return -1;
        }
    }

    // right shift
    for (size_t ii = list->size; ii > idx; ii--)
    {
        list->data[ii] = list->data[ii - 1];
    }
    list->data[idx] = ele;
    list->size++;

    return idx;
}

/** @copydoc al_append */
size_t al_append(AList *list, void *ele)
{
    return al_insert(list, ele, list->size);
}

/** @copydoc al_prepend */
size_t al_prepend(AList *list, void *ele)
{
    return al_insert(list, ele, 0);
}

/** @copydoc al_get */
void *al_get(AList *list, size_t idx)
{
    if (idx >= list->size)
    {
        printf("Index out of bound\n");
        return NULL;
    }
    return list->data[idx];
}

/** @copydoc al_remove */
int al_remove(AList *list, size_t idx)
{
    if (idx > list->size)
    {
        printf("Index out of bound\n");
        return -1;
    }

    // check capacity
    if (list->size == (list->capacity / 2))
    {
        // if capacity is double the size then resize--
        if (!al_shrink(list))
        {
            printf("Cannot remove the element with index %ld\n", idx);
            return 0;
        }
    }

    // left shift
    for (size_t ii = idx; ii < list->size; ii++)
    {
        list->data[ii] = list->data[ii + 1];
    }
    list->size--;

    return 1;
}

/** @copydoc al_print */
void al_print(AList *list)
{
    ALType type = list->type;
    void **data = list->data;
    if (type == AL_TYPE_STR)
    {
        for (size_t ii = 0; ii < list->size; ii++)
            printf("%s\n", (char *)data[ii]);
    }
    else if (type == AL_TYPE_INT8)
    {
        for (size_t ii = 0; ii < list->size; ii++)
            printf("%d\n", *(int8_t *)data[ii]);
    }
    else if (type == AL_TYPE_INT16)
    {
        for (size_t ii = 0; ii < list->size; ii++)
            printf("%d\n", *(int16_t *)data[ii]);
    }
    else if (type == AL_TYPE_INT32)
    {
        for (size_t ii = 0; ii < list->size; ii++)
            printf("%d\n", *(int32_t *)data[ii]);
    }
    else if (type == AL_TYPE_INT64)
    {
        for (size_t ii = 0; ii < list->size; ii++)
            printf("%ld\n", *(int64_t *)data[ii]);
    }
}