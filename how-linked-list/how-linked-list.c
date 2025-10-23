#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "../utils/nllist.h"
#include "../utils/llist.h"

int main(void)
{
    printf("--------- numeric list test ---------\n");
    NLList *nl = nll_create();
    nll_append(nl, 150);
    nll_prepend(nl, 151);
    nll_add(nl, 152, 1);
    nll_append(nl, 999);
    nll_remove(nl, nl->size - 1);

    size_t tail_ele;
    size_t head_ele;
    if (nll_get_value_head(nl, &head_ele))
        printf("head ele %ld\n", head_ele);
    if (nll_get_value_tail(nl, &tail_ele))
        printf("tail ele %ld\n", tail_ele);

    {
        // assertion block
        assert(head_ele == 151);
        assert(tail_ele == 150);
    }

    nll_print(nl);
    nll_print_reverse(nl);
    nll_destroy(nl);

    printf("---------- void* list test ----------\n");

    // void* list test
    LList *l = ll_create();
    char data[] = {"hello"};
    char data2[] = {"world"};
    int32_t data3 = 150;
    int32_t data4 = 1;
    int32_t dataToRemove = 999;

    ll_add(l, data, LL_TYPE_CHAR, 0);
    ll_add(l, data2, LL_TYPE_CHAR, 1);
    ll_add(l, &data3, LL_TYPE_INT32, 0);
    ll_add(l, &data3, LL_TYPE_INT32, (nl->size) - 1);
    ll_append(l, &data4, LL_TYPE_INT32);
    ll_prepend(l, &data4, LL_TYPE_INT32);
    ll_append(l, &dataToRemove, LL_TYPE_INT32);
    ll_prepend(l, data, LL_TYPE_CHAR);
    ll_remove(l, l->size - 1);

    void *value_head = ll_get_value_head(l);
    if (value_head != NULL)
    {
        char *vh = (char *)value_head;
        printf("head ele %s\n", vh);
    }

    void *value_tail = ll_get_value_tail(l);
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

    ll_print(l);
    ll_print_reverse(l);
    ll_destroy(l);

    return 0;
}