#ifndef _GC_EVENT_H_
#define _GC_EVENT_H_

#include "gc_shared.h"

#include <stdbool.h>
#include <stddef.h>

/* GCEvent is an opaque pointer to the internal underlying struct "_GCEvent".
 * The GCEvent API provides methods for GCEvent creation, destruction,
 * subscribing... 
 *
 * It is important to note that the same GCEvent is raised multiple times -
 * there is only one instance of the event that is being re-raised. This
 * behavior may differ from other implementations of an event system.
 *
 * Internally, the GCEvent is esentially a more sophisticated Observer pattern.
 * The GCEvent contains an internal vector of subscribers(observers) that are
 * notified iteratively when an event occurs. */
typedef struct _GCEvent* GCEvent;

/* Event source or event subscriber. */
typedef void* GCEventParticipant;

/* Method called when an event is raised. */
typedef void (*GCEventHandler)(GCEventParticipant subscriber,
        const GCEvent event, void* data);

/* A GCEventSubscription contains the information about who the event
 * subscriber is and what method to call when the event occurs. */
struct GCEventSubscription
{
    GCEventParticipant subscriber; // non-NULL
    GCEventHandler event_handler; // non-NULL
};

/* Can be used by the library user to differentiate between event types.
 * When an event is raised, a subscriber will have a reference to the event
 * which means he will also be able to know the event type.
 *
 * The user may choose to ignore this feature altogether. */
typedef size_t gc_etype;

#define GC_EVENT_TYPE_DEFAULT 0

/* -------------------------------------------------------------------------- */

/* Dynamically allocates memory for the underlying sturct _GCEvent. Initializes
 * the struct with provided values('source' and 'type').
 *
 * RETURN VALUE:
 *   ON SUCCESS: address of the dynamically allocated opaque struct;
 *   ON FAILURE: NULL.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_ALLOC_FAIL: Failed to allocate memory for the underlying struct;
 *   3. GC_ERR_INVALID_ARG: 'source' is NULL. */
GCEvent gc_event_create(GCEventParticipant source, gc_etype type,
        gc_status* out_status);

/* Frees the dynamically allocated memory for the GCEvent and its fields.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'event' is NULL. */
void gc_event_destroy(GCEvent event, gc_status* out_status);

/* ------------------------------------------------------ */

/* Appends the GCEventSubscription to the event's internal subscription list.
 * This means that the subscriber(specified in 'subscription') will be notified
 * when the GCEvent 'event' occurs. The method called is the event_handler
 * (specified in 'subscription').
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'event' is NULL or 'subscription' is invalid
 *   ('subscriber' or 'event_handler' is NULL);
 *   3. GC_ERR_EVENT_ALR_SUB: subscriber specified in 'subscription' is
 *   already subscribed;
 *   4. GC_ERR_ALLOC_FAIL: internal vector failed to dynamically allocate
 *   more memory. */
void gc_event_subscribe(GCEvent event, struct GCEventSubscription subscription,
        gc_status* out_status);

/* Unsubscribes the 'subscriber' from the GCEvent 'event'. This is done by
 * removing the appropriate GCEventSubscription from the GCEvent's
 * internal vector.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful,
 *   2. GC_ERR_INVALID_ARG: 'event' is NULL or 'subscriber' is NULL,
 *   3. GC_ERR_EVENT_NOT_SUB: 'subscriber' is not subscribed to 'event'. */
void gc_event_unsubscribe(GCEvent event, GCEventParticipant subscriber,
        gc_status* out_status);

/* ------------------------------------------------------ */

/* Raises the specified GCEvent 'event'. This is done by iterating through
 * the event's subscribers and invoking the specified event handlers with
 * appropriate parameters.
 *
 * It is important to note that the same GCEvent is raised multiple times -
 * there is only one instance of the event that is being re-raised. This
 * behavior may differ from other implementations of an event system.
 *
 * Event context can be passed by using the 'data' parameter. This 'data'
 * is passed to GCEventHandlers which are specified when subscribing to an
 * event. */
void gc_event_raise(GCEvent event, void* data, gc_status* out_status);

/* ------------------------------------------------------ */

/* Checks if 'subscriber' is subscribed to GCEvent 'event'.
 *
 * Assumes that 'event' and 'subscriber' are valid pointers. */
bool gc_event_is_subscribed(GCEvent event, GCEventParticipant subscriber);

/* Returns the event's source. Assumes that 'event' is a valid pointer. */
GCEventParticipant gc_event_source(const GCEvent event);

/* Returns the event's type. Assumes that 'event' is a valid pointer. */
gc_etype gc_event_type(const GCEvent event);

/* ------------------------------------------------------ */

#endif // _GC_EVENT_H_
