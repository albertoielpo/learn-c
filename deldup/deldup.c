/**
 * This program calculate sha1 hash of some input filenames and removes duplicates
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha1.h"
#include "../third-party/hash-table/ht.h"

/**
 * Delete a file from the fs
 * Requires unistd.h
 * @param[in] filename
 */
static bool delete_file(const char *filename)
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
 * This function finds and remove duplicates inside an array of Fhash structure.
 * fhs array is not sorted by hash
 *
 * @param[in] fhs array of results
 * @param[in] len array length
 */
static void remove_duplicates(Fhash *fhs, size_t len)
{
    // create an hash table to reduce scan complexity
    ht *table = ht_create();
    if (table == NULL)
    {
        perror("Cannot allocate hash table");
        exit(1);
    }

    // init to all zeros
    uint8_t init[20] = {0};

    for (size_t ii = 0; ii < len - 1; ii++)
    {
        // convertion from uint8_t to hex string
        char hash_str[SHA1_LENGTH_CHAR]; // size needed 40 + \0

        if (memcmp(fhs[ii].hash, init, SHA1_LENGTH) == 0)
        {
            // hash not calculated, all zeros
            continue;
        }

        // Convert binary hash to hex string
        if (!hash_to_hex(fhs[ii].hash, SHA1_LENGTH, hash_str, SHA1_LENGTH_CHAR))
        {
            // cannot continue
            exit(1);
        }

        void *value = ht_get(table, hash_str);
        if (value == NULL)
        {
            // not found
            ht_set(table, hash_str, fhs[ii].filename);
        }
        else
        {
            // if found then it's a duplicate
            printf("%s is a duplicate of %s\n", fhs[ii].filename, ((char *)value));
            delete_file(fhs[ii].filename);
        }
    }
    ht_destroy(table);
}

/**
 * Main function
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
        remove_duplicates(fhs, tot_files);

    free(fhs); // free the space

    return EXIT_SUCCESS;
}
