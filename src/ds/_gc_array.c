#include "_gc_shared.h"
#include "ds/gc_array.h"
#include <string.h>
#include "ds/_gc_array.h"

void __gc_arr_init(struct __GCArray* array, size_t cap,
        size_t el_size, gc_status* out_status)
{
    array->_data = malloc(cap * el_size);
    if(array->_data == NULL) 
    {
        GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);   
    }

    array->_size = 0;
    array->_el_size = el_size;
    array->_capacity = cap;

    GC_VRETURN(out_status, GC_SUCCESS);
}

void __gc_arr_destroy(struct __GCArray* array)
{
    if(array->_data != NULL)
    {
        free(array->_data);
        array->_data = NULL;
    }

    array->_capacity = 0;
    array->_size = 0;
    array->_el_size = 0;

}

void __gc_arr_insert(struct __GCArray* array, size_t pos, const void* data_arr,
        size_t data_size)
{
    if(data_size == 0) return;

    void* start_pos = __gc_arr_at(array, pos);

    size_t bytes_inserted = data_size * array->_el_size;

    size_t elements_shifted = array->_size - pos;
    if(elements_shifted > 0)
    {
        size_t bytes_shifted = elements_shifted * array->_el_size;
        memmove(start_pos + bytes_inserted, start_pos, bytes_shifted);
    }

    memcpy(start_pos, data_arr, bytes_inserted);

    array->_size += data_size;
}

void __gc_arr_remove(struct __GCArray* array, size_t start_pos,
        size_t end_pos)
{
    void* _start_pos = __gc_arr_at(array, start_pos);
    void* _end_pos = __gc_arr_at(array, end_pos);

    size_t elements_shifted = array->_size - end_pos;

    if(elements_shifted > 0)
        memmove(_start_pos, _end_pos, elements_shifted * array->_el_size);

    array->_size -= (end_pos - start_pos);
}
