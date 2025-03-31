#include "ds/gc_array.h"
#include <string.h>
#include "ds/_gc_array.h"

void __gc_array_insert_at(_GCArray* array, size_t pos, const void* data_arr,
        size_t count)
{
    if(count == 0) return;

    void* start_pos = __gc_array_at(array, pos);

    size_t elements_shifted = array->_count - pos;
    if(elements_shifted > 0)
    {
        size_t bytes_shifted = elements_shifted * array->_el_size;
        memmove(start_pos, start_pos + bytes_shifted, bytes_shifted);
    }

    memcpy(start_pos, data_arr, count * array->_el_size);

    array->_count += count;
}

void __gc_array_remove_at(_GCArray* array, size_t start_pos,
        size_t end_pos)
{
    void* _start_pos = __gc_array_at(array, start_pos);
    void* _end_pos = __gc_array_at(array, end_pos);

    size_t elements_shifted = array->_count - end_pos;

    if(elements_shifted > 0)
        memmove(_start_pos, _end_pos, elements_shifted * array->_el_size);

    array->_count -= (end_pos - start_pos);
}
