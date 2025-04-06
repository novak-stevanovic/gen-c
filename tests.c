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
    gc_status _status;

    GCString s = gc_str("NOVAK_EMILIJA", NULL);
    GCString s2 = gc_str("_NOVAK", NULL);

    gc_str_cat(&s, &gc_str_to_sv(&s2), &_status);
    assert(_status == 0);

    GCString n = gc_str("_", NULL);

    GCStringView v[10];
    v[0] = gc_str_to_sv(&s);
    v[1] = gc_str_to_sv(&n);

    struct GCStringFindObject obj = gc_str_rfind((GCStringView*)&s,
            v, 2, true, &_status);

    printf("%zu\n", obj.str_pos);

    return EXIT_SUCCESS;
}
