/**
 * This program simulates the behavior of hexdump -C <filename>
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>

#define VISUAL_ROW 16
#define DOT 46

/**
 * Print human readable dec characters
 */
void print_human(uint8_t *str, size_t row_offset)
{
    size_t offset = row_offset % VISUAL_ROW == 0 ? VISUAL_ROW : row_offset % VISUAL_ROW;
    printf("\t|");
    size_t ii = 0;
    for (; ii < offset; ii++)
    {
        uint8_t cur = str[row_offset - offset + ii];
        printf("%c", isprint(cur) ? cur : DOT); // if is not printable then print a dot (".")
    }

    printf("|\n");
}

/**
 * Print pad in the dec part if is not divisible by ROW_LENGTH
 */
void print_pad(size_t pad)
{
    for (size_t ii = 0; ii < pad; ii++)
    {
        printf("    ");
    }
}

/**
 * Decdump implementation
 * It simulates the hexdump behavior except that print in dec
 */
void dec_dump(uint8_t *buf, uint8_t len, uint32_t row_counter)
{
    size_t ii = 0;
    printf("%08d  ", row_counter);
    printf("%03d ", buf[ii]); // print the left side (dec value)
    for (ii = 1; ii < len; ii++)
    {

        if (ii % 8 == 0)
        {
            putchar(' '); // additional space every 8 bytes
        }

        // this code is used only if dec_dump is invoked with len > and multiple of VISUAL_ROW
        // in this case is dead code
        // if (ii % VISUAL_ROW == 0)
        // {
        //     print_human(buf, ii); // print the right side
        // }

        printf("%03d ", buf[ii]); // print the left side (dec value)
    }

    size_t rem = len % VISUAL_ROW;
    if (rem > 0)
    {
        print_pad(VISUAL_ROW - rem); // print pad in the left side
    }
    print_human(buf, ii); // print the right side
}

/**
 * Close a file
 */
int close_file(FILE *file)
{
    if (fclose(file) == EOF)
    {
        printf("Cannot close file");
        return 1;
    }
    return 0;
}

/**
 * Read a file and produce decdump
 */
int file_dump(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Cannot open file\n");
        exit(1);
    }

    int cur;                 // current character
    uint8_t buf[VISUAL_ROW]; // line buffer. Each position is an unsigned char
    uint8_t ii = 0;          // max goes to VISUAL_ROW value
    uint32_t row_counter = 0;

    while ((cur = fgetc(file)) != EOF)
    {
        buf[ii] = cur;
        ii++;
        if (ii % VISUAL_ROW == 0 && ii != 0)
        {
            // dump
            dec_dump(buf, VISUAL_ROW, row_counter);
            ii = 0;
            row_counter += VISUAL_ROW;
        }
    }
    dec_dump(buf, ii, row_counter);
    printf("%08d\n", row_counter + ii);
    return close_file(file);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Missing filename\n");
        return 1;
    }
    return file_dump(argv[1]);
}
