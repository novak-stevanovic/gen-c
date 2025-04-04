#include "ds/gc_array.h"
#include "ds/_gc_array.h"
#include "_gc_shared.h"
#include <string.h>

void _gc_array_init(_GCArray* array, size_t cap,
        size_t el_size, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(cap == 0) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(el_size == 0) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }

    array->_data = malloc(cap * el_size);
    if(array->_data == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_MALLOC_FAIL);   
    }

    array->_size = 0;
    array->_el_size = el_size;
    array->_capacity = cap;

    GC_VRETURN(out_status, GC_SUCCESS);
}

void _gc_array_destroy(_GCArray* array, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }

    if(array->_data != NULL)
    {
        free(array->_data);
        array->_data = NULL;
    }
    array->_capacity = 0;
    array->_size = 0;
    array->_el_size = 0;

    GC_VRETURN(out_status, GC_SUCCESS);
}

void* _gc_array_at(const _GCArray* array, size_t pos, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_RETURN(NULL, out_status, GC_ERR_INVALID_ARG);   
    }
    if(pos >= array->_size) 
    {
        GC_RETURN(NULL, out_status, GC_ERR_OUT_OF_BOUNDS);   
    }

    void* addr = __gc_array_at(array, pos);
    GC_RETURN(addr, out_status, GC_SUCCESS);
}

void _gc_array_set(_GCArray* array, const void* data, size_t pos,
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

    void* addr = __gc_array_at(array, pos);

    memcpy(addr, data, array->_el_size);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void _gc_array_insert(_GCArray* array, const void* data, size_t pos,
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

    __gc_array_insert_at(array, pos, data, 1);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void _gc_array_push_back(_GCArray* array, const void* data, gc_status* out_status)
{
    _gc_array_insert(array, data, array->_size, out_status);

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

void _gc_array_remove_at(_GCArray* array, size_t pos, gc_status* out_status)
{
    if(array == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);   
    }
    if(pos >= array->_size) 
    {
        GC_VRETURN(out_status, GC_ERR_OUT_OF_BOUNDS);   
    }
    
    __gc_array_remove_at(array, pos, pos + 1);

    GC_VRETURN(out_status, GC_SUCCESS);
}

void _gc_array_pop_back(_GCArray* array, gc_status* out_status)
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

void _gc_array_reserve(_GCArray* array, size_t capacity, gc_status* out_status)
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
            GC_VRETURN(out_status, GC_ERR_REALLOC_FAIL);
        }
        else
        {
            array->_data = new_data;
            array->_capacity = capacity;
            GC_VRETURN(out_status, GC_SUCCESS);
        }
    }
}

void _gc_array_shrink_to_fit(_GCArray* array, gc_status* out_status)
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
