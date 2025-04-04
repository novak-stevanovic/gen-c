#include "ds/gc_string.h"

#include <stdlib.h>

#include "_gc_shared.h"

#define CAPACITY_FACTOR 1.75

#define GET_STR(data, len, cap)                                                \
    (GCString) {                                                               \
        ._data = data,                                                         \
        ._len = len,                                                           \
        ._capacity = cap                                                       \
    }                                                                          \

#define GET_EMPTY_STR() GET_STR(NULL, 0, 0)

#define GET_STR_VIEW(data, len)                                                \
    (GCStringView) {                                                           \
        ._data = data,                                                         \
        ._len = len                                                            \
    }                                                                          \

#define GET_EMPTY_STR_VIEW() GET_STR_VIEW(NULL, 0)

struct GCStringLike 
{
    const char* _data;
    const size_t _len; // excluding \0
};

/* -------------------------------------------------------------------------- */

GCString gc_str_create(const char* content, size_t content_len,
        gc_status* out_status)
{
    if((content == NULL) || (content_len == 0))
    {
        GC_RETURN(gc_str_create_empty(), out_status, GC_SUCCESS);
    }

    char* data = malloc(content_len + 1);

    if(data == NULL)
    {
        GC_RETURN((GET_EMPTY_STR()), out_status, GC_ERR_MALLOC_FAIL);
    }
    else
    {
        GC_RETURN((GET_STR(data, content_len, content_len)),
                out_status, GC_SUCCESS);
    }
}

GCString gc_str_create_from_view(const GCStringView* view,
        gc_status* out_status)
{
    if(view == NULL)
    {
        GC_RETURN((GET_EMPTY_STR()), out_status, GC_ERR_INVALID_ARG);
    }

    char* data = malloc(view->_len + 1);

    if(data == NULL)
    {
        GC_RETURN((GET_EMPTY_STR()), out_status, GC_ERR_MALLOC_FAIL);
    }
    else
    {
        GC_RETURN((GET_STR(data, view->_len, view->_len)),
                out_status, GC_SUCCESS);
    }
}

GCString gc_str_create_empty()
{
    return GET_EMPTY_STR();
}

GCStringView gc_str_view_create(const GCStringLike* str, size_t offset,
        gc_status* out_status)
{
    if(str == NULL)
    {
        GC_RETURN((GET_EMPTY_STR_VIEW()), out_status, GC_ERR_INVALID_ARG);
    }
    if(offset >= (str->_len - 1))
    {
        GC_RETURN((GET_EMPTY_STR_VIEW()), out_status, GC_ERR_INVALID_ARG);
    }

    GCStringView view = GET_STR_VIEW((str->_data + offset),
            (str->_len - offset));

    GC_RETURN(view, out_status, GC_SUCCESS);
}

void gc_str_cat(GCString* str1, const GCStringLike* str2,
        gc_status* out_status)
{
    if((str1 == NULL) || (str2 == NULL))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    size_t total_mem_needed = str1->_len + str2->_len + 1;
    size_t available_mem = str1->_capacity + 1;
    if(total_mem_needed > available_mem)
    {

    }
}

/* ------------------------------------------------------ */

void gc_str_set(GCString* dest, const GCStringLike* src);

/* ------------------------------------------------------ */

size_t gc_str_find(const GCStringLike* haystack,
        const GCStringLike* needle);

/* ------------------------------------------------------ */

gc_str_diff gc_str_cmp(const GCStringLike* str1, const GCStringLike* str2,
        bool case_sensitive);

/* ------------------------------------------------------ */
