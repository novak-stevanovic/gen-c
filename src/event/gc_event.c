#include <stdbool.h>
#include "event/gc_event.h"
#include "_gc_shared.h"
#include "ds/gc_vector.h"
#include "gc_shared.h"
#include <assert.h>
#include <stdlib.h>

struct _GCEvent
{
    GCEventParticipant source;
    GCPVector subscribers;
};

/* Function assumes correct arguments */
static void __event_init(GCEvent event, GCEventParticipant source,
        gc_status* out_status)
{
    event->source = source;

    gc_status _status;
    event->subscribers = gc_vec_create_val(10, struct GCEventSubscription, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

/* -------------------------------------------------------------------------- */

GCEvent gc_event_create(GCEventParticipant source, gc_status* out_status)
{
    GCEvent event = (GCEvent)malloc(sizeof(struct _GCEvent));

    if(source == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
    }
    
    if(event == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
    }

    gc_status _status;
    __event_init(event, source, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(event, out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            free(event);
            GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
        default:
            free(event);
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

void gc_event_destroy(GCEvent event, gc_status* out_status)
{
    if(event == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    if(event->subscribers != NULL)
        gc_vec_destroy(event->subscribers, NULL);

    event->source = NULL;

    free(event);

    GC_VRETURN(out_status, GC_SUCCESS);
}

/* ------------------------------------------------------ */

void gc_event_subscribe(GCEvent event, struct GCEventSubscription subscription,
        gc_status* out_status)
{
    if((event == NULL) || (subscription.subscriber == NULL) ||
            (subscription.event_handler == NULL))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(gc_event_is_subscribed(event, subscription.subscriber))
    {
        GC_VRETURN(out_status, GC_ERR_EVENT_ALR_SUB);
    }

    gc_status _status;
    gc_vec_push_back_val(event->subscribers, &subscription, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void gc_event_unsubscribe(GCEvent event, GCEventParticipant subscriber,
        gc_status* out_status)
{
    if((event == NULL) || (subscriber == NULL))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    GCVVector subs = event->subscribers;
    size_t i;

    struct GCEventSubscription curr_sub;
    for(i = 0; i < gc_vec_size(subs); i++)
    {
        curr_sub = *gc_vec_at_val(subs, i, NULL, struct GCEventSubscription);

        if(curr_sub.subscriber == subscriber)
        {
            gc_status _status;
            gc_vec_remove(subs, i, &_status);

            switch(_status)
            {
                case GC_SUCCESS:
                    GC_VRETURN(out_status, GC_SUCCESS);
                default:
                    GC_VRETURN(out_status, GC_ERR_UNHANDLED);
            }
        }
    }

    GC_VRETURN(out_status, GC_ERR_EVENT_NOT_SUB);
}

/* ------------------------------------------------------ */

void gc_event_raise(GCEvent event, void* data, gc_status* out_status)
{
    if(event == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    size_t i;
    struct GCEventSubscription curr_sub;
    for(i = 0; i < gc_vec_size(event->subscribers); i++)
    {
        curr_sub = *gc_vec_at_val(event->subscribers, i, NULL,
                struct GCEventSubscription);

        curr_sub.event_handler(curr_sub.subscriber, event, data);
    }

    GC_VRETURN(out_status, GC_SUCCESS);
}

/* ------------------------------------------------------ */

bool gc_event_is_subscribed(GCEvent event, GCEventParticipant subscriber)
{
    if(event == NULL) return false;
    if(subscriber == NULL) return false;

    size_t i;
    GCVVector subs = event->subscribers;
    struct GCEventSubscription curr_sub;
    for(i = 0; i < gc_vec_size(subs); i++)
    {
        curr_sub = *gc_vec_at_val(subs, i, NULL, struct GCEventSubscription);
        if(curr_sub.subscriber == subscriber)
            return true;
    }

    return false;
}

GCEventParticipant gc_event_source(const GCEvent event)
{
    return event->source;
}
