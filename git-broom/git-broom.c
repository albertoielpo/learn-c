#define _POSIX_C_SOURCE 200809L
/**
 * @file git-broom.c
 * @brief Clean up developer dependencies in git repositories
 *
 * Git broom recursively searches for .git directories and cleans up
 * build artifacts and dependencies (node_modules, dist, target, etc.)
 * in each project found.
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 * @version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../utils/alist.h"

#define BROOM_VERSION 1
#define BROOM_PATH_SIZE 10024
#define DOT_GIT ".git"
#define GIT_SUFFIX "/" DOT_GIT
#define GIT_SUFFIX_LEN 5

/**
 * @brief List of target directories to clean
 *
 * Common build output and dependency directories across various
 * programming languages and frameworks.
 */
const char *broom_targets[] = {
    "node_modules", // npm/yarn/pnpm dependencies (JavaScript/TypeScript)
    "dist",         // distribution/build output (JavaScript/TypeScript)
    "build",        // build output (various languages)
    ".next",        // Next.js build cache (JavaScript/TypeScript)
    ".nuxt",        // Nuxt.js build cache (JavaScript/TypeScript)
    "out",          // output directory (various frameworks)
    "coverage",     // test coverage reports (various languages)
    ".turbo",       // Turborepo cache (JavaScript/TypeScript)
    "target",       // Maven/Cargo build directory (Java/Rust)
    "generated",    // generated resources (various languages)
};
const size_t broom_targets_count = sizeof(broom_targets) / sizeof(broom_targets[0]);

/**
 * @brief Find directories by name recursively
 *
 * Recursively searches the file system starting from the given path
 * for directories matching the target_name. When found, strips the
 * target_name suffix and adds the parent directory path to the list.
 *
 * This function is designed to find .git directories and return their
 * parent project directories for cleanup operations.
 *
 * @param[in] path Starting directory path to search from
 * @param[in] target_name Directory name to search for (e.g., ".git")
 * @param[out] list ArrayList to store found parent directory paths
 *
 * @note Function silently skips directories it cannot read
 * @note Allocates heap memory for each found path (caller must free via al_destroy_deep)
 */
static void find_by_name(const char *path, const char *target_name, AList *list)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char full_path[BROOM_PATH_SIZE];

    // Open directory
    dir = opendir(path);
    if (dir == NULL)
    {
        // Silently skip directories we can't read (like find does)
        return;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Build full path with length validation
        int written = snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (written >= (int)sizeof(full_path))
        {
            fprintf(stderr, "[find_by_name] Path too long: %s/%s\n", path, entry->d_name);
            continue;
        }

        // Check if name matches target
        if (strcmp(entry->d_name, target_name) == 0)
        {
            // Allocate heap memory for the string
            char *heap_path = strdup(full_path);
            if (!heap_path)
            {
                fprintf(stderr, "[find_by_name] strdup failed! Skipping path %s\n", full_path);
                continue;
            }

            // Remove "/.git" suffix to get project directory
            size_t len = strlen(heap_path);
            if (len >= GIT_SUFFIX_LEN && strcmp(heap_path + len - GIT_SUFFIX_LEN, GIT_SUFFIX) == 0)
            {
                heap_path[len - GIT_SUFFIX_LEN] = '\0';
            }
            else
            {
                fprintf(stderr, "[find_by_name] Unexpected path format: %s\n", heap_path);
                free(heap_path);
                continue;
            }

            // Add to list - returns 1 on success, 0 on error
            if (!al_append(list, heap_path))
            {
                free(heap_path);
                fprintf(stderr, "[find_by_name] al_append failed for path %s\n", full_path);
                closedir(dir);
                return;
            }
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

/**
 * @brief Recursively remove a directory and all its contents
 *
 * Performs a depth-first recursive deletion of a directory tree.
 * Handles both files and subdirectories, removing all contents
 * before removing the directory itself.
 *
 * @param[in] path Path to directory to remove
 * @return 0 on success
 * @return 1 if directory doesn't exist (not an error for cleanup)
 * @return 2 if path exists but is not a directory
 * @return 3 on any other error (permissions, I/O errors, etc.)
 *
 * @note This function stops on the first error encountered
 * @note Uses native POSIX calls (opendir, readdir, unlink, rmdir)
 */
int remove_directory(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char full_path[BROOM_PATH_SIZE];
    int result = 0;

    // Check if path exists
    if (stat(path, &statbuf) != 0)
    {
        return 1; // Doesn't exist - not an error in cleanup context
    }

    // Check if it's a directory
    if (!S_ISDIR(statbuf.st_mode))
    {
        fprintf(stderr, "%s is not a directory\n", path);
        return 2;
    }

    // Open directory
    dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir failed");
        return 3;
    }

    // Remove all entries in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Build full path with length validation
        int written = snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
        if (written >= (int)sizeof(full_path))
        {
            fprintf(stderr, "Path too long: %s/%s\n", path, entry->d_name);
            result = 3;
            break;
        }

        // Get file stats
        if (stat(full_path, &statbuf) == -1)
        {
            perror("stat failed");
            result = 3;
            break;
        }

        // Recursively remove if directory, otherwise unlink file
        if (S_ISDIR(statbuf.st_mode))
        {
            int ret = remove_directory(full_path);
            if (ret > 1) // Ignore "doesn't exist" errors
            {
                result = ret;
                break;
            }
        }
        else
        {
            if (unlink(full_path) != 0)
            {
                perror("unlink failed");
                result = 3;
                break;
            }
        }
    }

    closedir(dir);

    // If there was an error during removal, don't try to remove the directory
    if (result != 0)
    {
        return result;
    }

    // Finally remove the directory itself
    if (rmdir(path) != 0)
    {
        perror("rmdir failed");
        return 3;
    }

    return 0;
}

