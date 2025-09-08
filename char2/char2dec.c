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
            printf("%d ", cur);
        }
    }

    printf("\n");

    return 0;
}