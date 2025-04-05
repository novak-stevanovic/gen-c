#include "ds/gc_string.h"

#include <stdlib.h>
#include <string.h>

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

#define STR_END(str) (str->_data + str->_len)

struct GCStringLike 
{
    const char* _data;
    const size_t _len; // excluding \0
};

/* -------------------------------------------------------------------------- */

#define EXPAND_SUCCESS 0
#define EXPAND_ALLOC_FAIL 1

static int _expand_string(GCString* str, size_t mem_needed)
{
    size_t new_capacity = mem_needed * CAPACITY_FACTOR;

    void* new_data;
    if(str->_data != NULL) // empty str
        new_data = realloc(str->_data, new_capacity);
    else
        new_data = malloc(new_capacity);

    if(new_data == NULL)
    {
        return 1;
    }
    else
    {
        str->_data = new_data;
        str->_capacity = new_capacity;
        return 0;
    }
}

static gc_str_diff _str_cmp(const char* str1, size_t str1_len,
        const char* str2, size_t str2_len,
        bool case_sensitive)
{
    if(str1_len > str2_len)
    {
        return GC_STR_DIFF_STR1_LONGER;
    }
    else if(str2_len > str1_len)
    {
        return GC_STR_DIFF_STR2_LONGER;
    }
    else
    {
        size_t i;
        char c1, c2;
        for(i = 0; i < str1_len; i++)
        {
            c1 = (!case_sensitive ? gc_str_get_lowercase_letter(str1[i]) : str1[i]);
            c2 = (!case_sensitive ? gc_str_get_lowercase_letter(str2[i]) : str2[i]);
            if(c1 != c2)
                return (c2 - c1);
        }
        return GC_STR_DIFF_EQUAL;
    }
}

GCString gc_str_create(const char* content, size_t content_len,
        gc_status* out_status)
{
    if((content == NULL) || (content_len == 0))
    {
        GC_RETURN(gc_str_create_empty(), out_status, GC_SUCCESS);
    }

    char* data = malloc(content_len);

    if(data == NULL)
    {
        GC_RETURN((GET_EMPTY_STR()), out_status, GC_ERR_ALLOC_FAIL);
    }
    else
    {
        GC_RETURN((GET_STR(data, content_len, content_len)),
                out_status, GC_SUCCESS);
    }
}

GCString gc_str_create_noalloc(char* content, size_t content_len,
        gc_status* out_status)
{
    if((content == NULL) || (content_len == 0))
    {
        GC_RETURN(gc_str_create_empty(), out_status, GC_SUCCESS);
    }

    GC_RETURN((GET_STR(content, content_len, content_len)),
            out_status, GC_SUCCESS);
}

GCString gc_str_create_from_view(const GCStringView* view,
        gc_status* out_status)
{
    if(view == NULL)
    {
        GC_RETURN((GET_EMPTY_STR()), out_status, GC_ERR_INVALID_ARG);
    }

    char* data = malloc(view->_len);

    if(data == NULL)
    {
        GC_RETURN((GET_EMPTY_STR()), out_status, GC_ERR_ALLOC_FAIL);
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
        ssize_t len, gc_status* out_status)
{
    if(str == NULL)
    {
        GC_RETURN((GET_EMPTY_STR_VIEW()), out_status, GC_ERR_INVALID_ARG);
    }
    if(offset >= (str->_len))
    {
        GC_RETURN((GET_EMPTY_STR_VIEW()), out_status, GC_ERR_INVALID_ARG);
    }
    if(len == GC_STR_VIEW_LEN_MAX)
        len = str->_len - offset;

    if(len >= (str->_len - offset))
    {
        GC_RETURN((GET_EMPTY_STR_VIEW()), out_status, GC_ERR_INVALID_ARG);
    }

    GCStringView view = GET_STR_VIEW((str->_data + offset), len);

    GC_RETURN(view, out_status, GC_SUCCESS);
}

void gc_str_cat(GCString* str1, const GCStringLike* str2,
        gc_status* out_status)
{
    if((str1 == NULL) || (str2 == NULL))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    size_t total_mem_needed = str1->_len + str2->_len;
    size_t available_mem = str1->_capacity;
    if(total_mem_needed > available_mem)
    {
        int expand_status = _expand_string(str1, total_mem_needed);

        switch(expand_status)
        {
            case EXPAND_SUCCESS:
                break;
            case EXPAND_ALLOC_FAIL:
                GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
            default:
                GC_VRETURN(out_status, GC_ERR_UNHANDLED);
        }
    }

    memmove(STR_END(str1), str2->_data, str2->_len);

    str1->_len += str2->_len;
}

/* ------------------------------------------------------ */

void gc_str_cpy(GCString* dest, const GCStringLike* src, gc_status* out_status)
{
    if((dest == NULL) || (src == NULL))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    size_t dest_mem_needed = src->_len;

    if(dest_mem_needed > dest->_len)
    {
        int expand_status = _expand_string(dest, dest_mem_needed);

        switch(expand_status)
        {
            case EXPAND_SUCCESS:
                break;
            case EXPAND_ALLOC_FAIL:
                GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
            default:
                GC_VRETURN(out_status, GC_ERR_UNHANDLED);
        }
    }

    memmove(dest->_data, src->_data, src->_len);

    dest->_len = src->_len;
}

/* ------------------------------------------------------ */

#define _GET_STR_FIND_RES(pos, idx) (struct GCStringFindResult) {              \
    .str_pos = pos,                                                            \
    .needle_idx = idx                                                          \
}                                                                              \

