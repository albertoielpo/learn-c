#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha1.h"

/**
 * Delete a file from the fs
 * Requires unistd.h
 * @param[in] filename
 */
bool safe_remove_file(const char *filename)
{
    if (!filename)
    {
        fprintf(stderr, "Invalid filename\n");
        return false;
    }

    // Check if file exists
    if (access(filename, F_OK) != 0)
    {
        fprintf(stderr, "File does not exist: %s\n", filename);
        return false;
    }

    // Check if we have write permission
    if (access(filename, W_OK) != 0)
    {
        fprintf(stderr, "No permission to delete: %s\n", filename);
        return false;
    }

    // Delete the file
    if (remove(filename) != 0)
    {
        perror("Error deleting file");
        return false;
    }

    printf("File deleted: %s\n", filename);
    return true;
}

/**
 * This function finds duplicates inside an array of Fhash structure
 * @param[in] fhs array of results
 * @param[in] len array length
 */
void find_duplicates(Fhash *fhs, size_t len)
{
    uint8_t init[20] = {0}; // All zeros
    for (size_t ii = 0; ii < len - 1; ii++)
    {
        for (size_t jj = ii + 1; jj < len; jj++)
        {
            if (memcmp(fhs[ii].hash, fhs[jj].hash, SHA1_LENGTH) == 0 &&
                memcmp(fhs[jj].hash, init, SHA1_LENGTH) != 0)
            {
                safe_remove_file(fhs[jj].filename);
            }
        }
    }
}

/**
 * This program calculate sha1 hash of some input filenames and removes duplicates
 * @param[in]  argc  args count
 * @param[in]  argv  Filenames
 */
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Missing parameters\n");
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
            continue;

        fhprint(fh);
    }

    if (tot_files > 1)
        find_duplicates(fhs, tot_files);

    free(fhs); // free the space

    return EXIT_SUCCESS;
}
