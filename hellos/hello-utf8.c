#include <stdio.h>
#include <string.h>
#include "utf8.h"

// utf8.h usage: https://github.com/sheredom/utf8.h

int main(void)
{
    char emoji[] = "🙂";
    // 🙂 / sizeof 5 / strlen 4 / utf8len 1
    printf("%s / sizeof %ld / strlen %ld / utf8len %ld\n", emoji, sizeof(emoji), strlen(emoji), utf8len(emoji));
}