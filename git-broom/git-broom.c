#define _POSIX_C_SOURCE 200809L
// .h
/**
 * @brief Clean up developer dependencies
 *
 * Git broom will clean up all files that are generated in the development process, such as node_modules, dist, target and so on
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "../utils/alist.h"

// .c
// Global variable - accessible throughout this file
const char *broom_names[] = {"node_modules", "dist", "build", "target"};
const size_t broom_names_count = sizeof(broom_names) / sizeof(broom_names[0]);

static void find_by_name(const char *path, const char *target_name, AList *list)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char full_path[1024]; // stack buffer, it needs strdup

    // Open directory
    dir = opendir(path);

    // Silently skip directories we can't read (like find does)
    if (dir == NULL)
        return;

    // Read directory entries
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Build full path
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Check if name matches
        if (strcmp(entry->d_name, target_name) == 0)
        {
            // Allocate heap memory for the string
            char *heap_path = strdup(full_path);
            if (heap_path)
                al_append(list, heap_path);
            else
                fprintf(stderr, "[find_by_name] cannot strdup! skip path %s", full_path);
        }

        // Get file stats
        if (stat(full_path, &statbuf) == -1)
            continue;

        // If it's a directory, recurse into it
        if (S_ISDIR(statbuf.st_mode))
            find_by_name(full_path, target_name, list);
    }

    closedir(dir);
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 git-broom.c
int main(int argc, char *argv[])
{
    // 1. list .git files from the current directory
    const char *start_path = ".";
    const char *target_name = ".git";
    if (argc >= 2)
    {
        // TODO check better
        start_path = argv[1];
    }

    {
        AList *dir_list = al_create(256, AL_TYPE_STR);
        if (!dir_list)
            return 1;

        find_by_name(start_path, target_name, dir_list);

        // 2. move to the same directory where .git is located
        for (size_t ii = 0; ii < dir_list->size; ii++)
        {
            void *cur_dir = dir_list->data[ii];
            printf("Go to %s\n", (char *)cur_dir);

            // 3. clean up target directories such as node_modules, dist, target, ... and so on
        }

        al_destroy_deep(dir_list);
    }

    return 0;
}