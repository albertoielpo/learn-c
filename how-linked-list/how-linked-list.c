#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../utils/nllist.h"
#include "../utils/llist.h"

void test_1(void)
{
    NLList *list = nll_create();
    nll_append(list, 150);
    nll_prepend(list, 151);
    nll_add(list, 152, 1);
    nll_append(list, 999);
    nll_remove(list, list->size - 1);

    size_t tail_ele;
    size_t head_ele;
    if (nll_get_value_head(list, &head_ele))
        printf("head ele %ld\n", head_ele);
    if (nll_get_value_tail(list, &tail_ele))
        printf("tail ele %ld\n", tail_ele);

    {
        // assertion block
        assert(head_ele == 151);
        assert(tail_ele == 150);
    }

    nll_print(list);
    nll_print_reverse(list);
    nll_destroy(list);
}

void test_2()
{
    printf("LLNode size: %zu bytes\n", sizeof(LLNode));
    printf("prev offset: %zu\n", offsetof(LLNode, prev));
    printf("elem offset: %zu\n", offsetof(LLNode, elem));
    printf("next offset: %zu\n", offsetof(LLNode, next));
    printf("type offset: %zu\n", offsetof(LLNode, type));

    printf("LList size: %zu bytes\n", sizeof(LList));
    printf("head offset: %zu\n", offsetof(LList, head));
    printf("tail offset: %zu\n", offsetof(LList, tail));
    printf("size offset: %zu\n", offsetof(LList, size));

    // void* list test
    LList *list = ll_create();
    char data[] = {"hello"};
    char data2[] = {"world"};
    int32_t data3 = 150;
    int32_t data4 = 1;
    int32_t dataToRemove = 999;

    ll_add(list, data, LL_TYPE_STR, 0);
    ll_add(list, data2, LL_TYPE_STR, 1);
    ll_add(list, &data3, LL_TYPE_INT32, 0);
    ll_add(list, &data3, LL_TYPE_INT32, (list->size) - 1);
    ll_append(list, &data4, LL_TYPE_INT32);
    ll_prepend(list, &data4, LL_TYPE_INT32);
    ll_append(list, &dataToRemove, LL_TYPE_INT32);
    ll_prepend(list, data, LL_TYPE_STR);
    ll_remove(list, list->size - 1);

    void *value_head = ll_get_value_head(list);
    if (value_head != NULL)
    {
        char *vh = (char *)value_head;
        printf("head ele %s\n", vh);
    }

    void *value_tail = ll_get_value_tail(list);
    if (value_tail != NULL)
    {
        int *vt = (int *)value_tail;
        printf("tail ele %d\n", *vt);
    }

    {
        // assertion block
        char *vh = (char *)value_head;
        int *vt = (int *)value_tail;

        assert(vh[0] == 'h');
        assert(*vt == 1);
    }

    ll_print(list);
    ll_print_reverse(list);
    ll_destroy(list);
}

void test_3(void)
{
    LList *list = ll_create();
    char h, e, l, o;
    h = 'h';
    e = 'e';
    l = 'l';
    o = 'o';
    ll_append(list, &h, LL_TYPE_INT8);
    ll_append(list, &e, LL_TYPE_INT8);
    ll_append(list, &l, LL_TYPE_INT8);
    ll_append(list, &l, LL_TYPE_INT8);
    ll_append(list, &o, LL_TYPE_INT8);
    ll_print(list);

    for (size_t ii = 0; ii < list->size; ii++)
    {
        char *val = (char *)ll_get_value(list, ii);
        putchar(*val);
    }
    printf("\n");

    ll_destroy(list);
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 ../utils/llist.c ../utils/nllist.c how-linked-list.c
int main(void)
{
    printf("--------- numeric list test ---------\n");
    test_1();
    printf("---------- void* list test ----------\n");
    test_2();
    printf("------- void* list int8 test --------\n");
    test_3();

    return 0;
}