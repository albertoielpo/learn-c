#define _GNU_SOURCE // required for asprintf
#include <errno.h>  // global errno
#include <stdarg.h> // required for ielpf
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief custom printf implementation
 *
 * @param fmt format
 * @param ... va_list
 * @return vprintf res or -1
 */
int ielpf(const char *fmt, ...) {

    va_list args;
    va_start(args, fmt);

    char *local_fmt;
    int ra = asprintf(&local_fmt, "[ielpf] %s", fmt);
    if (ra == -1) {
        fprintf(stderr, "asprintf failed: errno %d\n", errno);
        return -1;
    }

    int rv = vprintf(local_fmt, args);
    va_end(args);

    free(local_fmt);
    return rv;
}

// gcc -Wall -Wextra -Wpedantic -O2 -g -std=c99 how-printf.c
int main(void) {
    int x = 10;

    {
        // fprintf
        // print to a file (including std)
        FILE *f = fopen("test.txt", "w");
        if (f == NULL) {
            return 1;
        }
        fprintf(f, "x: %d", x);
        fclose(f);
    }

    {
        // dprintf
        // print to a file descriptor
        FILE *f = fopen("test2.txt", "w");
        if (f == NULL) {
            return 1;
        }

        dprintf(fileno(f), "x: %d", x);
        fclose(f);
    }

    printf("---------- snprintf ----------\n");
    {
        // snprintf
        // print to a user allocated string
        size_t str_len = 16;
        char str[str_len];
        int r = snprintf(str, str_len, "x: %d", x);
        if (r == -1) {
            fprintf(stderr, "snprintf failed: errno %d\n", errno);
            return 1;
        }
        printf("str is \"%s\" at pointer %p\n", str, str);
    }

    printf("---------- asprintf ----------\n");

    {
        // asprintf.
        // print to a string auto allocating the string in the heap
        char *str;
        int r = asprintf(&str, "x: %d", x);
        if (r == -1) {
            fprintf(stderr, "asprintf failed: errno %d\n", errno);
            return 1;
        }
        printf("str is \"%s\" at pointer %p\n", str, str);
        free(str);
    }

    printf("---------- custom printf -----\n");

    {
        // custom printf: ielpf
        ielpf("x: %d", x);
    }

    return 0;
}