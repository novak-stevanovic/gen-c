#include <stdbool.h>
#include "event/gc_event.h"
#include "_gc_shared.h"
#include "gc_shared.h"
#include <assert.h>
#include <stdlib.h>

struct GCEventSubListNode
{
    struct GCEventSubscription subscription;
    struct GCEventSubListNode* next;
};

typedef struct GCEventSubList
{
    struct GCEventSubListNode* head;
    struct GCEventSubListNode* tail;

    size_t count;
} GCEventSubList;

typedef struct _GCEvent 
{
    gc_etype _type;

    GCEventParticipant _source;
    GCEventSubList _subscriptions;
} _GCEvent;

/* -------------------------------------------------------------------------- */

struct GCEventSubListNode* _event_subscription_node_alloc(
        struct GCEventSubscription subscription)
{
    struct GCEventSubListNode* new = (struct GCEventSubListNode*)
        malloc(sizeof(struct GCEventSubListNode));

    if(new == NULL) return NULL;

    new->subscription = subscription;
    new->next = NULL;

    return new;
}

static void _event_sub_list_init(GCEventSubList* list)
{
    if(list == NULL) return;

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

static void _event_sub_list_pop_front(GCEventSubList* list)
{
    if(list->count == 0) return;

    struct GCEventSubListNode* old_head = list->head;
    list->head = list->head->next;
    free(old_head);

    list->count--;

    if(list->count == 0)
        list->tail = NULL;
}

static void _event_sub_list_destroy(GCEventSubList* list)
{
    if(list == NULL) return;

    while(list->count > 0)
        _event_sub_list_pop_front(list);

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

static bool _event_sub_list_is_subbed(GCEventSubList* list,
        const GCEventParticipant subscriber)
{
    struct GCEventSubListNode* it;
    for(it = list->head; it != NULL; it = it->next)
    {
        if(it->subscription.subscriber == subscriber)
            return true;
    }

    return false;
}

static void _event_sub_list_add_sub(GCEventSubList* list,
        const struct GCEventSubscription subscription, gc_status* out_status)
{
    if(list == NULL) return;

    if(_event_sub_list_is_subbed(list, subscription.subscriber))
    {
        GC_VRETURN(out_status, GC_ERR_EVENT_ALR_SUB);
    }

    struct GCEventSubListNode* new =
        _event_subscription_node_alloc(subscription);

    if(new == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
    }

    if(list->tail != NULL) // add to tail
        list->tail->next = new;
    else // add head
        list->head = new;

    list->count++;

    GC_VRETURN(out_status, GC_SUCCESS);
}

static void _event_sub_list_remove_sub(GCEventSubList* list,
        const GCEventParticipant subscriber, gc_status* out_status)
{
    if(list == NULL) return;
    if(list->count == 0) return;

    if(list->head->subscription.subscriber == subscriber)
    {
        _event_sub_list_pop_front(list);
        return;
    }

    struct GCEventSubListNode* it;
    struct GCEventSubListNode* prev = NULL;
    for(it = list->head; it != NULL; prev = it, it = it->next)
    {
        if(it->subscription.subscriber == subscriber)
        {
            prev->next = it->next;

            if(list->tail == it)
                list->tail = prev;

            free(it);

            list->count--;

            GC_VRETURN(out_status, GC_SUCCESS);
        }
    }

    GC_VRETURN(out_status, GC_ERR_EVENT_NOT_SUB);
}

/* -------------------------------------------------------------------------- */

GCEvent gc_event_create(GCEventParticipant source,
        gc_etype event_type, gc_status* out_status)
{
    if(source == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
    }

    GCEvent event = (GCEvent)malloc(sizeof(struct _GCEvent));
    if(event == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
    }

    event->_type = event_type;

    event->_source = source;

    _event_sub_list_init(&event->_subscriptions);

    GC_RETURN(event, out_status, GC_SUCCESS);
}

void gc_event_destroy(GCEvent event, gc_status* out_status)
{
    if(event == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    event->_source = NULL;
    _event_sub_list_destroy(&event->_subscriptions);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void gc_event_subscribe(GCEvent event, const struct
        GCEventSubscription subscription, gc_status* out_status)
{
    gc_status _status;
    _event_sub_list_add_sub(&event->_subscriptions, subscription, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        case GC_ERR_EVENT_ALR_SUB:
            GC_VRETURN(out_status, GC_ERR_EVENT_ALR_SUB);
    }
}

void gc_event_unsubscribe(GCEvent event, GCEventParticipant subscriber,
        gc_status* out_status)
{
    gc_status _status;

    _event_sub_list_remove_sub(&event->_subscriptions, subscriber, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        case GC_ERR_EVENT_ALR_SUB:
            GC_VRETURN(out_status, GC_ERR_EVENT_ALR_SUB);
    }
}

void gc_event_raise(GCEvent event, void* context,
        gc_status* out_status)
{
    struct GCEventSubListNode* it;
    for(it = event->_subscriptions.head; it != NULL; it = it->next)
    {
        it->subscription.event_handler(it->subscription.subscriber,
                event, context);
    }
}

GCEventParticipant gc_event_source(const GCEvent event)
{
    return event->_source;
}

gc_etype gc_event_type(const GCEvent event)
{
    return event->_type;
}
