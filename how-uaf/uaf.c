/** Use after free (UAF) example */
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int a;
    int b;
} Data;

int main(void) {
    Data *data = malloc(sizeof(Data));
    data->a = 2;
    data->b = 3;
    printf("%p to data\n", data);
    free(data);
    printf("data now is free\n");
    // data = NULL; // should be set to NULL

    Data *data_2 = malloc(sizeof(Data));
    printf("%p to data_2\n", data_2);
    data_2->a = 20;
    data_2->b = 40;

    // "d" is freed but the pointer is still valid
    // "d" can write !
    data->a = 30;
    printf("data_2->a is expecting 20 but it gets %d because data has modified the content without the right\n", data_2->a); // 30

    // can read
    printf("data->b %d can access data d->b without the right\n", data->b); // 40

    return 0;
}