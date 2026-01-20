#include <limits.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    // in C standard types lenght is not guarantee
    // in x86_64 arch is usually like this
    char c = 'c';     // 1 byte
    short s = 1;      // 2 bytes
    int i = 2;        // 4 bytes
    long l = 3;       // 8 bytes ==> long as processor word (x86_64 == 8 bytes)
    long long ll = 4; // at least 8 bytes

    printf("char size: %lu byte (%d / %d)\n", sizeof(c), CHAR_MIN, CHAR_MAX);
    printf("short size: %lu bytes (%d / %d)\n", sizeof(s), SHRT_MIN, SHRT_MAX);
    printf("int size: %lu bytes (%d / %d)\n", sizeof(i), INT_MIN, INT_MAX);
    printf("long size: %lu bytes (%ld / %ld)\n", sizeof(l), LONG_MIN, LONG_MAX);
    printf("long long size: %lu bytes (%lld / %lld)\n", sizeof(ll), LLONG_MIN, LLONG_MAX);

    printf("*********\n");

    // to be explicit use stdint.h
    int8_t i8 = 'c';
    int16_t i16 = 1;
    int32_t i32 = 2;
    int64_t i64 = 3;
    size_t st = -4; // max allowed

    printf("int8_t size: %lu byte\n", sizeof(i8));
    printf("int16_t size: %lu bytes\n", sizeof(i16));
    printf("int32_t size: %lu bytes\n", sizeof(i32));
    printf("int64_t size: %lu bytes\n", sizeof(i64));
    printf("size_t size: %lu bytes\n", sizeof(st));

    return 0;
}
