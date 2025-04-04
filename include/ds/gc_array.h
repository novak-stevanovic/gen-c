#ifndef _GC_ARRAY_H_
#define _GC_ARRAY_H_

#include "gc_shared.h"
#include <stdlib.h>

#define GC_ERR_ARRAY_NO_CAP 201
#define GC_ERR_ARRAY_EMPTY 202

typedef struct _GCArray
{
    /* size - the array currently has this many elements */
    size_t _size;

    /* capacity - the array has enough memory allocated to store this many
     * elements */
    size_t _capacity;

    /* data - pointer to dynamically allocated chunk of memory reserved for
     * array's elements */
    void* _data;
    
    /* el_size - size of single element(bytes) */
    size_t _el_size;
} _GCArray;

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use an array-specific macro instead.
 *
 * Initializes the array with specified values. Dynamically allocates enough
 * memory to store 'capacity' elements.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL, 'capacity' is 0 or 'el_size' is 0,
 *   3. GC_ERR_MALLOC_FAIL - Dynamic allocation for the array's data failed. */

void _gc_arr_init(_GCArray* array, size_t capacity,
        size_t el_size, gc_status* out_status);

/* ------------------------------------------------------ */

/* Destroys the array. Sets its fields to default values. Frees the dynamically
 * allocated memory for array's data field.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL. */

void gc_arr_destroy(_GCArray* array, gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use an array-specific macro instead.
 *
 * Returns address of element with position 'pos' inside the array.
 *
 * RETURN VALUE:
 *   ON SUCCESS: adress of element with position 'pos',
 *   ON FAILURE: NULL.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void* _gc_arr_at(const _GCArray* array, size_t pos, gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use an array-specific macro instead.
 *
 * Assigns specified data 'data' to element with position 'pos'.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' or 'data' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void _gc_arr_set(_GCArray* array, const void* data, size_t pos,
        gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use an array-specific macro instead.
 *
 * Inserts new element containing the specified data 'data' at position 'pos'.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the array.
 * This causes all elements right of(including) 'pos' to shift rightward.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' or 'data' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds.
 *   4. GC_ERR_ARRAY_NO_CAP - 'array' doesn't have enough capacity to insert
 *   new elements. */

void _gc_arr_insert(_GCArray* array, const void* data, size_t pos,
        gc_status* out_status);

/* INTERNAL FUNCTION - use an array-specific macro instead.
 *
 * Appends new element with data 'data' to the end of the array.
 * This is done by copying data pointed to by 'data' to the appropriate location
 * in the array.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' or 'data' is NULL,
 *   3. GC_ERR_ARRAY_NO_CAP - 'array' doesn't have enough capacity to append
 *   new elements. */

void _gc_arr_push_back(_GCArray* array, const void* data, gc_status* out_status);

/* ------------------------------------------------------ */

/* Removes element with position 'pos' from the array. This causes all elements
 * right of 'pos' to shift leftward.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void gc_arr_remove_at(_GCArray* array, size_t pos, gc_status* out_status);

/* Removes the last element inside the array.
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   3. GC_ERR_ARRAY_EMPTY - 'array' is already empty. */

void gc_arr_pop_back(_GCArray* array, gc_status* out_status);

/* ------------------------------------------------------ */

/* Attempts to re-allocate array's 'data' field so that it can store 'capacity'
 * elements. 'capacity' argument must be greater than or equal to current's
 * array capacity. If the re-allocation fails, an error code will be provided
 * and the array's internal state will remain unchanged.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL OR 'capacity' < current array cap,
 *   3. GC_ERR_REALLOC_FAIL - realloc() returned NULL. */

void gc_arr_reserve(_GCArray* array, size_t capacity, gc_status* out_status);

/* Shrinks the allocated memory for the array's data field to match its current
 * size, minimizing unused space. This helps optimize memory usage by freeing
 * excess capacity that is no longer needed.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL. */

void gc_arr_shrink(_GCArray* array, gc_status* out_status);

/* ------------------------------------------------------ */

/* Gets array's size. Assumes that 'array' is a pointer to a valid
 * array. */

#define gc_arr_size(array)->_size

/* ------------------------------------------------------ */

/* Gets array's capacity. Assumes that 'array' is a pointer to a valid
 * array. */

#define gc_arr_capacity(array) (array)->_capacity

/* -------------------------------------------------------------------------- */

/* Below are "value array" and "pointer array" specific macros. Use these 
 * instead of internal functions/macros defined above
 * (they are prefixed by '_') */

/* ------------------------------------------------------ */

/* Value array - stores copies of values inside the array.
 * 'type' argument inside some of the macros below refers to the value type
 * stored inside the array - if the array stores ints, it should be 'int'. */

typedef struct _GCArray GCVArray;

#define gc_arr_init_val(valarr, init_cap, type, out_status) \
    _gc_array_init((valarr), (init_cap), sizeof((type)), (out_status))

/* Returns pointer to the element inside the array and casts the pointer to
 * type*. If _gc_arr_at() fails, the result may be NULL. */
#define gc_arr_at_val(valarr, pos, out_status, type) \
    (type *)_gc_array_at((valarr), (pos), (out_status))

#define gc_arr_set_val(valarr, data, pos, out_status) \
    _gc_array_set((valarr), (data), (pos), (out_status))

#define gc_arr_insert_val(valarr, data, pos, out_status) \
    _gc_array_insert((valarr), (data), (pos), (out_status))

#define gc_arr_push_back_val(valarr, data, out_status) \
    _gc_array_push_back((valarr), (data), (out_status))

/* -------------------------------------------------------------------------- */

/* Pointer array - stores pointers inside the array.
 * 
 * GCPArray provides convienent functions to help work with pointer arrays.
 * This is important because functions like _gc_arr_push_back() expect
 * parameter 'data' to be pointer to actual data held inside the array.
 * Since the array holds pointers, this would require 'data' to be a double pointer.
 *
 * These macros allow the user not to worry about double pointers and just provide
 * single pointers instead.
 *
 * 'type' argument inside some of the macros below refers to the value type
 * stored inside the array - if the array stores int*, it should be 'int*'. */

typedef struct _GCArray GCPArray;

#define gc_arr_init_ptr(ptrarr, init_cap, out_status) \
    _gc_array_init((ptrarr), (init_cap), sizeof(void*), (out_status))

/* Returns the address of the pointer stored in the array, performing an
 * appropriate cast. Since the array stores pointers, the returned address
 * is a double pointer. */
#define gc_arr_at_ptr(ptrarr, pos, out_status, type) \
    (type *)_gc_array_at((ptrarr), (pos), (out_status))

#define gc_arr_set_ptr(ptrarr, data, pos, out_status) \
    _gc_array_set((ptrarr), &(data), (pos), (out_status))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_arr_insert_ptr(ptrarr, data, pos, out_status) \
    _gc_array_insert((ptrarr), &(data), (pos), (out_status))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_arr_push_back_ptr(ptrarr, data, out_status) \
    _gc_array_push_back((ptrarr), &(data), (out_status))

#endif // _GC_ARRAY_H_
