#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha1.h"

/**
 * This program calculate sha1 hash of some input strings
 * @param[in]  argc  args count
 * @param[in]  argv  strings
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
        if (!sha1((uint8_t *)argv[ii], strlen(argv[ii]), hash))
            return 1;

        for (int i = 0; i < SHA1_LENGTH; i++)
            printf("%02x", hash[i]);
        printf("  %s\n", argv[ii]); // 1e4e888ac66f8dd41e00c5a7ac36a32a9950d271  ciao
    }

    return 0;
}