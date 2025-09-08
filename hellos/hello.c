#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_SIZE 10

void print_it(char *x)
{
    printf("%s\n", x);
    x[1] = 'y';
}

// string example
int main2(void)
{
    char test[] = "test"; // sizeof 5 strlen 4
    printf("%ld\n", sizeof(test));
    for (size_t ii = 0; ii < strlen(test); ii++)
    {
        printf("%c\n", test[ii]);
    }
    print_it(test); // test and &test[0] are equals if test is an array and the function accept a *
    print_it(&test[0]);
    printf("%s\n", test);
    return 0;
}

void modify_int(int *src, int dst)
{
    *src = dst; // assign dst to the ref in memory of src
}

void swap_int(int *src, int *dst)
{
    int tmp = *src;
    *src = *dst;
    *dst = tmp;
}

int main_3(void)
{
    // int x = 5;
    // int *y = &x;
    // // *y = 10;                                 // dereference the pointer and change the content
    // modify_int(&x, 40);
    // printf("%d %d %ld\n", x, *y, sizeof(x)); // now the src and the dst are 10

    // swap int
    int x = 10;
    int y = 20;
    printf("x:%d y:%d\n", x, y);
    swap_int(&x, &y);
    printf("x:%d y:%d\n", x, y);
    return 0;
}

int main_str(void)
{
    // this string is allocated in the stack
    char stack_str[] = "my_string"; // read write
    printf("%p %s\n", stack_str, stack_str);
    // the string is mutable
    stack_str[0] = 'M';
    printf("%p %s\n", stack_str, stack_str);

    puts("****************");

    // this string is allocated in the static (read only)
    char *static_str = "my_string";
    printf("%p %s\n", static_str, static_str);
    // the string is immutable
    // static_str[0] = 'M'; // Segmentation fault (core dumped)
    // printf("%s\n", static_str);

    puts("****************");

    // this string is allocated in the heap (read write)
    char *heap_str = malloc(sizeof(char) * 10); // 10 chars (9+\0)
    strcpy(heap_str, "my_string");
    printf("%p %s\n", heap_str, heap_str);
    // the string is mutable
    heap_str[0] = 'A';
    printf("%p %s\n", heap_str, heap_str);

    // memory can be deallocated
    free(heap_str);
    // printf("%p %s\n", heap_str, heap_str); // bad access
    return 0;
}

// int main(void)
// {
//     // int a[] = {1, 2, 3, 4};
//     // a[0] = 2;
//     // for (int ii = 0; ii < sizeof(a) / sizeof(a[0]); ii++)
//     // {
//     //     printf("%d\n", a[ii]);
//     // }

//     char data[DATA_SIZE];
//     printf("Enter data: \n");
//     scanf("%[^\n]", data); // read all until new line

//     printf("sizeof: %ld strlen: %ld\n", sizeof(data), strlen(data));
//     for (int ii = 0; ii < strlen(data); ii++)
//     {
//         // if used strlen is not required to if break with \0
//         // if (data[ii] == '\0')
//         //     break;
//         printf("%c", data[ii]);
//     }

//     printf("\n");

//     return 0;
// }

int main_utf8(void)
{
    char emoji[] = "😃"; // 4 bytes for emoji
    char eur[] = "€";    // 3 bytes for eur
    // char problem = '€';        // this is a problem (overflow)
    printf("%s sizeof %ld strlen %ld\n", emoji, sizeof(emoji), strlen(emoji));
    printf("%s sizeof %ld strlen %ld\n", eur, sizeof(eur), strlen(eur));
    return 0;
}

void print_binary_recursive(unsigned int x, int bits)
{

    if (bits == 0)
        return;
    // First print all higher bits
    print_binary_recursive(x >> 1, bits - 1);
    // Then print this bit (LSB of the shifted value above)
    putchar(((x & 1U) ? '1' : '0'));
}

int main(void)
{
    unsigned char a = 32;
    unsigned char b = ~a; // 223
    printf("%u %u\n", a, b);
    print_binary_recursive(a, 8);
    printf("\n");
    print_binary_recursive(b, 8);
    printf("\n*********\n");

    unsigned int number = 0b1111;
    // shift to right
    unsigned int number_right = number >> 2;
    // shift to left
    unsigned int number_left = number << 2;
    print_binary_recursive(number, 8);
    printf("\n");
    print_binary_recursive(number_right, 8);

    printf("\n");
    print_binary_recursive(number_left, 8);

    // 00001111
    // 00000011 // shift right
    // 00111100 // shift left
    // 15 3 60  // in decimal
    printf("\n%d %d %d\n", number, number_right, number_left);

    char *ab = calloc(1, sizeof(char));
    char *buffer = ab;
    *buffer = 'A';
    printf("%p %d %d", ab, *ab, *buffer);
    free(ab);

    return 0;
}