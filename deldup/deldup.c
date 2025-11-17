/**
 * This program calculate sha1 hash of some input filenames and removes duplicates
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "../utils/sha1.h"
#include "../utils/hmap.h"

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
    // create an hash map to reduce scan complexity
    HMap *map = hmap_create(1024);
    if (map == NULL)
    {
        perror("Cannot allocate hash map");
        exit(1);
    }

    // init to all zeros
    uint8_t init[20] = {0};

    // convertion from uint8_t to hex string
    // malloc is used because data are not owned by hash map
    // so we need to allocate 40 + \0 byte for each hash
    void *all_hash_str = malloc(sizeof(char) * SHA1_LENGTH_CHAR * len); // [hash_str (41) | hash_str | ....]
    // point to first
    char *hash_str = all_hash_str;

    for (size_t ii = 0; ii < len; ii++)
    {
        if (memcmp(fhs[ii].hash, init, SHA1_LENGTH) == 0)
            continue; // hash not calculated, all zeros

        // Convert binary hash to hex string
        if (!hash_to_hex(fhs[ii].hash, SHA1_LENGTH, hash_str, SHA1_LENGTH_CHAR))
            exit(1); // cannot continue

        HEntry *entry = hmap_get(map, hash_str); // ht_get(map, hash_str);
        if (entry == NULL)
        {
            // not found then HMap *map, char *key, void *value, HEType type, uint32_t value_size
            hmap_add(map, hash_str, fhs[ii].filename, HE_TYPE_STR, 1);
        }
        else
        {
            // if found then it's a duplicate. cast to char* is safe
            printf("%s is a duplicate of %s\n", fhs[ii].filename, ((char *)entry->value));
            delete_file(fhs[ii].filename);
        }
        hash_str += SHA1_LENGTH_CHAR; // go to the next one
    }

    free(all_hash_str); // destroy hashes
    hmap_destroy(map);  // destroy map
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
            continue;

        fhprint(fh);
    }

    if (tot_files > 1)
        remove_duplicates(fhs, tot_files);

    free(fhs); // free the space

    return EXIT_SUCCESS;
}
