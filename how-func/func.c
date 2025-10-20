/**
 * function pointer introduction
 */
#include <stdio.h>
#include <stdlib.h>

void hello(void)
{
    printf("Hello!\n");
}

void baubau(void)
{
    printf("Bau Bau!\n");
}

/**
 * @param[in] n how many time
 * @param[in] x function to call
 */
void call_n_times(int n, void (*x)(void))
{
    while (n--)
    {
        x();
    }
}

/**
 * @param[in] a pointer to the first element
 * @param[in] b pointer to the second element
 */
int sort_asc(const void *a, const void *b)
{
    const int *ap = a;
    const int *bp = b;
    if (*ap > *bp)
        return 1;
    if (*ap < *bp)
        return -1;
    return 0;
}

int main(void)
{
    // declaration of function pointer
    void (*x)(void);
    // assignment
    x = hello;
    // call
    x();
    // then assign to another that have the same prototype
    x = baubau;
    // call again
    x();

    printf("****\n");

    call_n_times(3, hello);
    call_n_times(3, baubau);

    printf("****\n");

    // example using quick sort with a function*
    int ELEMS = 10;
    int a[ELEMS];
    for (int ii = 0; ii < ELEMS; ii++)
    {
        // rand() is deterministic
        // for pseudo random numbers use a seed
        a[ii] = rand() & 0xFF; // take only last byte
        printf("%d ", a[ii]);
    }
    printf("\n");

    /**
     * Signature
     * void qsort(void *__base, size_t __nmemb, size_t __size, __compar_fn_t __compar)
     * where __compar_fn_t is int (*__compar_fn_t) (const void *, const void *)
     * so a function pointer which return int and accepts two void pointers
     */
    qsort(a, ELEMS, sizeof(int), sort_asc);

    for (int ii = 0; ii < ELEMS; ii++)
        printf("%d ", a[ii]);

    printf("\n");

    return 0;
}