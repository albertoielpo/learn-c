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

        fhprint(fh);
    }

    free(fhs); // free the space

    return 0;
}