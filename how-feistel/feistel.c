#include <stdint.h>
#include <stdio.h>

uint8_t fn_shuffle(uint8_t x) {
    // random implementation
    return ~x * 4 + 2;
}

void fn_deround(uint8_t *p) {
    uint8_t left = p[0];
    uint8_t right = p[1];

    p[1] = left;                     // right = left
    p[0] = right ^ fn_shuffle(left); // left = right XOR suffle(left)
}

// p points to the left 8 bits (L)
// [00000000][00000000]
//     L         R
void fn_round(uint8_t *p) {
    uint8_t left = p[0];
    uint8_t right = p[1];

    p[0] = right;                    // left = right
    p[1] = left ^ fn_shuffle(right); // right = left XOR shuffle(right)
}

int main() {

    // every number will have only one rounded version
    // 65031 will be 510
    // and with fn_deround it's possible to return to the original value

    // feistel network are useful in crypto or to fast shuffle data

    uint16_t value = 12345;                  // 16 bits
    uint8_t *left_value = (uint8_t *)&value; // point to the first 8 bits
    printf("x: %d\n", value);
    fn_round(left_value);
    printf("x: %d\n", value);
    fn_deround(left_value);
    printf("x: %d\n", value);

    printf("**************\n");
    // return 0;

    // print for every number from 0 to 65535 (16 bits) range it's round version
    uint16_t cur = 0;
    uint8_t *p = (uint8_t *)&cur;
    for (int ii = 0; ii < 65536; ii++) {
        cur = ii;
        printf("%d ", cur);
        fn_round(p);
        printf("%d\n", cur);
    }

    return 0;
}
