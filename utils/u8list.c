#include <stdio.h>
#include "u8list.h"

/** @copydoc u8list_create */
U8List *u8list_create(size_t capacity)
{
    if (capacity <= 0)
    {
        printf("Invalid capacity\n");
        return NULL;
    }

    U8List *list = malloc(sizeof(U8List));
    if (list == NULL)
    {
        perror("Cannot create a new array list");
        return NULL;
    }
    list->capacity = capacity;
    list->size = 0;
    list->data = malloc(sizeof(uint8_t) * capacity);
    return list;
}

/** @copydoc u8list_destroy */
void u8list_destroy(U8List *list)
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
static size_t u8list_grow(U8List *list)
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
static size_t u8list_shrink(U8List *list)
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

/** @copydoc u8list_insert */
size_t u8list_insert(U8List *list, uint8_t ele, size_t idx)
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
        if (!u8list_grow(list))
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

/** @copydoc u8list_append */
size_t u8list_append(U8List *list, uint8_t ele)
{
    return u8list_insert(list, ele, list->size);
}

/** @copydoc u8list_prepend */
size_t u8list_prepend(U8List *list, uint8_t ele)
{
    return u8list_insert(list, ele, 0);
}

/** @copydoc u8list_get */
int u8list_get(U8List *list, size_t idx, uint8_t *res)
{
    if (idx >= list->size)
    {
        printf("Index out of bound\n");
        return 0;
    }
    *res = list->data[idx];
    return 1;
}

/** @copydoc u8list_remove */
int u8list_remove(U8List *list, size_t idx)
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
        if (!u8list_shrink(list))
        {
            printf("Cannot remove the element with index %ld\n", idx);
            return 0;
        }
    }

    // left shift
    for (size_t ii = idx; ii < list->size - 1; ii++)
    {
        list->data[ii] = list->data[ii + 1];
    }
    list->size--;

    return 1;
}

/** @copydoc u8list_print */
void u8list_print(U8List *list)
{
    uint8_t *data = list->data;
    for (size_t ii = 0; ii < list->size; ii++)
        printf("%d\n", data[ii]);
}