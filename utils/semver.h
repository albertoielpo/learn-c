/**
 * @file semver.h
 * @brief Semantic Versioning (SemVer) packed into a 32-bit integer.
 *
 * Bit layout (AABBBCCC):
 *   - [31:24] major  (8 bits,  0-255)
 *   - [23:12] minor  (12 bits, 0-4095)
 *   - [11:0]  patch  (12 bits, 0-4095)
 *
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef SEMVER_H
#define SEMVER_H

#include <stdint.h>

/** @brief Fixed-size buffer for a semver string (max "255.4095.4095") */
typedef struct {
    char value[14];
} Semver;

#define BUILD_VERSION(major, minor, patch) \
    (((uint32_t)(major) << 24) | ((uint32_t)(minor) << 12) | (uint32_t)(patch))

/**
 * @brief Build a packed 32-bit version from individual components
 * @param major major version (0-255)
 * @param minor minor version (0-4095)
 * @param patch patch version (0-4095)
 * @return packed 32-bit version
 */
uint32_t build_version(uint8_t major, uint16_t minor, uint16_t patch);

/**
 * @brief Print version in human-readable semver format (major.minor.patch)
 * @param version packed 32-bit version
 */
void print_human(uint32_t version);

/**
 * @brief Get version in human-readable semver format (major.minor.patch)
 * @param version packed 32-bit version
 * @return Semver containing the version string
 */
Semver get_human(uint32_t version);

/**
 * @brief Get patch version
 * Are considered only the last 12 bits
 * @param version
 * @return patch version
 */
uint32_t get_patch(uint32_t version);

/**
 * @brief Get minor version
 * Are considered only the mid 12 bits
 * @param version
 * @return minor version
 */
uint32_t get_minor(uint32_t version);

/**
 * @brief Get major version
 * Are considered only the upper 8 bits
 * @param version
 * @return major version
 */
uint32_t get_major(uint32_t version);

/**
 * @brief Increment patch version by one
 * @param version pointer to the packed version
 */
void next_patch(uint32_t *version);

/**
 * @brief Increment minor version by one and reset patch to 0
 * @param version pointer to the packed version
 */
void next_minor(uint32_t *version);

/**
 * @brief Increment major version by one and reset minor and patch to 0
 * @param version pointer to the packed version
 */
void next_major(uint32_t *version);

#endif /* SEMVER_H */
