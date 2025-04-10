#ifndef _GC_VECTOR_H_
#define _GC_VECTOR_H_

#include <stdlib.h>
#include "gc_shared.h"

/* ------------------------------------------------------------------------- */ 

typedef struct __GCVector* _GCVector;

/* -------------------------------------------------------------------------- */

/* Gets vector's size.
 * Assumes that 'vector' is a pointer to a valid vector. */

size_t gc_vec_size(const _GCVector vector);

/* ------------------------------------------------------ */

/* Gets vector's capacity.
 * Assumes that 'vector' is a pointer to a valid vector. */

size_t gc_vec_capacity(const _GCVector vector);

/* ------------------------------------------------------ */

/* Gets vector's data field.
 * Assumes that 'vector' is a pointer to a valid vector. */

void* _gc_vec_data(const _GCVector vector);

/* -------------------------------------------------------------------------- */

/* INTERNAL FUNCTION - use a convenience macro instead.
 *
 * Dynamically allocates memory for the struct __GCVector. Initializes
 * the struct with provided values. Dynamically allocates enough memory
 * to store 'capacity' elements inside the vector. Returns the opaque pointer.
 *
 * RETURN VALUES:
 *   ON SUCCESS: Address of dynamically allocated _GCVector,
 *   ON FAILURE: NULL.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_ALLOC_FAIL - Dynamic allocation failed,
 *   3. GC_ERR_INVALID_ARG - provided 'el_size' or 'capacity' is equal to 0. */

_GCVector _gc_vec_create(size_t capacity, size_t el_size, gc_status* out_status);

/* ------------------------------------------------------ */

/* Destroys the vector. Sets its fields to default values. Frees the dynamically
 * allocated memory for the vector and the vector's data field.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' is NULL. */

void gc_vec_destroy(_GCVector vector, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* INTERNAL FUNCTION - use a vector-specific macro instead.
 *
 * Returns address of element with position 'pos' inside the vector.
 *
 * RETURN VALUE:
 *   ON SUCCESS: adress of element with position 'pos',
 *   ON FAILURE: NULL.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void* _gc_vec_at(const _GCVector vector, size_t pos, gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use a vector-specific macro instead.
 *
 * Assigns specified data 'data' to element with position 'pos'.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' or 'data' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void _gc_vec_set(_GCVector vector, const void* data, size_t pos,
        gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* INTERNAL FUNCTION - use a vector-specific macro instead.
 *
 * Inserts new element containing the specified data 'data' at position 'pos'.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the vector.
 * This causes all elements right of(including) 'pos' to shift rightward.
 * If the vector is at max capacity, it will attempt to resize(via realloc()).
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' or 'data' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds.
 *   4. GC_ERR_ALLOC_FAIL - vector attempted to realloc() for more memory
 *   and the realloc() call failed. */

void _gc_vec_insert(_GCVector vector, const void* data, size_t pos,
        gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use a vector-specific macro instead.
 *
 * Appends new element with data 'data' to the end of the vector.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the vector.
 *
 * If the vector is at max capacity, it will attempt to resize(via realloc()).
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' or 'data' is NULL,
 *   3. GC_ERR_ALLOC_FAIL - vector attempted to realloc() for more memory
 *   and the realloc() call failed. */

void _gc_vec_push_back(_GCVector vector, const void* data, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* Removes element with position 'pos' from the vector. This causes all elements
 * right of 'pos' to shift leftward.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void gc_vec_remove(_GCVector vector, size_t pos, gc_status* out_status);

/* ------------------------------------------------------ */

/* Removes the last element inside the vector.
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' is NULL,
 *   3. GC_ERR_VECTOR_EMPTY - 'vector' is already empty. */

void gc_vec_pop_back(_GCVector vector, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* Attempts to re-allocate vector's 'data' field so that it can store 'capacity'
 * elements. 'capacity' argument must be greater than or equal to current's
 * vector capacity. If the re-allocation fails, an error code will be provided
 * and the vector's internal state will remain unchanged.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' is NULL OR 'capacity' < current vector 
 *   capacity,
 *   3. GC_ERR_ALLOC_FAIL - realloc() returned NULL. */

void gc_vec_reserve(_GCVector vector, size_t capacity, gc_status* out_status);

/* ------------------------------------------------------ */

/* Shrinks the allocated memory for the vector's data field to match its current
 * size, minimizing unused space. This helps optimize memory usage by freeing
 * excess capacity that is no longer needed.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'vector' is NULL. */

void gc_vec_fit(_GCVector vector, gc_status* out_status);

/* CONVENIENCE MACROS ------------------------------------------------------- */

/* 'type' refers to the data type stored inside the vector */
#define gc_vec_data(vec, type) (type *)_gc_vec_data(vec)

/* Below are "value vector" and "pointer vector" specific macros. Use these 
 * instead of internal functions/macros defined above
 * (they are prefixed by '_') */

/* ------------------------------------------------------ */

/* Value vector - stores copies of values inside the vector.
 * 'type' argument inside some of the macros below refers to the value type
 * stored inside the vector - if the vector stores ints, it should be 'int'. */

typedef _GCVector GCVVector;

#define gc_vec_create_val(init_cap, type, out_status) \
    _gc_vec_create((init_cap), sizeof(type), (out_status))

/* Returns pointer to the element inside the vector and peforms a cast to the
 * specified type 'type'. */
#define gc_vec_at_val(vvector, pos, out_status, type) \
    (type *)_gc_vec_at((vvector), (pos), (out_status))

#define gc_vec_set_val(vvector, data, pos, out_status) \
    _gc_vec_set((vvector), (data), (pos), (out_status))

#define gc_vec_insert_val(vvector, data, pos, out_status) \
    _gc_vec_insert((vvector), (data), (pos), (out_status))

#define gc_vec_push_back_val(vvector, data, out_status) \
    _gc_vec_push_back((vvector), (data), (out_status))

/* -------------------------------------------------------------------------- */

/* Pointer vector - stores pointers inside the vector.
 * 
 * GCPVector provides convienent functions to help work with pointer vectors.
 * This is important because functions like _gc_vec_push_back() expect
 * parameter 'data' to be pointer to actual data held inside the vector.
 * Since the vector holds pointers, this would require 'data' to be a double pointer.
 *
 * These macros allow the user not to worry about double pointers and just provide
 * single pointers instead.
 *
 * 'type' argument inside some of the macros below refers to the value type
 * stored inside the vector - if the vector stores int*, it should be 'int*'. */

typedef _GCVector GCPVector;

#define gc_vec_create_ptr(init_cap, out_status) \
    _gc_vec_create((init_cap), sizeof(void*), (out_status))

/* Finds the address of element inside the vector with position 'pos'.
 * This is a double pointer, because the element itself is a pointer.
 * It casts this double pointer to the appropriate type. Then, the
 * double pointer is dereferenced so that a single pointer is returned. */
#define gc_vec_at_ptr(vec, pos, out_status, type) \
    (type *)_gc_vec_at((vec), (pos), (out_status))

#define gc_vec_set_ptr(vec, data, pos, out_status) \
    _gc_vec_set((vec), &(data), (pos), (out_status))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_vec_insert_ptr(vec, data, pos, out_status) \
    _gc_vec_insert((vec), &(data), (pos), (out_status))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_vec_push_back_ptr(vec, data, out_status) \
    _gc_vec_push_back((vec), &(data), (out_status))

#endif // _GC_VECTOR_H_
