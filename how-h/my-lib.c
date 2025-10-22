#include <stdio.h>
#include "my-lib.h"

void swapValue(int *a, int *b)
{
    int c = *a;
    *a = *b;
    *b = c;
}