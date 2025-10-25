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
    printf("elem_size offset: %zu\n", offsetof(LLNode, elem_size));

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

    ll_add(list, data, 1, LL_TYPE_STR, 0);
    ll_add(list, data2, 1, LL_TYPE_STR, 1);
    ll_add(list, &data3, 1, LL_TYPE_INT32, 0);
    ll_add(list, &data3, 1, LL_TYPE_INT32, (list->size) - 1);
    ll_append(list, &data4, 1, LL_TYPE_INT32);
    ll_prepend(list, &data4, 1, LL_TYPE_INT32);
    ll_append(list, &dataToRemove, 1, LL_TYPE_INT32);
    ll_prepend(list, data, 1, LL_TYPE_STR);
    ll_remove(list, list->size - 1);

    LLNode *head = ll_get_head(list);
    if (head != NULL && head->elem != NULL)
    {
        char *vh = (char *)(head->elem);
        printf("head ele %s\n", vh);
    }

    LLNode *tail = ll_get_tail(list);
    if (tail != NULL && tail->elem != NULL)
    {
        int *vt = (int *)(tail->elem);
        printf("tail ele %d\n", *vt);
    }

    {
        // assertion block
        char *vh = (char *)(head->elem);
        int *vt = (int *)(tail->elem);

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
    {
        char h, e, l, o;
        h = 'h';
        e = 'e';
        l = 'l';
        o = 'o';
        ll_append(list, &h, 1, LL_TYPE_INT8);
        ll_append(list, &e, 1, LL_TYPE_INT8);
        ll_append(list, &l, 1, LL_TYPE_INT8);
        ll_append(list, &l, 1, LL_TYPE_INT8);
        ll_append(list, &o, 1, LL_TYPE_INT8);
        ll_print(list);

        for (size_t ii = 0; ii < list->size; ii++)
        {
            LLNode *node = ll_get(list, ii);
            if (node != NULL && node->elem != NULL)
            {
                char *val = (char *)node->elem;
                if (node->elem_size == 1) // security check
                    putchar(*val);
            }
        }
        printf("\n");
    }

    ll_destroy(list);
}

void test_4(void)
{
    LList *list = ll_create();
    {
        int8_t int8_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        char str[] = {"hello world"};
        char char_array[] = {"hello world"};
        ll_append(list, int8_array, 10, LL_TYPE_INT8);
        ll_append(list, str, 1, LL_TYPE_STR);          // do not need to specify the length
        ll_append(list, char_array, 11, LL_TYPE_INT8); // in this case is threated as int8_t[]. Exclude the \0
        ll_print(list);
        printf("----------\n");
        ll_print_reverse(list);
    }
    ll_destroy(list);
}

void test_5(void)
{
    LList *list = ll_create();
    {
        int8_t elem1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        int8_t elem2[] = {1, 2, 3, 4, 5};
        ll_append(list, elem1, 10, LL_TYPE_INT8);
        ll_append(list, elem2, 5, LL_TYPE_INT8);

        printf("print list\n");
        ll_print(list);

        LLNode node;
        if (!ll_pop(list, &node))
        {
            printf("Cannot pop the element!\n");
            return;
        }

        printf("popped elem size: %d\n", node.elem_size);
        char *cur = (char *)node.elem;
        for (size_t ii = 0; ii < node.elem_size; ii++)
        {
            printf("%d ", *cur);
            cur++;
        }
        printf("\n");

        printf("print list\n");
        ll_print(list);

        // change the int8_t[] with another element of a different type
        int64_t new_elem = 48763251;
        ll_change(list, &new_elem, 1, LL_TYPE_INT64, 0);
        ll_print(list);
    }
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
    printf("------ void* list int8[] test -------\n");
    test_4();
    printf("----- void* list new func test ------\n");
    test_5();
    return 0;
}