/**
 * @brief Clean up target directories in a project
 *
 * Iterates through the list of target directories (node_modules, dist, etc.)
 * and attempts to remove each one from the specified project directory.
 * Prints success messages to stdout and errors to stderr.
 *
 * @param[in] path Project directory path
 *
 * @note Error code 1 (directory doesn't exist) is not reported as an error
 * @note All other error codes are reported to stderr
 */
static void clean_up_targets(const char *path)
{
    for (size_t ii = 0; ii < broom_targets_count; ii++)
    {
        char broom_path[BROOM_PATH_SIZE] = {0};
        int written = snprintf(broom_path, sizeof(broom_path), "%s/%s", path, broom_targets[ii]);

        // Validate path length
        if (written >= (int)sizeof(broom_path))
        {
            fprintf(stderr, "Path too long: %s/%s\n", path, broom_targets[ii]);
            continue;
        }

        // Attempt to remove directory
        int errcode = remove_directory(broom_path);

        // Report results
        if (errcode == 0)
            printf("✓ Cleaned %s\n", broom_path);

        // errcode == 1 means directory doesn't exist (not an error in cleanup context)
        else if (errcode > 1)
            fprintf(stderr, "✗ Failed to clean %s (error code: %d)\n", broom_path, errcode);
    }
}

/**
 * @brief Main entry point for git-broom
 *
 * Workflow:
 * 1. Search for all .git directories starting from the given path
 * 2. For each project found, clean up target directories
 * 3. Report results to stdout (cleaned) and stderr (errors)
 *
 * Usage: ./git-broom [path]
 *   path: Starting directory (default: current directory ".")
 *
 * Example:
 *   ./git-broom /home/user/projects
 *   ./git-broom >stdout.txt 2>stderr.txt
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return 0 on success, 1 on initialization failure
 */
int main(int argc, char *argv[])
{
    printf("git-broom v%d\n", BROOM_VERSION);
    printf("Cleaning development artifacts from git repositories\n\n");

    // Parse command line arguments
    const char *start_path = ".";

    if (argc >= 2)
        start_path = argv[1];

    printf("Working directory: %s\n", start_path);
    printf("Searching for %s directories...\n\n", DOT_GIT);

    // Create list to store found project directories
    AList *dir_list = al_create(256, AL_TYPE_STR);
    if (!dir_list)
    {
        fprintf(stderr, "Failed to create directory list\n");
        return 1;
    }

    // Find all .git directories and extract their parent paths
    find_by_name(start_path, DOT_GIT, dir_list);

    if (dir_list->size == 0)
    {
        printf("No git repositories found.\n");
    }
    else
    {
        printf("Found %zu git repositor%s\n\n", dir_list->size, dir_list->size == 1 ? "y" : "ies");

        // Clean up each project directory
        for (size_t ii = 0; ii < dir_list->size; ii++)
        {
            char *cur_dir = (char *)al_get(dir_list, ii);
            if (!cur_dir)
                continue;

            printf("---------------------------\n");
            printf("Project %zu/%zu: %s\n", ii + 1, dir_list->size, cur_dir);
            printf("---------------------------\n");

            clean_up_targets(cur_dir);
            printf("\n");
        }
    }

    // Clean up
    al_destroy_deep(dir_list);

    printf("---------------------------\n");
    printf("Cleanup complete.\n");

    return 0;
}