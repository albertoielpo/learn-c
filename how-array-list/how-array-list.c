#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../utils/alist.h"
#include "../utils/u8list.h"

void test_1(void)
{
    AList *list = al_create(3, AL_TYPE_STR);
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
        assert(list->type == AL_TYPE_STR);
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

        for (int ii = 0; ii < 10; ii++)
            al_append(list, &a);
        printf("List capacity: %ld size: %ld type %d\n", list->capacity, list->size, list->type);
        for (int ii = 0; ii < 10; ii++)
            al_remove(list, 2); // remove 10 elements consecutevely

        al_print(list);
        printf("List capacity: %ld size: %ld type %d\n", list->capacity, list->size, list->type);
    }
    al_destroy(list);
}

void test_3(void)
{
    AList *list = al_create(10, AL_TYPE_INT8);

    {
        char h = 'h';
        char e = 'e';
        char l = 'l';
        char o = 'o';
        al_append(list, &h);
        al_append(list, &e);
        al_append(list, &l);
        al_append(list, &l);
        al_append(list, &o);

        al_print(list);
        for (size_t ii = 0; ii < list->size; ii++)
        {
            char *val = (char *)al_get(list, ii); // get char*
            putchar(*val);                        // dereference
        }
        printf("\n");
    }

    al_destroy(list);
}

void test_4(void)
{
    U8List *list = u8list_create(10);
    {
        for (size_t ii = 0; ii < 6000; ii++)
            u8list_append(list, ii % 256); // cast to uint8
        uint8_t last;
        uint8_t bast;
        u8list_get(list, list->size - 1, &last);
        u8list_get(list, list->size - 2, &bast);

        printf("last two elements are %d %d\n", last, bast);
        assert(last == 111);
        assert(bast == 110);

        printf("List capacity: %ld size: %ld\n", list->capacity, list->size);

        for (size_t ii = 0; ii < 5998; ii++)
            u8list_remove(list, 0); // remove at top

        // only last two elements are expected
        u8list_print(list);
        printf("List capacity: %ld size: %ld\n", list->capacity, list->size);
    }
    u8list_destroy(list);
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 ../utils/alist.c ../utils/u8list.c how-array-list.c
int main(void)
{
    printf("------- Test 1 -------\n");
    test_1();
    printf("------- Test 2 -------\n");
    test_2();
    printf("------- Test 3 -------\n");
    test_3();
    printf("--- Test 4 Uint8[] ---\n");
    test_4();
    return 0;
}