#include <stdio.h>
#include <stdlib.h>

// create a custom type called pippo of type int
typedef int pippo;

// struct definition
// custom type that contains values
typedef struct
{
    int x;
    int y;
} Point;

void edit_x_point(Point *p, int new_value) {
    // (*p).x = 99; // can be written as p->x = 99
    p->x = new_value;
}

int main(void) {
    // init
    Point p1 = {10, 20};

    // or init in this way
    Point p2;
    p2.x = 15;
    p2.y = 25;

    Point p3 = {.x = 5, .y = 15};

    edit_x_point(&p1, 99);

    printf("(%d,%d) - (%d,%d) - (%d,%d)\n", p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);

    // work as int
    pippo ciao = 5;
    printf("%d\n", ciao);

    // can be allocated also dyn
    Point *p4 = malloc(sizeof(Point));
    p4->x = 10;
    p4->y = 20;
    edit_x_point(p4, 30);
    printf("(%d,%d)", p4->x, p4->y);

    printf("\n************\n");

    // matrix
    int data[2][3] = {{1, 2, 3}, {4, 5, 6}};
    for (int ii = 0; ii < 2; ii++) {
        for (int jj = 0; jj < 3; jj++) {
            printf("%d", data[ii][jj]);
        }
    }
    free(p4);
    printf("\n************\n");

    double a = 0.01;
    double b = 0.05;

    printf("%f\n", a + b);
    printf("%.15f\n", a + b); // formatted print

    printf("************\n");
    // array of points with malloc
    int point_length = 3;
    Point *points = malloc(sizeof(Point) * point_length);

    /**
     * Syntax: points[0] == (*(points + 1)).x == (points + 1)->x
     */
    points[0].x = 1;
    points[0].y = 1;
    (*(points + 1)).x = 2;
    (*(points + 1)).y = 2;
    (points + 2)->x = 3;
    (points + 2)->y = 3;

    for (int ii = 0; ii < point_length; ii++) {
        printf("%d (%d,%d)\n", ii, points[ii].x, points[ii].y);
    }

    free(points);

    return 0;
}
