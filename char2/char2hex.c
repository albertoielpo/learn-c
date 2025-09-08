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
 */
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Missing parameters\n");
        return 1;
    }

    for (int ii = 1; ii < argc; ii++)
    {
        for (int jj = 0; jj < strlen(argv[ii]); jj++)
        {
            unsigned char cur = argv[ii][jj]; // convert into an unsigned char
            printf("%02x ", cur);
        }
    }

    printf("\n");

    return 0;
}