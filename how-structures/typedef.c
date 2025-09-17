#include <stdio.h>

// new type definition
typedef int errorcode;

// fract alias for struct
typedef struct
{
    int n;
    int d;
} fract;

// pointer to a struct fract
typedef fract *fractptr;

errorcode foo(void)
{
    errorcode a = -20;
    return a;
}

int main(void)
{
    printf("%d\n", foo());
    fract f;
    f.n = 10;
    f.d = 20;
    printf("%d/%d\n", f.n, f.d);
    fractptr fp = &f;
    printf("%p\n", fp);
    return 0;
}
