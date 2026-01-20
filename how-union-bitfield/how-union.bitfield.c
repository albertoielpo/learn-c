#include <stdint.h>
#include <stdio.h>

typedef struct
{
    uint32_t a;   // 4 bytes
    uint8_t b[4]; // 4 bytes
} foo;            // total 8 bytes

// union example
typedef struct
{
    union // 4
    {
        uint32_t a;   // same pointer
        uint8_t b[4]; // same pointer
    };

} bar;

// bitfield example
typedef struct
{
    uint8_t a : 4; // 4 bit,
    uint8_t b : 4; // 4 bit
    uint8_t c : 8; // 8 bit
} zap;

int main(void) {
    foo fool;
    fool.a = 256;
    fool.b[0] = 1;
    fool.b[1] = 2;
    fool.b[2] = 3;
    fool.b[3] = 4;

    printf("%ld bytes %d [%d %d %d %d]\n", sizeof(fool), fool.a, fool.b[0], fool.b[1], fool.b[2], fool.b[3]);

    bar barz;
    // barz.a = 65535;
    barz.b[0] = 255;
    barz.b[1] = 255;
    barz.b[2] = 0;
    barz.b[3] = 0;

    printf("%ld bytes %d [%d %d %d %d]\n", sizeof(barz), barz.a, barz.b[0], barz.b[1], barz.b[2], barz.b[3]);

    zap zaps;
    zaps.a = 15;
    // zaps.a = 17; // warning: unsigned conversion from ‘int’ to ‘unsigned char:4’ changes value from ‘17’ to ‘1’
    zaps.b = 2;
    zaps.c = 255;

    printf("%ld bytes %d %d %d \n", sizeof(zaps), zaps.a, zaps.b, zaps.c);

    return 0;
}