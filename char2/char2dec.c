#include <stdio.h>
#include <string.h>

/**
 * Convert a character list into a dec numbers.
 *
 * Usage
 * single param
 * ./char2dec ciao -> 99 105 97 111
 *
 * double quote param
 * ./char2dec "hello world!" -> 104 101 108 108 111 32 119 111 114 108 100
 *
 * multiple parameters support
 * ./char2dec hello world -> 104 101 108 108 111 119 111 114 108 100
 *
 * build: gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 char2hex.c
 */
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <str_1> <str_2>...<str_n>\n", argv[0]);
        return 1;
    }

    for (int ii = 1; ii < argc; ii++) {
        for (size_t jj = 0; jj < strlen(argv[ii]); jj++) {
            unsigned char cur = argv[ii][jj]; // convert into an unsigned char
            printf("%d ", cur);
        }
    }

    printf("\n");

    return 0;
}