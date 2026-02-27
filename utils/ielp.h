/**
 * Common static precompiled utility functions used across projects (e.g. print_binary).
 * Headers utils
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef IELP_H
#define IELP_H

#define print_binary(val, bits)            \
    do {                                   \
        int ii = bits;                     \
        for (; ii >= 0; ii--)              \
            printf("%d", (val >> ii) & 1); \
        printf("\n");                      \
    } while (0)

#endif // IELP_H