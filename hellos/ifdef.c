#include <stdio.h>

int main(void) {
#ifdef DEBUG
    // the code inside DEBUG block is available only if is compiled with -D property
    // ex:gcc -DDEBUG main.c -o main
    printf("hello DEBUG\n");
#elif DEBUG == 2
    // it's also possible to assign a specific value
    // gcc -DDEBUG=2 ifdef.c && ./a.out
    printf("hello DEBUG 2\n");
#else
    // in this case the DEBUG variable is not set
    printf("no DEBUG variable set\n");
#endif
    return 0;
}