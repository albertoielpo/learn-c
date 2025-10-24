/**
 * @author Alberto Ielpo
 * Array list implementation
 * All elements must be of the same type
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum
{
    LL_TYPE_CHAR,  // char pointer (string)
    LL_TYPE_INT16, // 16-bit signed integer pointer
    LL_TYPE_INT32, // 32-bit signed integer pointer
    LL_TYPE_INT64  // 64-bit signed integer pointer
} ALType;

typedef struct
{
    size_t capacity; // max data length
    size_t size;     // current data length
    void *data;      // data array
    ALType type;     // Type of the element (4 bytes)
                     // 4 bytes padding
} AList;

AList *al_create(size_t capacity, ALType type)
{
    if (capacity <= 0)
    {
        perror("Invalid capacity");
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

    size_t data_alloc;
    if (type == LL_TYPE_CHAR)
        data_alloc = sizeof(char);
    else if (type == LL_TYPE_INT16)
        data_alloc = sizeof(int16_t);
    else if (type == LL_TYPE_INT32)
        data_alloc = sizeof(int32_t);
    else if (type == LL_TYPE_INT64)
        data_alloc = sizeof(int64_t);
    else
    {
        perror("Invalid data alloc type");
        return NULL;
    }

    list->data = malloc(data_alloc * capacity);
    return list;
}

int main(void)
{
    AList list;
    list.data = NULL;
    list.capacity = 0;
    list.size = 0;
    printf("Alist size %ld\n", sizeof(AList));
    return 0;
}