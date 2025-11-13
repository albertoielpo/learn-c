#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../utils/alist.h"

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
    AList *list = al_create(10, AL_TYPE_STR);
    al_append(list, strdup("hello")); // here list own the element
    al_append(list, strdup("hello")); // this is another hello
    // al_remove(list, 0)  // 💩 leak
    al_remove_deep(list, 0); // remove deep first hello because is owned by AList
    al_print(list);

    // al_destroy(list);  // 💩 leak
    al_destroy_deep(list);
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 ../utils/alist.c how-array-list.c
int main(void)
{
    printf("------- Test 1 -------\n");
    test_1();
    printf("------- Test 2 -------\n");
    test_2();
    printf("------- Test 3 -------\n");
    test_3();
    printf("------- Test 4 -------\n");
    test_4();
    return 0;
}