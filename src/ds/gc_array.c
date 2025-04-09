#include "ds/gc_array.h"
#include "ds/_gc_array.h"
#include "_gc_shared.h"
#include <string.h>

_GCArray _gc_arr_create(size_t capacity, size_t el_size, gc_status* out_status)
{
    if((capacity == 0) || (el_size == 0))
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);
    }

    _GCArray arr = (_GCArray)malloc(sizeof(struct __GCArray));

    if(arr == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
    }

    gc_status _status;
    __gc_arr_init(arr, capacity, el_size, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(arr, out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            free(arr);
            GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
        default:
            free(arr);
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

void gc_arr_destroy(_GCArray array, gc_status* out_status)
{
    if(array == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    __gc_arr_destroy(array);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void* _gc_arr_at(const _GCArray array, size_t pos, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);   
    }
    if(pos >= array->_size) 
    {
        GC_RETURN(NULL, out_status, GC_ERR_OUT_OF_BOUNDS);   
    }

    void* addr = __gc_arr_at(array, pos);
    GC_RETURN(addr, out_status, GC_SUCCESS);
}

void _gc_arr_set(_GCArray array, const void* data, size_t pos,
        gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(data == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(pos >= array->_size) 
    {
        GC_VRETURN(out_status, GC_ERR_OUT_OF_BOUNDS);   
    }

    void* addr = __gc_arr_at(array, pos);

    memcpy(addr, data, array->_el_size);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void _gc_arr_insert(_GCArray array, const void* data, size_t pos,
        gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(pos > array->_size) 
    {
        GC_VRETURN(out_status, GC_ERR_OUT_OF_BOUNDS);   
    }
    if(data == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }

    if(array->_size >= array->_capacity) 
    {
        GC_VRETURN(out_status, GC_ERR_ARRAY_NO_CAP);   
    }

    __gc_arr_insert(array, pos, data, 1);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void _gc_arr_push_back(_GCArray array, const void* data, gc_status* out_status)
{
    _gc_arr_insert(array, data, array->_size, out_status);

    if(out_status == NULL) return;

    switch(*out_status)
    {
        case GC_SUCCESS:
            GC_VRETURN(out_status, GC_SUCCESS);

        case GC_ERR_INVALID_ARG:
            GC_VRETURN(out_status, GC_ERR_INVALID_ARG);

        case GC_ERR_ARRAY_NO_CAP:
            GC_VRETURN(out_status, GC_ERR_ARRAY_NO_CAP);

        default:
            GC_VRETURN(out_status, GC_ERR_UNHANDLED);
    }
}

void gc_arr_remove(_GCArray array, size_t pos, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(pos >= array->_size) 
    {
        GC_VRETURN(out_status, GC_ERR_OUT_OF_BOUNDS);   
    }
    
    __gc_arr_remove(array, pos, pos + 1);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void gc_arr_pop_back(_GCArray array, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(array->_size == 0)
    {
        GC_VRETURN(out_status, GC_ERR_ARRAY_EMPTY);
    }

    array->_size--;
}

void gc_arr_reserve(_GCArray array, size_t capacity, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(capacity < array->_capacity)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    else if(capacity == array->_capacity) 
    {
        GC_VRETURN(out_status, GC_SUCCESS);   
    }
    else
    {
        void* new_data = realloc(array->_data, capacity * array->_el_size);
        if(new_data == NULL)
        {
            GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
        }
        else
        {
            array->_data = new_data;
            array->_capacity = capacity;
            GC_VRETURN(out_status, GC_SUCCESS);
        }
    }
}

void gc_arr_fit(_GCArray array, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }

    if(array->_size == array->_capacity) 
    {
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    array->_data = realloc(array->_data, array->_size * array->_el_size);

    array->_capacity = array->_size;
}

size_t gc_arr_size(const _GCArray array)
{
    return (array != NULL) ? array->_size : 0;
}

size_t gc_arr_capacity(const _GCArray array)
{
    return (array != NULL) ? array->_capacity : 0;
}

void* _gc_arr_data(const _GCArray array)
{
    return (array != NULL) ? array->_data : NULL;
}
