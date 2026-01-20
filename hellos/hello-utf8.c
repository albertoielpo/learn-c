#include "utf8.h"
#include <stdio.h>
#include <string.h>

// utf8.h usage: https://github.com/sheredom/utf8.h

int main(void) {
    // utf8_int8_t is char (signed) to facilitate string literal init
    // signed overflow in not guarantee across C version / compiler and so on...
    // in contrary unsigned overflow is guarantee and for this reason utf8 is unsigned
    utf8_int8_t emoji[] = "😀";
    utf8_int8_t *emoj = "😀";
    unsigned char smile[] = {240, 159, 152, 128, 0};
    uint8_t another_smile[] = {240, 159, 152, 128, 0};

    printf("%s / sizeof %ld / strlen %ld / utf8len %ld\n", emoji, sizeof(emoji), strlen(emoji), utf8len(emoji)); // 😀 / sizeof 5 / strlen 4 / utf8len 1
    printf("%s %s %s %s\n", emoji, emoj, smile, another_smile);                                                  // 😀 😀 😀 😀
}