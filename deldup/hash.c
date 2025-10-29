#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../utils/sha1.h"

/**
 * This program calculate sha1 hash of some input filenames
 * @param[in]  argc  args count
 * @param[in]  argv  Filenames
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Hash calculate the sha1 of some input filenames\n");
        printf("Usage: %s <file_1>...<file_n>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // allocate one struct per file
    int tot_files = argc - 1;
    Fhash *fhs = calloc(argc - 1, sizeof(Fhash));
    // printf("Total files %d\n", tot_files);

    for (int ii = 0; ii < tot_files; ii++)
    {
        Fhash *fh = &fhs[ii];
        fh->filename = argv[ii + 1]; // skip first argv

        if (!fhsha1(fh))
        {
            fprintf(stderr, "Error: hash not calculated for %s\n", fh->filename);
            continue;
        }

        fhprint(fh);
    }

    free(fhs); // free the space

    return EXIT_SUCCESS;
}