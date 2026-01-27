/**
 * @file semver.c
 * @brief Semantic Versioning (SemVer) packed into a 32-bit integer.
 *
 * Bit layout (AABBBCCC):
 *   - [31:24] major  (8 bits,  0-255)
 *   - [23:12] minor  (12 bits, 0-4095)
 *   - [11:0]  patch  (12 bits, 0-4095)
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#include "semver.h"
#include <stdio.h>

/** @copydoc build_version */
uint32_t build_version(uint8_t major, uint16_t minor, uint16_t patch) {
    uint32_t version = 0;
    version += patch;
    version += ((uint32_t)minor << 12);
    version += ((uint32_t)major << 24);
    return version;
}

/** @copydoc print_human */
void print_human(uint32_t version) {
    printf("%u.%u.%u\n", get_major(version), get_minor(version), get_patch(version));
}

/** @copydoc get_human */
Semver get_human(uint32_t version) {
    Semver s;
    snprintf(s.value, sizeof(s.value), "%u.%u.%u", get_major(version), get_minor(version), get_patch(version));
    return s;
}

/** @copydoc get_patch */
uint32_t get_patch(uint32_t version) {
    return version & 0xFFF;
}

/** @copydoc get_minor */
uint32_t get_minor(uint32_t version) {
    return (version >> 12) & 0xFFF;
}

/** @copydoc get_major */
uint32_t get_major(uint32_t version) {
    return (version >> 24) & 0xFF;
}

/** @copydoc next_patch */
void next_patch(uint32_t *version) {
    (*version) += 1;
}

/** @copydoc next_minor */
void next_minor(uint32_t *version) {
    uint32_t patch = get_patch(*version);
    (*version) += (0x1000 - patch);
}

/** @copydoc next_major */
void next_major(uint32_t *version) {
    uint32_t patch = get_patch(*version);
    uint32_t minor = get_minor(*version);
    uint32_t minor_shifted = minor << 12;
    (*version) += (0x1000000 - minor_shifted - patch);
}

// gcc -DSEMVER_DEBUG -Wall -Wextra -Wpedantic -g -std=c99 semver.c
#ifdef SEMVER_DEBUG
int main(void) {
    uint32_t version = 0;
    next_patch(&version);
    print_human(version); // 0.0.1

    next_minor(&version);
    print_human(version); // 0.1.0

    next_patch(&version);
    print_human(version); // 0.1.1

    next_major(&version);
    print_human(version); // 1.0.0

    next_major(&version);
    next_minor(&version);
    next_minor(&version);
    next_patch(&version);
    print_human(version); // 2.2.1

    next_major(&version);
    print_human(version); // 3.0.0

    print_human(build_version(0, 0, 0));
    print_human(build_version(1, 0, 0));
    print_human(build_version(1, 5, 4));
    print_human(build_version(4, 1954, 3214));
    print_human(build_version(255, 4095, 4095));

    return 0;
}
#endif