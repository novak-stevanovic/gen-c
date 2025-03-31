#ifndef _GC_ARRAY_H_
#define _GC_ARRAY_H_

#include "gc_shared.h"
#include <stdlib.h>

#define GC_ERR_ARRAY_NO_CAP 201
#define GC_ERR_ARRAY_EMPTY 202

typedef struct _GCArray
{
    /* capacity - the array has enough memory allocated to store this many
     * elements. */
    size_t _capacity;

    /* count - the array currently has this many elements. */
    size_t _count;

    /* data - pointer to dynamically allocated chunk of memory reserved for
     * array's elements. */
    void* _data;
    
    /* el_size - size of single element(bytes). */
    size_t _el_size;
} _GCArray;

/* ----------------------------------------------------------- */

/* Initializes the array with specified values. Dynamically allocates enough
 * memory to store 'capacity' elements.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' is NULL, 'capacity' is 0 or 'el_size' is 0,
 *   2. GC_ERR_MALLOC_FAIL - Dynamic allocation for the array's data failed. */

void _gc_array_init(_GCArray* array, size_t capacity,
        size_t el_size, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Destroys the array. Sets its fields to default values. Frees the dynamically
 * allocated memory for array's data field.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' is NULL. */

void _gc_array_destroy(_GCArray* array, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Returns address of element with position 'pos' inside the array.
 *
 * RETURN VALUE:
 *   ON SUCCESS: adress of element with position 'pos',
 *   ON FAILURE: NULL.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void* _gc_array_at(const _GCArray* array, size_t pos, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Assigns specified data 'data' to element with position 'pos'.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' or 'data' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void _gc_array_set(_GCArray* array, const void* data, size_t pos,
        gc_err* out_err);

/* ----------------------------------------------------------- */

/* Inserts new element containing the specified data 'data' at position 'pos'.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the array.
 * This causes all elements right of(including) 'pos' to shift rightward.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' or 'data' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds.
 *   3. GC_ERR_ARRAY_NO_CAP - 'array' doesn't have enough capacity to insert
 *   new elements. */

void _gc_array_insert_at(_GCArray* array, const void* data, size_t pos,
        gc_err* out_err);

/* ----------------------------------------------------------- */

/* Appends new element with data 'data' to the end of the array.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the array.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' or 'data' is NULL,
 *   2. GC_ERR_ARRAY_NO_CAP - 'array' doesn't have enough capacity to append
 *   new elements. */

void _gc_array_push_back(_GCArray* array, const void* data, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Removes element with position 'pos' from the array. This causes all elements
 * right of 'pos' to shift leftward.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   2. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void _gc_array_remove_at(_GCArray* array, size_t pos, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Removes the last element inside the array.
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   2. GC_ERR_ARRAY_EMPTY - 'array' is already empty. */

void _gc_array_pop_back(_GCArray* array, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Attempts to re-allocate array's 'data' field so that it can store 'capacity'
 * elements. 'capacity' argument must be greater than or equal to current's
 * array capacity. If the re-allocation fails, an error code will be provided
 * and the array's internal state will remain unchanged.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' is NULL OR 'capacity' < current array cap,
 *   2. GC_ERR_REALLOC_FAIL - realloc() returned NULL. */

void _gc_array_realloc(_GCArray* array, size_t capacity, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Shrinks the allocated memory for the array's data field to match its current
 * size, minimizing unused space. This helps optimize memory usage by freeing
 * excess capacity that is no longer needed.
 *
 * ERROR CODES:
 *   1. GC_ERR_INVALID_ARG - 'array' is NULL. */

void _gc_array_shrink_to_fit(_GCArray* array, gc_err* out_err);

/* ----------------------------------------------------------- */

/* Gets array's element count. Assumes that 'array' is a pointer to a valid
 * array. */
#define _gc_array_count(array) (array)->_count

/* ----------------------------------------------------------- */

/* Gets array's capacity. Assumes that 'array' is a pointer to a valid
 * array. */
#define _gc_array_capacity(array) (array)->_capacity

/* -------------------------------------------------------------------------- */

/* Value array - stores copies of values inside the array.
 * 'type' argument inside some of the macros below refers to the value type
 * stored inside the array - if the array stores ints, it should be 'int'. */

typedef struct _GCArray GCVArray;

#define gc_varray_init(varray, init_cap, type, out_err) \
    _gc_array_init((varray), (init_cap), sizeof((type)), (out_err))

#define gc_varray_destroy(varray, out_err) \
    _gc_array_destroy((varray), (out_err))

/* Returns pointer to the element inside the array and peforms a cast to the
 * specified type 'type'. */
#define gc_varray_at(varray, pos, out_err, type) \
    (type *)_gc_array_at((varray), (pos), (out_err))

/* Dereferences the result of gc_varray_at() to obtain the value stored
 * at the specified index. This is dangerous because gc_varray_at may
 * return NULL if it fails. Use with caution. */
#define gc_varray_get(varray, pos, out_err, type) \
    *(gc_varray_at((varray), (pos), NULL, (type)))

#define gc_varray_set(varray, data, pos, out_err) \
    _gc_array_set((varray), (data), (pos), (out_err))

#define gc_varray_insert_at(varray, data, pos, out_err) \
    _gc_array_insert_at((varray), (data), (pos), (out_err))

#define gc_varray_push_back(varray, data, out_err) \
    _gc_array_push_back((varray), (data), (out_err))

#define gc_varray_remove_at(varray, pos, out_err) \
    _gc_array_remove_at((varray), (pos), (out_err))

#define gc_varray_pop_back(varray, out_err) \
    _gc_array_pop_back((varray), (out_err))

#define gc_varray_realloc(varray, cap, out_err) \
    _gc_array_realloc((varray), (cap), (out_err))

#define gc_varray_shrink_to_fit(varray, out_err) \
    _gc_array_shrink_to_fit((varray), (out_err))

#define gc_varray_count(varray) \
    _gc_array_count(varray)

#define gc_varray_capacity(varray) \
    _gc_array_capacity(varray)

/* -------------------------------------------------------------------------- */

/* Pointer array - stores pointers inside the array.
 * 
 * GCPArray provides convienent functions to help work with pointer arrays.
 * This is important because functions like _gc_array_push_back() expect 'data'
 * to be pointer to *data held inside the array*. Since the array holds pointers,
 * this would require 'data' to be a double pointer.
 *
 * These macros allow the user not to worry about double pointers and just provide
 * single pointers instead.
 *
 * 'type' argument inside some of the macros below refers to the value type
 * stored inside the array - if the array stores int*, it should be 'int*'. */

typedef struct _GCArray GCPArray;

#define gc_parray_init(parray, init_cap, out_err) \
    _gc_array_init((parray), (init_cap), sizeof(void*), (out_err))

#define gc_parray_destroy(parray, out_err) \
    _gc_array_destroy((parray), (out_err))

/* Returns the address of the pointer stored in the array, performing an
 * appropriate cast. Since the array stores pointers, the returned address
 * is a double pointer. */
#define gc_parray_at(parray, pos, out_err, type) \
    (type *)_gc_array_at((parray), (pos), (out_err))

/* Dereferences the result of gc_parray_at() to obtain the pointer stored
 * at the specified index. This is dangerous because gc_parray_at may
 * return NULL if it fails. Use with caution. */
#define gc_parray_get(parray, pos, out_err, type) \
    *(gc_parray_at((parray), (pos), (NULL), (type)))

#define gc_parray_set(parray, data, pos, out_err) \
    _gc_array_set((parray), &(data), (pos), (out_err))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_parray_insert_at(parray, data, pos, out_err) \
    _gc_array_insert_at((parray), &(data), (pos), (out_err))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_parray_push_back(parray, data, out_err) \
    _gc_array_push_back((parray), &(data), (out_err))

#define gc_parray_remove_at(parray, pos, out_err) \
    _gc_array_remove_at((parray), (pos), (out_err))

#define gc_parray_pop_back(parray, out_err) \
    _gc_array_pop_back((parray), (out_err))

#define gc_parray_realloc(parray, cap, out_err) \
    _gc_array_realloc((parray), (cap), (out_err))

#define gc_parray_shrink_to_fit(parray, out_err) \
    _gc_array_shrink_to_fit((parray), (out_err))

#define gc_parray_count(parray) \
    _gc_array_count(parray)

#define gc_parray_capacity(parray) \
    _gc_array_capacity(parray)

#endif // _GC_ARRAY_H_
