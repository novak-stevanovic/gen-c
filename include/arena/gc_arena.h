#ifndef _GC_ARENA_H_
#define _GC_ARENA_H_

#include "gc_shared.h"
#include <stdlib.h>

/* -------------------------------------------------------------------------- */

/* GCArena is a simple thread-safe arena allocator. It contains regions organized
 * into a region list. When the tail of the region list runs out of memory in
 * its internal memory pool, a new region is pushed back to the list.
 *
 * The GCArena may also be rewinded, which will empty all of the allocated
 * regions (and their internal memory pools), allowing the user to
 * re-populate the once-used memory pools.
 *
 * This arena may perform poorly if 'region_cap' is too small relative to the
 * typical allocation size.
 *
 * It is important to note that each GCArena object has its own lock,
 * assuring optimal performance in a multi-threaded environment. Thread-safety
 * is achieved using pthreads. */

typedef struct GCArena* GCArena;

/* -------------------------------------------------------------------------- */

/* Dynamically allocates memory for 'struct GCArena' and initializes it. 
 * Dynamically allocates memory for the first memory region, initializes it
 * (which also means that it dynamically allocates memory for its memory pool)
 * and pushes it back to the region list.
 *
 * Each region inside the arena will be capable of holding at most
 * 'region_cap' bytes of data.
 *
 * STATUS CODES: 
 *   1. GC_SUCCESS - function call was successful;
 *   3. GC_ERR_INVALID_ARG - if 'region_cap' is 0; 
 *   3. GC_ERR_ALLOC_FAIL - if malloc() fails.
 *
 * RETURN VALUE:
 *   ON SUCCESS: address of the newly-allocated struct GCArena;
 *   ON FAILURE: NULL. */

GCArena gc_arena_create(size_t region_cap, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* Destroys the arena. This will destroy every region inside the region list.
 * 'Destroying a region implies freeing the memory occupied by the region's
 * memory pool. This also frees the dynamically allocated memory to
 * store the GCArena object itself. */

void gc_arena_destroy(GCArena arena, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* This function allocates memory within the arena. It finds the currently active
 * region in the list and attempts to allocate the requested memory size within
 * its internal memory pool. 'Currently active region' refers to:
 *
 * 1) If 'rewind mode' is off, the tail of the region list.
 * If that region does not have enough space, a new region may be created and
 * appended to the end of the list.
 *
 * 2) If 'rewind mode' is on, the first region containing free space inside its
 * memory pool. If that region does not have enough space, the next region will
 * be considered. 
 *
 * STATUS CODES: 
 *   1. GC_SUCCESS - function call was successful;
 *   2. GC_ERR_INVALID_ARG - if 'size' is 0 or greater than arena's 'region_cap';
 *   3. GC_ERR_ALLOC_FAIL - if malloc() fails.
 *
 * Calling gc_arena_destroy multiple times on a single GCArena is undefined
 * behavior.
 *
 * RETURN VALUE:
 *   ON SUCCESS: address of the newly-allocated memory inside the arena
 *   of 'size' bytes;
 *   ON FAILURE: NULL. */

void* gc_arena_malloc(GCArena arena, size_t size, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* This function allocates a zero-initialized memory block of the given size 
 * from the GCArena. It behaves similarly to gc_arena_malloc,
 * but ensures that the allocated memory is filled with zeros.

 * RETURN VALUE and STATUS CODES are the same as those for gc_arena_malloc. */

void* gc_arena_calloc(GCArena arena, size_t size, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* This function resets the arena by marking all allocated memory within
 * existing regions as available for reuse. It does not free any memory
 * but instead sets all regions' used capacity to zero. 
 * If multiple regions exist, the arena enters 'rewind mode' allowing
 * previously allocated regions to be reused in order.
 *
 * STATUS CODES: 
 *   1. GC_SUCCESS - function call was successful;
 *   2. GC_ERR_INVALID_ARG - if 'arena' is NULL. */

void gc_arena_rewind(GCArena arena, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* This function deallocates all allocated regions except the first one.
 * This means that all memory occupied by those regions will be freed.
 * The first region will be reset, making its memory available for reuse.
 * After this call, the arena will be in the same state as immediately
 * after gc_arena_init().
 *
 * STATUS CODES: 
 *   1. GC_SUCCESS - function call was successful;
 *   2. GC_ERR_INVALID_ARG - if 'arena' is NULL. */

void gc_arena_reset(GCArena arena, gc_status* out_status);

/* -------------------------------------------------------------------------- */

#endif // _GC_ARENA_H_
