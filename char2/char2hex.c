#include <stdio.h>
#include <string.h>

/**
 * Convert a character list into a hex numbers.
 *
 * Usage
 * single param
 * ./char2hex ciao -> 63 69 61 6f
 *
 * double quote param
 * ./char2hex "hello world" -> 68 65 6c 6c 6f 20 77 6f 72 6c 64
 *
 * multiple parameters support
 * ./char2hex hello world -> 68 65 6c 6c 6f 77 6f 72 6c 64
 *
 * build: gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 char2hex.c
 */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s <str_1> <str_2>...<str_n>\n", argv[0]);
        return 1;
    }

    for (int ii = 1; ii < argc; ii++)
    {
        for (size_t jj = 0; jj < strlen(argv[ii]); jj++)
        {
            unsigned char cur = argv[ii][jj]; // convert into an unsigned char
            printf("%02x ", cur);
        }
    }

    printf("\n");

    return 0;
}