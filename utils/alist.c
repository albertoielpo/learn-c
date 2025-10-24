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
 * @brief Resize data
 *
 * Resize data with capacity * 2. If new capacity is equals to old one then is not grown
 *
 * @param[in] list List pointer
 * @return new capacity or 0 in case of error
 */
static size_t al_resize(AList *list)
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
 * @brief Auto resize if needed
 *
 * @param list List pointer
 * @return 0 in case of error, 1 if all good
 */
static int al_auto_resize(AList *list)
{
    if (list->size >= list->capacity)
    {
        // resize
        if (!al_resize(list))
        {
            printf("Cannot append a new element\n");
            return 0;
        }
    }
    return 1;
}

/** @copydoc al_insert */
size_t al_insert(AList *list, void *ele, size_t idx)
{

    if (idx > list->size)
    {
        printf("Index out of bound\n");
        return -1;
    }

    // insert in the middle
    if (!al_auto_resize(list))
        return -1;

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

/** @copydoc al_print */
void al_print(AList *list)
{
    ALType type = list->type;
    void **data = list->data;
    if (type == AL_TYPE_CHAR)
    {
        for (size_t ii = 0; ii < list->size; ii++)
            printf("%s\n", (char *)data[ii]);
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