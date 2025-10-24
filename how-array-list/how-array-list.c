#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../utils/alist.h"

void test_1(void)
{
    AList *list = al_create(3, AL_TYPE_CHAR);
    {
        char str1[] = {"Append world!"};
        char str2[] = {"1 index world but will be at index 2"};
        char str3[] = {"Prepend world!"};

        al_append(list, str1);
        al_append(list, str1);
        al_append(list, str1);
        al_append(list, str1);
        al_insert(list, str2, 1);
        al_prepend(list, str3);
        al_append(list, str1);

        printf("--- print element index 1 ---\n");
        void *data;
        if ((data = al_get(list, 1)))
        {
            char *data_str = (char *)data;
            printf("%s\n", data_str);
            assert(data_str[0] == 'A');
        }

        printf("--- print all ---\n");
        al_print(list);
        printf("List capacity: %ld size: %ld type %d\n", list->capacity, list->size, list->type);

        assert(list->capacity == 12);
        assert(list->size == 7);
        assert(list->type == AL_TYPE_CHAR);
    }

    al_destroy(list);
}

void test_2(void)
{
    AList *list = al_create(10, AL_TYPE_INT32);
    {
        int32_t a, b, c;
        a = 1;
        b = 2;
        c = 3;
        al_append(list, &a);    // [1]
        al_prepend(list, &b);   // [2,1]
        al_insert(list, &c, 1); // [2,3,1]
        al_print(list);
        printf("List capacity: %ld size: %ld type %d\n", list->capacity, list->size, list->type);

        assert(list->capacity == 10);
        assert(list->size == 3);
        assert(list->type == AL_TYPE_INT32);
    }
    al_destroy(list);
}

int main(void)
{
    printf("--- Test 1 ---\n");
    test_1();
    printf("--- Test 2 ---\n");
    test_2();
    return 0;
}