#include <string.h>
#include <stdio.h>
#include "sha1.h"

typedef struct
{
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
} SHA1_CTX;

#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))

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

/**
 * @copydoc sha1
 */
void sha1(const uint8_t *data, size_t len, uint8_t *hash)
{
    SHA1_CTX ctx;
    sha1_init(&ctx);
    sha1_update(&ctx, data, len);
    sha1_final(&ctx, hash);
}
