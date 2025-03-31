#ifndef _GC_VECTOR_H_
#define _GC_VECTOR_H_

#include <stdlib.h>

#include "ds/gc_array.h"

#define GC_ERR_VECTOR_EMPTY 301

typedef struct _GCVector
{
    _GCArray _base;
} _GCVector;

/* ----------------------------------------------------------- */

/* Initializes the vector with specified values. Dynamically allocates enough
 * memory to store 'capacity' elements.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' is NULL, 'init_capacity' is 0 or 'el_size' is 0,
 *   2. GC_ERR_MALLOC_FAIL - Dynamic allocation for the vector's data failed. */

void _gc_vector_init(_GCVector* vector, size_t init_capacity,
        size_t el_size, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Destroys the vector. Sets its fields to default values. Frees the dynamically
 * allocated memory for vector's data field.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' is NULL. */

void _gc_vector_destroy(_GCVector* vector, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Returns address of element with position 'pos' inside the vector.
 *
 * RETURN VALUE:
 *   ON SUCCESS: adress of element with position 'pos',
 *   ON FAILURE: NULL.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void* _gc_vector_at(const _GCVector* vector, size_t pos, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Assigns specified data 'data' to element with position 'pos'.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' or 'data' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void _gc_vector_set(_GCVector* vector, const void* data, size_t pos,
        gc_err* out_err);

/* ----------------------------------------------------------- */

/* Inserts new element containing the specified data 'data' at position 'pos'.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the vector.
 * This causes all elements right of(including) 'pos' to shift rightward.
 * If the vector is at max capacity, it will attempt to resize(via realloc()).
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' or 'data' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds.
 *   3. GC_ERR_REALLOC_FAIL - vector attempted to realloc() for more memory
 *   and the realloc() call failed. */

void _gc_vector_insert_at(_GCVector* vector, const void* data, size_t pos,
        gc_err* out_err);

/* ----------------------------------------------------------- */

/* Appends new element with data 'data' to the end of the vector.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the vector.
 *
 * If the vector is at max capacity, it will attempt to resize(via realloc()).
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' or 'data' is NULL,
 *   2. GC_ERR_REALLOC_FAIL - vector attempted to realloc() for more memory
 *   and the realloc() call failed. */

void _gc_vector_push_back(_GCVector* vector, const void* data, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Removes element with position 'pos' from the vector. This causes all elements
 * right of 'pos' to shift leftward.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void _gc_vector_remove_at(_GCVector* vector, size_t pos, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Removes the last element inside the vector.
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' is NULL,
 *   2. GC_ERR_VECTOR_EMPTY - 'vector' is already empty. */

void _gc_vector_pop_back(_GCVector* vector, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Attempts to re-allocate vector's 'data' field so that it can store 'capacity'
 * elements. 'capacity' argument must be greater than or equal to current's
 * vector capacity. If the re-allocation fails, an error code will be provided
 * and the vector's internal state will remain unchanged.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' is NULL OR 'capacity' < current vector cap,
 *   2. GC_ERR_REALLOC_FAIL - realloc() returned NULL. */

void _gc_vector_realloc(_GCVector* vector, size_t capacity, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Shrinks the allocated memory for the vector's data field to match its current
 * size, minimizing unused space. This helps optimize memory usage by freeing
 * excess capacity that is no longer needed.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'vector' is NULL. */

void _gc_vector_shrink_to_fit(_GCVector* vector, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Gets vector's element count. Assumes that 'vector' is a pointer to a valid
 * vector. */
#define _gc_vector_count(vector) (vector)->_count

/* ----------------------------------------------------------- */

/* Gets vector's capacity. Assumes that 'vector' is a pointer to a valid
 * vector. */
#define _gc_vector_capacity(vector) (vector)->_capacity

/* -------------------------------------------------------------------------- */

typedef struct _GCVector GCVVector;

/* -------------------------------------------------------------------------- */

typedef struct _GCVector GCPVector;

#endif // _GC_VECTOR_H_
