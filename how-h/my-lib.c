#include "my-lib.h"
#include <stdio.h>

void swapValue(int *a, int *b) {
    int c = *a;
    *a = *b;
    *b = c;
}