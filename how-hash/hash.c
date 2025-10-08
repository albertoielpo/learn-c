#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha1.h"

/**
 * This program calculate sha1 hash of some input filenames
 * @param[in]  argc  args count
 * @param[in]  argv  Filenames
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Missing parameters\n");
        return 1;
    }

    // allocate one struct per file
    int tot_files = argc - 1;
    Fhash *fhs = malloc(sizeof(Fhash) * tot_files);
    printf("Total files %d\n", tot_files);

    for (int ii = 0; ii < tot_files; ii++)
    {
        Fhash *fh = &fhs[ii];
        fh->filename = argv[ii + 1]; // skip first argv

        if (!fhsha1(fh))
            continue;

        for (int jj = 0; jj < SHA1_LENGTH; jj++)
        {
            // for each parameter calculate the sha1
            printf("%02x", fh->hash[jj]); // print in hex format
        }
        printf("  %s\n", fh->filename); // 1e4e888ac66f8dd41e00c5a7ac36a32a9950d271  test.txt
    }

    free(fhs); // free the space

    return 0;
}

/**
 * This program calculate sha1 hash of some input strings
 * @param[in]  argc  args count
 * @param[in]  argv  strings
 */
int main_1(int argc, char *argv[])
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
        {
            // for each parameter calculate the sha1
            printf("%02x", hash[i]);
        }
        printf("\n");
    }

    return 0;
}