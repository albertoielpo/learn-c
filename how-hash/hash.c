#include <stdio.h>
#include "sha1.h"

int main(void)
{
    uint8_t hash[SHA1_LENGTH];
    uint8_t *data = (uint8_t *)"eccolo qui";
    sha1(data, 10, hash);
    for (int i = 0; i < SHA1_LENGTH; i++)
    {
        printf("%02x", hash[i]);
    }
    printf("\n");
    return 0;
}