#include <stdio.h>

int main(void)
{
    // FILE is a typedef (opaque type)
    FILE *fp = fopen("stdio1.c", "r");
    if (fp == NULL)
    {
        printf("Unable to open file\n");
        return 1;
    }
    char buf[32];
    while (fread(buf, 1, sizeof(buf), fp))
    {
        for (int ii = 0; ii < 32; ii++)
        {
            putchar(buf[ii]);
        }
    }

    printf("\n");
    fclose(fp);
    return 0;
}
