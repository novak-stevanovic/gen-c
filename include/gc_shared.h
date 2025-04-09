#ifndef _GC_SHARED_H_
#define _GC_SHARED_H_

/* -------------------------------------------------------------------------- */

typedef int gc_status;

#define GC_SUCCESS 0
#define GC_ERR_UNHANDLED 1
#define GC_ERR_ALLOC_FAIL 2
#define GC_ERR_INVALID_ARG 4
#define GC_ERR_OUT_OF_BOUNDS 5

// GCLock

#define GC_ERR_LOCK_CAP_REACHED 101

// GCArray

#define GC_ERR_ARRAY_NO_CAP 201
#define GC_ERR_ARRAY_EMPTY 202

// GCVector

#define GC_ERR_VECTOR_EMPTY 301

// GCString




// GCArena

// GCEvent

#define GC_ERR_EVENT_ALR_SUB 601
#define GC_ERR_EVENT_NOT_SUB 602


/* -------------------------------------------------------------------------- */

#endif // _GC_SHARED_H_
