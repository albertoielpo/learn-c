/**
 * @brief SHA-1 implementation
 *
 * SHA-1 implementation + some utils
 *
 * @note sha1_transform, sha1_init, sha1_update, sha1_final functions, are taken from public domain
 * @author Alberto Ielpo <alberto.ielpo@gmail.com>
 */
#ifndef SHA1_H
#define SHA1_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SHA1_LENGTH 20
#define SHA1_LENGTH_CHAR 41 // 40 + \0

typedef struct
{
    uint8_t hash[SHA1_LENGTH]; // 20 bytes sha1 hash
    char *filename;            // filename pointer
} Fhash;

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

/**
 * @brief wrap of fsha1 using Fhash struct
 */
bool fhsha1(Fhash *fh);

/**
 * @brief print fhash to stdout
 *
 * 1e4e888ac66f8dd41e00c5a7ac36a32a9950d271  test.txt
 * @param[in] fhash
 */
void fhprint(Fhash *fh);

/**
 * Convert uint8_t[] to string[]
 * Example SHA_1 uint8_t[20] to string[41]
 * @param[in] hash pointer to first element
 * @param[in] hash_len (20 for sha-1)
 * @param[out] hex_str (converted string of size hash_len*2+1)
 * @param[in] hex_str_len (41 for sha-1). This param is a safety check just to make sure that the dev know it
 */
bool hash_to_hex(const uint8_t *hash, size_t hash_len, char *hex_str, size_t hex_str_len);

#endif // SHA1_H