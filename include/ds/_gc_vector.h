#ifndef __GC_VECTOR_H__
#define __GC_VECTOR_H__

#include "ds/_gc_array.h"

struct __GCVector
{
    struct __GCArray _base;
};

/* The following functions and macros do not check for errors. They perform
 * under specific assumptions. These assumptions should be checked for before
 * using the listed functions/macros. */

/* Assumptions:
 * 1. 'vector' is a pointer to a valid struct __GCVector
 * 2. 'cap' > 0,
 * 3. 'el_size' > 0.
 * Initializes the provided struct __GCVector. ERRORS: GC_ERR_ALLOC_FAIL */
void __gc_vec_init(struct __GCVector* vector, size_t cap,
        size_t el_size, gc_status* out_status);

/* Assumptions:
 * 1. 'vector' is a pointer to a valid struct __GCVector.
 * Destroys the provided struct __GCVector. ERRORS: GC_ERR_INVALID_ARG */
void __gc_vec_destroy(struct __GCVector* vector);

#endif // __GC_VECTOR_H__
