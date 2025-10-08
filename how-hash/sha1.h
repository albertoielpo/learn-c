#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SHA1_LENGTH 20

/**
 * @brief Computes the SHA-1 hash of the given data.
 *
 * This function calculates the SHA-1 digest of the input data and stores
 * the result in the provided hash buffer. The caller is responsible for
 * ensuring the hash buffer is at least 20 bytes in size.
 *
 * @param[in]  data  Pointer to the data to hash. Must not be NULL.
 * @param[in]  len   Length of the data in bytes (excluding null terminator if string).
 * @param[out] hash  Pointer to buffer receiving the 20-byte hash. Must not be NULL.
 * @returns true if the hash is calculated else false
 *
 * @note The hash buffer must be at least 20 bytes (SHA1_DIGEST_LENGTH).
 *
 * Example:
 * @code
 * uint8_t hash[20];
 * const char *message = "hello world";
 * sha1((uint8_t*)message, strlen(message), hash);
 * @endcode
 */
bool sha1(const uint8_t *data, size_t len, uint8_t *hash);

/**
 * @brief Computes the SHA-1 hash of a given file
 *
 * @param[in] filename The filename
 * @param[out] hash  Pointer to buffer receiving the 20-byte hash. Must not be NULL.
 * @returns true if the hash is calculated else false
 */
bool fsha1(const char *filename, uint8_t *hash);

#endif // SHA1_H