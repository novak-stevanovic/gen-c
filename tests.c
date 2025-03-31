#include "ds/gc_array.h"
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

    GCPArray arr;
    gc_err err;
    gc_parray_init(&arr, 1000, &err);

    gc_parray_push_back(&arr, p1, &err);
    gc_parray_push_back(&arr, p2, &err);
    gc_parray_push_back(&arr, p3, &err);
    gc_parray_push_back(&arr, p4, &err);

    gc_parray_remove_at(&arr, 1, &err);
    gc_parray_remove_at(&arr, 0, &err);
    gc_parray_remove_at(&arr, 1, &err);
    gc_parray_remove_at(&arr, 0, &err);

    gc_parray_destroy(&arr, &err);

    printf("Done!\n");
    return EXIT_SUCCESS;
}
