#ifndef SHA1_H
#define SHA1_H

#include <stdint.h>
#include <stddef.h>

#define SHA1_LENGTH 20

uint8_t *sha1(uint8_t *data, size_t len, uint8_t *hash);

#endif // SHA1_H