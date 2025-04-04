#include "ds/gc_vector.h"
#include "_gc_shared.h"
#include "ds/gc_array.h"

#define EXPAND_FACTOR 2.0

void _gc_vector_init(_GCVector* vector, size_t init_capacity,
        size_t el_size, gc_status* out_status)
{
    gc_status _status;
    _gc_arr_init((_GCArray*)vector, init_capacity, el_size, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_MALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_MALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void _gc_vector_destroy(_GCVector* vector, gc_status* out_status)
{
    gc_status _status;
    gc_arr_destroy((_GCArray*)vector, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void* _gc_vector_at(const _GCVector* vector, size_t pos, gc_status* out_status)
{
    gc_status _status;
    void* addr = _gc_arr_at((const _GCArray*)vector, pos, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(addr, out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_OUT_OF_BOUNDS:
            GC_RETURN(NULL, out_status, GC_ERR_OUT_OF_BOUNDS);
        default:
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

void _gc_vector_set(_GCVector* vector, const void* data, size_t pos,
        gc_status* out_status)
{
    gc_status _status;
    _gc_arr_set((_GCArray*)vector, data, pos, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_OUT_OF_BOUNDS:
            GC_VRETURN(out_status, GC_ERR_OUT_OF_BOUNDS);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

static void __vector_expand(_GCVector* vector, gc_status* out_status)
{
    _GCArray* _vector = (_GCArray*)vector;

    gc_status _status;
    gc_arr_reserve(_vector, _vector->_capacity * EXPAND_FACTOR, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_REALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_REALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
    
}

void _gc_vector_insert_at(_GCVector* vector, const void* data, size_t pos,
        gc_status* out_status)
{
    _GCArray* _vector = (_GCArray*)vector;

    gc_status _status;
    _gc_arr_insert(_vector, data, pos, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_OUT_OF_BOUNDS:
            GC_VRETURN(out_status, GC_ERR_OUT_OF_BOUNDS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_ARRAY_NO_CAP:
            break;
    }

    __vector_expand(vector, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            break;
        case GC_ERR_REALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_REALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }

    _gc_arr_insert(_vector, data, pos, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void _gc_vector_push_back(_GCVector* vector, const void* data, gc_status* out_status)
{
    if(vector == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(data == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    _GCArray* _vector = (_GCArray*)vector;

    gc_status _status;
    _gc_vector_insert_at(vector, data, _vector->_size, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_REALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_REALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void _gc_vector_remove_at(_GCVector* vector, size_t pos, gc_status* out_status)
{
    gc_status _status;
    gc_arr_remove_at((_GCArray*)vector, pos, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_OUT_OF_BOUNDS:
            GC_VRETURN(out_status, GC_ERR_OUT_OF_BOUNDS);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void _gc_vector_pop_back(_GCVector* vector, gc_status* out_status)
{
    gc_status _status;
    gc_arr_pop_back((_GCArray*)vector, &_status); 

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_ARRAY_EMPTY:
            GC_VRETURN(out_status, GC_ERR_ARRAY_EMPTY);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void _gc_vector_reserve(_GCVector* vector, size_t capacity, gc_status* out_status)
{
    gc_status _status;
    gc_arr_reserve((_GCArray*)vector, capacity, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_REALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_REALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void _gc_vector_shrink_to_fit(_GCVector* vector, gc_status* out_status)
{
    gc_status _status;
    gc_arr_shrink((_GCArray*)vector, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}
