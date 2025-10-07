#include <stdio.h>
#include <string.h>
#include "sha1.h"

/**
 * This program calculate the sha1 hash from argv
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Missing parameters\n");
        return 1;
    }

    for (int ii = 1; ii < argc; ii++)
    {
        uint8_t hash[SHA1_LENGTH];
        sha1((uint8_t *)argv[ii], strlen(argv[ii]), hash);

        for (int i = 0; i < SHA1_LENGTH; i++)
        {
            // for each parameter calculate the sha1
            printf("%02x", hash[i]);
        }
        printf("\n");
    }

    return 0;
}