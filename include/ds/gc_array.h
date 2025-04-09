#ifndef _GC_ARRAY_H_
#define _GC_ARRAY_H_

#include "gc_shared.h"
#include <stdlib.h>

typedef struct __GCArray* _GCArray;

/* Getters ---------------------------------------------- */

/* Gets array's size.
 * Assumes that 'array' is a pointer to a valid array. */

size_t gc_arr_size(const _GCArray array);

/* Gets array's capacity.
 * Assumes that 'array' is a pointer to a valid array. */

size_t gc_arr_capacity(const _GCArray array);

/* INTERNAL FUNCTION - use a convenience macro instead.
 *
 * Gets array's data field.
 * Assumes that 'array' is a pointer to a valid array. */

void* _gc_arr_data(const _GCArray array);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use a convenience macro instead.
 *
 * Dynamically allocates memory for the struct __GCArray. Initializes
 * the struct with provided values. Dynamically allocates enough memory
 * to store 'capacity' elements inside the array. Returns the opaque pointer.
 *
 * RETURN VALUES:
 *   ON SUCCESS: Address of dynamically allocated _GCArray,
 *   ON FAILURE: NULL.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_ALLOC_FAIL - Dynamic allocation failed,
 *   3. GC_ERR_INVALID_ARG - provided 'el_size' or 'capacity' is equal to 0. */

_GCArray _gc_arr_create(size_t capacity, size_t el_size, gc_status* out_status);

/* ------------------------------------------------------ */

/* Destroys the array. Sets its fields to default values. Frees the dynamically
 * allocated memory for array's data field.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL. */

void gc_arr_destroy(_GCArray array, gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use a convenience macro instead.
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

void* _gc_arr_at(const _GCArray array, size_t pos, gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use a convenience macro instead.
 *
 * Assigns specified data 'data' to element with position 'pos'.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' or 'data' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void _gc_arr_set(_GCArray array, const void* data, size_t pos,
        gc_status* out_status);

/* ------------------------------------------------------ */

/* INTERNAL FUNCTION - use a convenience macro instead.
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

void _gc_arr_insert(_GCArray array, const void* data, size_t pos,
        gc_status* out_status);

/* INTERNAL FUNCTION - use a convenience macro instead.
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

void _gc_arr_push_back(_GCArray array, const void* data, gc_status* out_status);

/* ------------------------------------------------------ */

/* Removes element with position 'pos' from the array. This causes all elements
 * right of 'pos' to shift leftward.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   3. GC_ERR_OUT_OF_BOUNDS - 'pos' is out of bounds. */

void gc_arr_remove(_GCArray array, size_t pos, gc_status* out_status);

/* Removes the last element inside the array.
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL,
 *   3. GC_ERR_ARRAY_EMPTY - 'array' is already empty. */

void gc_arr_pop_back(_GCArray array, gc_status* out_status);

/* ------------------------------------------------------ */

/* Attempts to re-allocate array's 'data' field so that it can store 'capacity'
 * elements. 'capacity' argument must be greater than or equal to current's
 * array capacity. If the re-allocation fails, an error code will be provided
 * and the array's internal state will remain unchanged.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL OR 'capacity' < current array cap,
 *   3. GC_ERR_ALLOC_FAIL - realloc() returned NULL. */

void gc_arr_reserve(_GCArray array, size_t capacity, gc_status* out_status);

/* Shrinks the allocated memory for the array's data field to match its current
 * size, minimizing unused space. This helps optimize memory usage by freeing
 * excess capacity that is no longer needed.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS - Function call was successful,
 *   2. GC_ERR_INVALID_ARG - 'array' is NULL. */

void gc_arr_fit(_GCArray array, gc_status* out_status);

/* CONVENIENCE MACROS ------------------------------------------------------- */

/* 'type' refers to the data type stored inside the array */
#define gc_arr_data(arr, type) (type *)_gc_arr_data(arr)

/* Below are "value array" and "pointer array" specific macros. Use these 
 * instead of internal functions/macros defined above
 * (they are prefixed by '_') */

/* ------------------------------------------------------ */

/* Value array - stores copies of values inside the array.
 * 'type' argument inside some of the macros below refers to the value type
 * stored inside the array - if the array stores ints, it should be 'int'. */

typedef _GCArray GCVArray;

#define gc_arr_create_val(init_cap, type, out_status) \
    _gc_array_create((init_cap), sizeof(type), (out_status))

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

typedef _GCArray GCPArray;

#define gc_arr_create_ptr(init_cap, out_status) \
    _gc_array_create((init_cap), sizeof(void*), (out_status))

/* Finds the address of element inside the array with position 'pos'.
 * This is a double pointer, because the element itself is a pointer.
 * It casts this double pointer to the appropriate type. Then, the
 * double pointer is dereferenced so that a single pointer is returned. */
#define gc_arr_at_ptr(ptrarr, pos, out_status, type) \
    *(type **)_gc_array_at((ptrarr), (pos), (out_status))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_arr_set_ptr(ptrarr, data, pos, out_status) \
    _gc_array_set((ptrarr), &(data), (pos), (out_status))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_arr_insert_ptr(ptrarr, data, pos, out_status) \
    _gc_array_insert((ptrarr), &(data), (pos), (out_status))

/* 'data' refers to single pointer holding the address of some data. */
#define gc_arr_push_back_ptr(ptrarr, data, out_status) \
    _gc_array_push_back((ptrarr), &(data), (out_status))

#endif // _GC_ARRAY_H_
