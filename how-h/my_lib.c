#include <stdio.h>
#include "my_lib.h"

void swapValue(int *a, int *b)
{
    int c = *a;
    *a = *b;
    *b = c;
}