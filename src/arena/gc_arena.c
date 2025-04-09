#include "arena/gc_arena.h"

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "_gc_shared.h"

#define ARENA_LOCK(arena) pthread_mutex_lock(&arena->_lock)
#define ARENA_UNLOCK(arena) pthread_mutex_unlock(&arena->_lock)

/* -------------------------------------------------------------------------- */

typedef struct Region Region;
typedef struct RegionList RegionList;

struct Region
{
    size_t _used_cap;
    size_t _total_cap;
    char* _mem_pool;

    Region* _next;
};

static Region* _region_alloc(size_t total_cap);
static void _region_destroy(Region* region);

/* -------------------------------------------------------------------------- */

struct RegionList
{
    Region* _head;
    Region* _tail;

    size_t _count;
};

static void _region_list_init(RegionList* list);
static void _region_list_push_back(RegionList* list, size_t total_cap,
        gc_status* out_status);
static void _region_list_pop_front(RegionList* list);

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

struct GCArena
{
    RegionList _regions;
    size_t _region_cap;

    Region* _rewind_it;

    pthread_mutex_t _lock;
};

static Region* _region_alloc(size_t total_cap)
{
    Region* new = (Region*)malloc(sizeof(Region));

    if(new == NULL) return NULL;

    new->_next = NULL;
    new->_total_cap = 0;
    new->_used_cap = 0;

    new->_mem_pool = malloc(total_cap);

    if(new->_mem_pool == NULL)
    {
        free(new);
        return NULL;
    }

    new->_total_cap = total_cap;

    return new;
}

static void _region_destroy(Region* region)
{
    region->_next = NULL;
    region->_total_cap = 0;
    region->_used_cap = 0;

    if(region->_mem_pool != NULL) 
        free(region->_mem_pool);
    region->_mem_pool = NULL;

    free(region);
}

/* -------------------------------------------------------------------------- */

static void _region_list_init(RegionList* list)
{
    list->_count = 0;
    list->_head = NULL;
    list->_tail = NULL;
}

static void _region_list_push_back(RegionList* list, size_t total_cap,
        gc_status* out_status)
{
    Region* new = _region_alloc(total_cap);
    if(new == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
    }

    if(list->_head == NULL)
    {
        list->_head = new;
        list->_tail = new;
    }
    else
    {
        list->_tail->_next = new;
        list->_tail = new;
    }

    list->_count++;

    GC_VRETURN(out_status, GC_SUCCESS);
}

static void _region_list_pop_front(RegionList* list)
{
    if(list->_head == list->_tail)
    {
        _region_destroy(list->_head);
        list->_head = NULL;
        list->_tail = NULL;
    }
    else
    {
        Region* old_head = list->_head;

        list->_head = list->_head->_next;

        _region_destroy(old_head);
    }

    list->_count--;
}

/* -------------------------------------------------------------------------- */

static void _gc_arena_init(GCArena arena, size_t region_cap, gc_status* out_status);
static void* _gc_arena_malloc(GCArena arena, size_t size, gc_status* out_status);

/* -------------------------------------------------------------------------- */

GCArena gc_arena_create(size_t region_cap, gc_status* out_status)
{
    GCArena new = (GCArena)malloc(sizeof(struct GCArena));
    if(new == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
    }

    gc_status _status;
    _gc_arena_init(new, region_cap, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(new, out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            free(new);
            GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_ALLOC_FAIL:
            free(new);
            GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
        default:
            free(new);
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

void gc_arena_destroy(GCArena arena, gc_status* out_status)
{
    if(arena == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    while(arena->_regions._count > 0)
        _region_list_pop_front(&arena->_regions);

    arena->_region_cap = 0;
    arena->_rewind_it = NULL;
    pthread_mutex_destroy(&arena->_lock);
    free(arena);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void* gc_arena_malloc(GCArena arena, size_t size, gc_status* out_status)
{
    if(arena == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
    }

    ARENA_LOCK(arena);

    gc_status _status;
    void* alloc_addr = _gc_arena_malloc(arena, size, &_status);

    ARENA_UNLOCK(arena);
    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(alloc_addr, out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_ALLOC_FAIL:
            GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

void* gc_arena_calloc(GCArena arena, size_t size, gc_status* out_status)
{
    if(arena == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
    }

    ARENA_LOCK(arena);

    gc_status _status;
    void* alloc_addr = _gc_arena_malloc(arena, size, &_status);

    ARENA_UNLOCK(arena);

    switch(_status)
    {
        case GC_SUCCESS:
            memset(alloc_addr, 0, size);
            GC_RETURN(alloc_addr, out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_ALLOC_FAIL:
            GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

void gc_arena_rewind(GCArena arena, gc_status* out_status)
{
    if(arena == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    ARENA_LOCK(arena);

    if(arena->_regions._count == 0) 
    {
        ARENA_UNLOCK(arena);
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    Region* it = arena->_regions._head;

    for(; it != NULL; it = it->_next)
        it->_used_cap = 0;

    // start rewinding if more regions exist
    if(arena->_regions._count > 1)
        arena->_rewind_it = arena->_regions._head;

    ARENA_UNLOCK(arena);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void gc_arena_reset(GCArena arena, gc_status* out_status)
{
    ARENA_LOCK(arena);

    while(arena->_regions._count > 0)
        _region_list_pop_front(&arena->_regions);

    gc_status err;
    _gc_arena_init(arena, arena->_region_cap, &err);

    ARENA_UNLOCK(arena);

    GC_VRETURN(out_status, GC_SUCCESS);
}

/* -------------------------------------------------------------------------- */

static void _gc_arena_init(GCArena arena, size_t region_cap, gc_status* out_status)
{
    if(region_cap == 0)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    arena->_region_cap = region_cap;
    arena->_rewind_it = NULL;
    _region_list_init(&arena->_regions);

    gc_status _status;
    _region_list_push_back(&arena->_regions, region_cap, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            pthread_mutex_init(&arena->_lock, NULL);
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

static void* _gc_arena_malloc(GCArena arena, size_t size, gc_status* out_status)
{
    if((size > arena->_region_cap) || (size == 0))
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
    }

    Region* curr_region = (arena->_rewind_it == NULL) ?
        arena->_regions._tail : arena->_rewind_it;

    size_t curr_region_cap = curr_region->_total_cap - curr_region->_used_cap;

    if(size > curr_region_cap) // not enough memory in current region
    {
        if(arena->_rewind_it == NULL) // if not rewinding, push back a region
        {
            gc_status _status;
            _region_list_push_back(&arena->_regions, arena->_region_cap, &_status);

            if(_status != GC_SUCCESS)
            {
                GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
            }
        }
        else // if rewinding, advance rewind iterator
        {
            arena->_rewind_it = arena->_rewind_it->_next;

            // if at the tail, turn of rewinding
            if(arena->_rewind_it == arena->_regions._tail)
                arena->_rewind_it = NULL;
        }

        // advance the curr_region ptr after allocing region/advancing rewind
        curr_region = curr_region->_next;
    }

    void* alloc_addr = curr_region->_mem_pool + curr_region->_used_cap;
    curr_region->_used_cap += size;

    GC_RETURN(alloc_addr, out_status, GC_SUCCESS);
}
