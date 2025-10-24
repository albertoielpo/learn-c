#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sha1.h"

typedef struct
{
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
} SHA1_CTX;

#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define FSHA_BUFF_LEN 8192 // // 8kb

static void sha1_transform(SHA1_CTX *ctx, const uint8_t data[])
{
    uint32_t a, b, c, d, e, i, j, t, m[80];

    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    for (; i < 80; ++i)
        m[i] = ROTLEFT((m[i - 3] ^ m[i - 8] ^ m[i - 14] ^ m[i - 16]), 1);

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];

    for (i = 0; i < 20; ++i)
    {
        t = ROTLEFT(a, 5) + ((b & c) ^ (~b & d)) + e + 0x5a827999 + m[i];
        e = d;
        d = c;
        c = ROTLEFT(b, 30);
        b = a;
        a = t;
    }
    for (; i < 40; ++i)
    {
        t = ROTLEFT(a, 5) + (b ^ c ^ d) + e + 0x6ed9eba1 + m[i];
        e = d;
        d = c;
        c = ROTLEFT(b, 30);
        b = a;
        a = t;
    }
    for (; i < 60; ++i)
    {
        t = ROTLEFT(a, 5) + ((b & c) ^ (b & d) ^ (c & d)) + e + 0x8f1bbcdc + m[i];
        e = d;
        d = c;
        c = ROTLEFT(b, 30);
        b = a;
        a = t;
    }
    for (; i < 80; ++i)
    {
        t = ROTLEFT(a, 5) + (b ^ c ^ d) + e + 0xca62c1d6 + m[i];
        e = d;
        d = c;
        c = ROTLEFT(b, 30);
        b = a;
        a = t;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
}

static void sha1_init(SHA1_CTX *ctx)
{
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xc3d2e1f0;
    ctx->count[0] = ctx->count[1] = 0;
}

static void sha1_update(SHA1_CTX *ctx, const uint8_t data[], size_t len)
{
    uint32_t i, j;

    j = (ctx->count[0] >> 3) & 63;

    if ((ctx->count[0] += len << 3) < (len << 3))
        ctx->count[1]++;
    ctx->count[1] += (len >> 29);

    if ((j + len) > 63)
    {
        memcpy(&ctx->buffer[j], data, (i = 64 - j));
        sha1_transform(ctx, ctx->buffer);
        for (; i + 63 < len; i += 64)
        {
            sha1_transform(ctx, &data[i]);
        }
        j = 0;
    }
    else
        i = 0;

    memcpy(&ctx->buffer[j], &data[i], len - i);
}

static void sha1_final(SHA1_CTX *ctx, uint8_t hash[])
{
    uint32_t i;
    uint8_t finalcount[8];

    for (i = 0; i < 8; i++)
    {
        finalcount[i] = (uint8_t)((ctx->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255);
    }

    sha1_update(ctx, (const uint8_t *)"\200", 1);
    while ((ctx->count[0] & 504) != 448)
    {
        sha1_update(ctx, (const uint8_t *)"\0", 1);
    }
    sha1_update(ctx, finalcount, 8);

    for (i = 0; i < 20; i++)
    {
        hash[i] = (uint8_t)((ctx->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
}

/** @copydoc sha1 */
bool sha1(const uint8_t *data, size_t len, uint8_t *hash)
{
    if (data == NULL || len <= 0 || hash == NULL)
    {
        printf("Invalid parameters! data, len and hash output must be valid!\n");
        return false;
    }

    SHA1_CTX ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, data, len);
    sha1_final(&ctx, hash);

    return true;
}

/**
 * @brief is a regular file?
 *
 * If path is not a regular file than return 0
 *
 * @param[in] path
 * @return >0 if is a regular file, 0 if is not
 */
static int is_regular_file(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
        return 0; // stat failed

    // check if is a regular file
    return S_ISREG(path_stat.st_mode);
}

/** @copydoc fsha1 */
bool fsha1(const char *filename, uint8_t *hash)
{
    if (filename == NULL || hash == NULL)
    {
        printf("Invalid parameters! filename and hash output must be valid!\n");
        return false;
    }

    if (!is_regular_file(filename))
    {
        // printf("%s is not a regular file\n", filename);
        return false;
    }

    FILE *fd = fopen(filename, "r");
    if (fd == NULL)
    {
        printf("Cannot open file with name %s\n", filename);
        return false;
    }

    // init
    uint8_t buffer[FSHA_BUFF_LEN];
    size_t bytes_read;
    SHA1_CTX ctx;
    sha1_init(&ctx);

    // sha1 is incremental, so is possible to compute in chunks
    while (1)
    {
        bytes_read = fread(buffer, 1, sizeof(buffer), fd);
        if (bytes_read <= 0)
            break;
        sha1_update(&ctx, buffer, bytes_read);
    }

    fclose(fd);             // close file immediately
    sha1_final(&ctx, hash); // finalize the sha

    return true;
}

/** @copydoc fhsha1 */
bool fhsha1(Fhash *fh)
{
    if (fh == NULL || fh->filename == NULL)
    {
        printf("Invalid parameters! filename and hash output must be valid!\n");
        return false;
    }
    return fsha1(fh->filename, fh->hash);
}

/** @copydoc fhprint */
void fhprint(Fhash *fh)
{
    for (int ii = 0; ii < SHA1_LENGTH; ii++)
    {
        // print in hex format
        printf("%02x", fh->hash[ii]);
    }
    printf("  %s\n", fh->filename); // 1e4e888ac66f8dd41e00c5a7ac36a32a9950d271  test.txt
}

/** @copydoc hash_to_hex */
bool hash_to_hex(const uint8_t *hash, size_t hash_len, char *hex_str, size_t hex_str_len)
{
    if ((hash_len * 2 + 1) != hex_str_len)
    {
        // this is a safety check
        printf("hex_str_len must be double size + 1 of or hash_len\n");
        return false;
    }
    static const char hex_chars[] = "0123456789abcdef";

    for (size_t ii = 0; ii < hash_len; ii++)
    {
        hex_str[ii * 2] = hex_chars[(hash[ii] >> 4) & 0xF];
        hex_str[ii * 2 + 1] = hex_chars[hash[ii] & 0xF];
    }
    hex_str[hash_len * 2] = '\0';
    return true;
}