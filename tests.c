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

void sub_ehandler1(GCEventParticipant subscriber,
        const GCEvent event, void* context)
{
    printf("Desio se event1 sa textom: %s\n", (char*)context);
    printf("EVENT ADDR: %p | EVENT SRC: %p | EVENT TYPE: %ld | EVENT SUB: %p\n",
            event, gc_event_source(event), gc_event_type(event), subscriber);
}

void sub_ehandler2(GCEventParticipant subscriber,
        const GCEvent event, void* context)
{
    printf("Desio se event2 sa textom: %s\n", (char*)context);
    printf("EVENT ADDR: %p | EVENT SRC: %p | EVENT TYPE: %ld | EVENT SUB: %p\n",
            event, gc_event_source(event), gc_event_type(event), subscriber);

}

int main(int argc, char *argv[])
{

    GCStringSt str1 = gc_strst("Novak123|Emilija,Djordjevic,Emili|4456");

    GCStringView sep[10];
    sep[0] = gc_strst_sv(gc_strst("ili"));
    sep[1] = gc_strst_sv(gc_strst("ak"));
    // sep[1] = gc_strst_sv(gc_strst(""));

    struct GCStringSepObject obj = gc_str_sep(gc_strst_sv(str1), sep, 2, false, NULL);

    size_t i;
    printf("%d separations\n", obj.count);
    for(i = 0; i < obj.count; i++)
    {
        // printf("%d\n", obj.views[i]._len);
        printf("%s\n", gc_str_data(gc_str_create_(obj.views[i]._data, obj.views[i]._len, NULL)));
    }



    // gc_status _status;
    //
    // GCEvent e1 = gc_event_create((void*)0x1000, 1, &_status);
    // printf("%d\n", _status);
    // GCEvent e2 = gc_event_create((void*)0x2000, 2, &_status);
    //
    // struct GCEventSubscription s1 = {
    //     .subscriber = (void*)0x1,
    //     .event_handler = sub_ehandler1
    // };
    //
    // struct GCEventSubscription s2 = {
    //     .subscriber = (void*)0x2,
    //     .event_handler = sub_ehandler2
    // };
    //
    // gc_event_subscribe(e1, s1, &_status);
    // printf("%d\n", _status);
    //
    // gc_event_subscribe(e2, s2, &_status);
    // printf("%d\n", _status);
    //
    // char c;
    // char str[100];
    // while(1)
    // {
    //     c = getchar();
    //     if(c == 'q')
    //     {
    //         break;
    //     }
    //     else if(c == '1')
    //     {
    //         printf("Enter text\n");
    //         scanf("%s", str);
    //         gc_event_raise(e1, str, &_status);
    //         assert(_status == 0);
    //     }
    //     else if(c == '2')
    //     {
    //         printf("Enter text\n");
    //         scanf("%s", str);
    //         gc_event_raise(e2, str, &_status);
    //         assert(_status == 0);
    //     }
    // }
    //
    // gc_event_destroy(e1, &_status);
    // printf("%d\n", _status);
    //
    // gc_event_destroy(e2, &_status);
    // printf("%d\n", _status);

}
