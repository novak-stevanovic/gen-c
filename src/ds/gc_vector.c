#include "ds/gc_vector.h"

void _gc_vector_init(_GCVector* vector, size_t init_capacity,
        size_t el_size, gc_err* out_err);

void _gc_vector_destroy(_GCVector* vector, gc_err* out_err);

void* _gc_vector_at(const _GCVector* vector, size_t pos, gc_err* out_err);

void _gc_vector_set(_GCVector* vector, const void* data, size_t pos,
        gc_err* out_err);

void _gc_vector_insert_at(_GCVector* vector, const void* data, size_t pos,
        gc_err* out_err);

void _gc_vector_push_back(_GCVector* vector, const void* data, gc_err* out_err);

void _gc_vector_remove_at(_GCVector* vector, size_t pos, gc_err* out_err);

void _gc_vector_pop_back(_GCVector* vector, gc_err* out_err);

void _gc_vector_realloc(_GCVector* vector, size_t capacity, gc_err* out_err);

void _gc_vector_shrink_to_fit(_GCVector* vector, gc_err* out_err);
