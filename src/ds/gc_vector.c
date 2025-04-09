#include "ds/gc_vector.h"
#include "_gc_shared.h"
#include "ds/_gc_vector.h"
#include "ds/gc_array.h"

#define EXPAND_FACTOR 2.0

_GCVector _gc_vec_create(size_t capacity, size_t el_size, gc_status* out_status)
{
    if((capacity == 0) || (el_size == 0))
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
    }

    _GCVector vec = (_GCVector)malloc(sizeof(struct __GCVector));
    if(vec == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
    }

    gc_status _status;
    __gc_vec_init(vec, capacity, el_size, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(vec, out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            free(vec);
            GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
        default:
            free(vec);
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

void gc_vec_destroy(_GCVector vector, gc_status* out_status)
{
    if(vector == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    __gc_vec_destroy(vector);
}

void* _gc_vec_at(const _GCVector vector, size_t pos, gc_status* out_status)
{
    gc_status _status;
    void* addr = _gc_arr_at((const _GCArray)vector, pos, &_status);

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

void _gc_vec_set(_GCVector vector, const void* data, size_t pos,
        gc_status* out_status)
{
    gc_status _status;
    _gc_arr_set((_GCArray)vector, data, pos, &_status);

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

static void __vec_expand(_GCVector vector, gc_status* out_status)
{
    _GCArray _vector = (_GCArray)vector;

    gc_status _status;
    gc_arr_reserve(_vector, gc_arr_capacity(_vector) * EXPAND_FACTOR, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
    
}

void _gc_vec_insert(_GCVector vector, const void* data, size_t pos,
        gc_status* out_status)
{
    _GCArray _vector = (_GCArray)vector;

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

    __vec_expand(vector, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            break;
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
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

void _gc_vec_push_back(_GCVector vector, const void* data, gc_status* out_status)
{
    if(vector == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(data == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    _GCArray _vector = (_GCArray)vector;

    gc_status _status;
    _gc_vec_insert(vector, data, gc_arr_size(_vector), &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void gc_vec_remove(_GCVector vector, size_t pos, gc_status* out_status)
{
    gc_status _status;
    gc_arr_remove((_GCArray)vector, pos, &_status);

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

void gc_vec_pop_back(_GCVector vector, gc_status* out_status)
{
    gc_status _status;
    gc_arr_pop_back((_GCArray)vector, &_status); 

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

void gc_vec_reserve(_GCVector vector, size_t capacity, gc_status* out_status)
{
    gc_status _status;
    gc_arr_reserve((_GCArray)vector, capacity, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);
        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
        case GC_ERR_ALLOC_FAIL:
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void gc_vec_fit(_GCVector vector, gc_status* out_status)
{
    gc_status _status;
    gc_arr_fit((_GCArray)vector, &_status);

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

size_t gc_vec_size(const _GCVector vector)
{
    return (vector != NULL) ? gc_arr_size((_GCArray)vector) : 0;
}

size_t gc_vec_capacity(const _GCVector vector)
{
    return (vector != NULL) ? gc_arr_capacity((_GCArray)vector) : 0;
}

void* _gc_vec_data(const _GCVector vector)
{
    return (vector != NULL) ? _gc_arr_data((_GCArray)vector) : NULL;
}
