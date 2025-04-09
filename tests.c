#include "ds/gc_string.h"
#include "event/gc_event.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>

typedef struct Point
{
    int x, y;
} Point;

void sub_ehandler(GCEventParticipant subscriber,
        const GCEvent event, void* context)
{
    printf("SUB1 EVENT HANDLER | SUB: %p | EVENT: %p\n", subscriber, event);
}

int main(int argc, char *argv[])
{
    gc_status _status;

    GCString s = gc_str_create("NOvak", NULL);

    GCStringView v = gc_str_substr(gc_sv(s), 0, 3, NULL);

    return 0;
}
