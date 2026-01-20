#include "../utils/nalist.h"
#include <assert.h>
#include <stdio.h>

// gcc -Wall -Wpedantic -O2 -g -std=c99 ../utils/nalist.c how-narray-list.c
int main(void) {
    NAList *list = nal_create(16);

    {
        // test
        nal_append(list, 150);
        nal_append(list, 200);
        nal_append(list, 300);
        nal_prepend(list, 1234567890);
        nal_remove(list, 1);
        nal_print(list);

        size_t res = 0;
        if (!nal_get(list, 0, &res)) {
            fprintf(stderr, "Cannot get element with index 0\n");
            return 1;
        }

        printf("res is %zu\n", res);
        assert(res == 1234567890);
    }

    nal_destroy(list);

    return 0;
}