#define _GET_STR_FIND_RES_EMPTY()                                              \
    _GET_STR_FIND_RES(GC_STR_FIND_NOT_FOUND, GC_STR_FIND_NOT_FOUND)            \

struct GCStringFindResult gc_str_find(const GCStringLike* haystack, size_t haystack_start_pos, 
        GCStringLike* needles, size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    if((haystack == NULL) || (needle_count == 0) || (needles == NULL))
    {
        GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack->_len == 0)
    {
        GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_ERR_INVALID_ARG);
    }

    if(haystack_start_pos == GC_STR_FIND_START_POS_DEFAULT)
        haystack_start_pos = 0;

    if(haystack_start_pos >= haystack->_len)
    {
        GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_ERR_INVALID_ARG);
    }

    ssize_t i, j;
    const char* it_str;
    gc_str_diff it_diff;

    for(i = haystack_start_pos; i < haystack->_len; i++)
    {
        it_str = haystack->_data + i;
        for(j = 0; j < needle_count; j++)
        {
            if(i + needles[j]._len >= haystack->_len) continue;

            it_diff = _str_cmp(it_str, needles[j]._len,
                    needles[j]._data, needles[j]._len,
                    case_sensitive);

            if(it_diff == GC_STR_DIFF_EQUAL)
            {
                GC_RETURN((_GET_STR_FIND_RES(i, j)), out_status, GC_ERR_INVALID_ARG);
            }
        }
    }

    GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_SUCCESS);
}

struct GCStringFindResult gc_str_rfind(const GCStringLike* haystack, size_t haystack_start_pos, 
        GCStringLike* needles, size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    if((haystack == NULL) || (needle_count == 0) || (needles == NULL))
    {
        GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack->_len == 0)
    {
        GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_ERR_INVALID_ARG);
    }

    if(haystack_start_pos == GC_STR_FIND_START_POS_DEFAULT)
        haystack_start_pos = haystack->_len - 1;

    if(haystack_start_pos >= haystack->_len)
    {
        GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_ERR_INVALID_ARG);
    }

    ssize_t i, j;
    const char* it_str;
    gc_str_diff it_diff;

    for(i = haystack_start_pos; i >= 0; i--)
    {
        for(j = 0; j < needle_count; j++)
        {
            it_str = (needles[j]._len <= i) ?
                haystack->_data + i - needles[j]._len + 1 :
                NULL;

            if(it_str == NULL) continue;

            it_diff = _str_cmp(it_str, needles[j]._len,
                    needles[j]._data, needles[j]._len,
                    case_sensitive);

            if(it_diff == GC_STR_DIFF_EQUAL)
            {
                GC_RETURN((_GET_STR_FIND_RES(i, j)), out_status, GC_SUCCESS);
            }
        }
    }

    GC_RETURN((_GET_STR_FIND_RES_EMPTY()), out_status, GC_SUCCESS);
}

/* ------------------------------------------------------ */

gc_str_diff gc_str_cmp(const GCStringLike* str1, const GCStringLike* str2,
        bool case_sensitive, gc_status* out_status)
{
    if((str1 == NULL) || (str2 == NULL))
    {
        GC_RETURN(1, out_status, GC_ERR_INVALID_ARG);
    }

    return _str_cmp(str1->_data, str1->_len,
            str2->_data, str2->_len,
            case_sensitive);
}

/* ------------------------------------------------------ */

void gc_str_reserve(GCString* str, size_t capacity, gc_status* out_status)
{
    if((str == NULL) || (capacity < str->_len))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(capacity == str->_len)
    {
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    void* new_data;
    if(str->_data == NULL) // empty str
        new_data = malloc(capacity);
    else
    {
        new_data = realloc(str->_data, capacity);
    }

    if(new_data == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
    }
    else
    {
        str->_data = new_data;
        str->_capacity = capacity;
        GC_VRETURN(out_status, GC_SUCCESS);
    }
}

void gc_str_fit(GCString* str, gc_status* out_status)
{
    if(str == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    if(str->_data == NULL) // empty str
    {
        str->_capacity = 0;
        str->_len = 0;
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    gc_status _status;
    gc_str_reserve(str, str->_len, &_status);

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
