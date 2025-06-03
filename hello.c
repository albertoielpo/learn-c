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
    for (int ii = 0; ii < strlen(test); ii++)
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
    printf("%p %s\n", heap_str, heap_str); // bad access
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

int main(void)
{
    char emoji[] = "😃"; // 4 bytes for emoji
    char eur[] = "€";    // 3 bytes for eur
    // char problem = '€';        // this is a problem (overflow)
    printf("%s sizeof %ld strlen %ld\n", emoji, sizeof(emoji), strlen(emoji));
    printf("%s sizeof %ld strlen %ld\n", eur, sizeof(eur), strlen(eur));
}