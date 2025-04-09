#ifndef _GC_EVENT_H_
#define _GC_EVENT_H_

#include "gc_shared.h"

#include <stdbool.h>
#include <stddef.h>

/* Definitions and declarations --------------------------------------------- */

typedef struct _GCEvent* GCEvent;

typedef void* GCEventParticipant;
typedef void (*GCEventHandler)(GCEventParticipant subscriber,
        const GCEvent event, void* data);

struct GCEventSubscription
{
    GCEventParticipant subscriber;
    GCEventHandler event_handler;
};

/* Methods ------------------------------------------------------------------ */

typedef size_t gc_etype;

GCEvent gc_event_create(GCEventParticipant source,
        gc_etype event_type, gc_status* out_status);

void gc_event_destroy(GCEvent event, gc_status* out_status);

void gc_event_subscribe(GCEvent event, const struct
        GCEventSubscription subscription, gc_status* out_status);

void gc_event_unsubscribe(GCEvent event, GCEventParticipant subscriber,
        gc_status* out_status);

void gc_event_raise(GCEvent event, void* data, gc_status* out_status);

bool gc_event_is_subscribed(GCEvent event, GCEventParticipant subscriber);
GCEventParticipant gc_event_source(const GCEvent event);
gc_etype gc_event_type(const GCEvent event);

#endif // _GC_EVENT_H_
