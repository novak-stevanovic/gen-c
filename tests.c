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
    GCString_ s = gc_str_("Novak");

    GCString e = gc_str("Emilija", NULL);
    //
    size_t i;
    for(i = 0; i < 2; i++)
        gc_str_cat(e, gc_sv(s), NULL);

    printf("%s\n", e->data);

    struct GCStringFindAllObject obj = gc_str_find_all(gc_sv(e), &gc_sv(s), 1, true, NULL);
    for(i = 0; i < obj.count; i++)
    {
        printf("i: %d | %d\n", i, obj.find_objects[i].str_pos);
    }

    return 0;
}
