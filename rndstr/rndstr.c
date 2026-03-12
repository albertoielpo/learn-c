/**
 * Random string generator
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#include "../utils/semver.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

// *_LEN does not contains 0 at the end
#define AC_LEN 94
#define DEF_RND_LEN 20

uint32_t VERSION = BUILD_VERSION(1, 0, 0);

int main(int argc, char const *argv[]) {

    int rnd_len = DEF_RND_LEN;
    if (argc == 2) {
        rnd_len = atoi(argv[1]);
        if (rnd_len == 0) {
            Semver semver = get_human(VERSION);
            fprintf(stderr, "Random string generator v%s\nUsage: %s [number]\n", semver.value, argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    char allowed_characters[AC_LEN + 1] = {0};

    for (int ii = 0, kk = 33; ii < AC_LEN; kk++, ii++)
        allowed_characters[ii] = kk;

    char rnd_str[rnd_len + 1];
    rnd_str[rnd_len] = 0;

    for (int ii = 0; ii < rnd_len; ii++) {
        uint32_t n;
        if (getrandom(&n, sizeof(n), 0) != sizeof(n)) {
            fprintf(stderr, "Cannot get random\n");
            exit(EXIT_FAILURE);
        }

        rnd_str[ii] = allowed_characters[n % AC_LEN];
    }

    printf("%s\n", rnd_str);

    return EXIT_SUCCESS;
}
