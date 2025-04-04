#include "ds/gc_array.h"
#include <string.h>
#include "ds/_gc_array.h"

void __gc_arr_insert(_GCArray* array, size_t pos, const void* data_arr,
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

void __gc_arr_remove(_GCArray* array, size_t start_pos,
        size_t end_pos)
{
    void* _start_pos = __gc_arr_at(array, start_pos);
    void* _end_pos = __gc_arr_at(array, end_pos);

    size_t elements_shifted = array->_size - end_pos;

    if(elements_shifted > 0)
        memmove(_start_pos, _end_pos, elements_shifted * array->_el_size);

    array->_size -= (end_pos - start_pos);
}
