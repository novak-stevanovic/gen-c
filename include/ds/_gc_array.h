#ifndef __GC_ARRAY_H__
#define __GC_ARRAY_H__

#include "gc_shared.h"
#include <stddef.h>

struct __GCArray
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
};

/* The following functions and macros do not check for errors. They perform
 * under specific assumptions. These assumptions should be checked for before
 * using the listed functions/macros. */

/* Assumptions:
 * 1. 'array' is a pointer to a valid struct __GCArray
 * 2. 'cap' > 0,
 * 3. 'el_size' > 0.
 * Initializes the provided struct __GCArray. ERRORS: GC_ERR_ALLOC_FAIL */
void __gc_arr_init(struct __GCArray* array, size_t cap,
        size_t el_size, gc_status* out_status);

/* Assumptions:
 * 1. 'array' is a pointer to a valid struct __GCArray.
 * Destroys the provided struct __GCArray. ERRORS: GC_ERR_INVALID_ARG */
void __gc_arr_destroy(struct __GCArray* array);

/* Assumptions:
 * 1. 'array' is a valid _GCArray,
 * 2. 'pos' is a valid position inside the array. */
#define __gc_arr_at(array, pos) ((array)->_data + ((pos) * (array)->_el_size))

/* Assumptions:
 * 1. 'array' is a valid _GCArray,
 * 2. 'pos' is a valid position inside the array,
 * 3. 'data_arr' is a valid array of data, 
 * 4. each element in 'data_arr' is the size of array->_el_size, 
 * 5. 'array' has enough capacity to store additional 'data_size' elements. */
void __gc_arr_insert(struct __GCArray* array, size_t pos, const void* data_arr,
        size_t data_size);

/* 'start_pos' is included, 'end_pos' is not included.
 * Assumptions:
 * 1. 'array' is a valid _GCArray,,
 * 2. 'start_pos' < array->_size,
 * 3. 'end_pos' <= array->_size,
 * 4. 'array' has size greater than 0.
 * 5. 'end_pos' >= 'start_pos' */
void __gc_arr_remove(struct __GCArray* array, size_t start_pos,
        size_t end_pos);

#endif // __GC_ARRAY_H__
