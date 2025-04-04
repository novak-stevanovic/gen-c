#include "ds/gc_array.h"
#include "ds/gc_string.h"
#include "ds/gc_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

typedef struct Point
{
    int x, y;
} Point;

int main(int argc, char *argv[])
{
    Point* p1 = (Point*)malloc(sizeof(Point));
    Point* p2 = (Point*)malloc(sizeof(Point));
    Point* p3 = (Point*)malloc(sizeof(Point));
    Point* p4 = (Point*)malloc(sizeof(Point));

    p1->x = 1;
    p1->y = 1;

    p2->x = 2;
    p2->y = 2;

    p3->x = 3;
    p3->y = 3;

    p4->x = 4;
    p4->y = 4;

    printf("Done!\n");
    return EXIT_SUCCESS;
}
