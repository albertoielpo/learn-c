#include <stdio.h>
#include "my_lib.h"

/**
 * build chain
 * gcc -c my_lib.c && gcc -c program.c && gcc program.o my_lib.o -o program
 */
int main(void)
{
    int a = 10;
    int b = 20;
    printf("before swap > a: %d, b: %d\n", a, b);
    // swap value is defined inside my_lib (.h signature, .c implementation)
    swapValue(&a, &b);
    printf("after swap > a: %d, b: %d\n", a, b);
    return 0;
}
