#ifndef __GC_ARRAY_H__
#define __GC_ARRAY_H__

#include <stddef.h>

typedef struct _GCArray _GCArray;

/* The following functions and macros do not check for errors. They perform
 * under specific assumptions. These assumptions should be checked for before
 * using the listed functions/macros. */

/* Assumptions:
 * 1. 'array' is a valid _GCArray,
 * 2. 'pos' is a valid position inside the array. */
#define __gc_array_at(array, pos) ((array)->_data + ((pos) * (array)->_el_size))

/* Assumptions:
 * 1. 'array' is a valid _GCArray,
 * 2. 'pos' is a valid position inside the array,
 * 3. 'data_arr' is a valid array of data, 
 * 4. each element in 'data_arr' is the size of array->_el_size, 
 * 5. 'array' has enough capacity to store additional 'data_size' elements. */
void __gc_array_insert_at(_GCArray* array, size_t pos, const void* data_arr,
        size_t data_size);

/* 'start_pos' is included, 'end_pos' is not included.
 * Assumptions:
 * 1. 'array' is a valid _GCArray,,
 * 2. 'start_pos' < array->_size,
 * 3. 'end_pos' <= array->_size,
 * 4. 'array' has size greater than 0.
 * 5. 'end_pos' >= 'start_pos' */
void __gc_array_remove_at(_GCArray* array, size_t start_pos,
        size_t end_pos);

#endif // __GC_ARRAY_H__
