#include "ds/gc_string.h"

#include <stdlib.h>
#include <string.h>

#include "_gc_shared.h"
#include "ds/gc_vector.h"

#define _CAPACITY_FACTOR 1.75

#define _STR_END(str) (str->_data + str->_len)

typedef struct GCStringViewMutable
{
    const char* _data;
    size_t _len; // excluding \0
} GCStringViewMutable;

/* -------------------------------------------------------------------------- */

#define _EXPAND_SUCCESS 0
#define _EXPAND_ALLOC_FAIL 1

static const GCString _STR_EMPTY = {0};

static int _expand_string(GCString* str, size_t mem_needed)
{
    size_t new_capacity = mem_needed * _CAPACITY_FACTOR;

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

/* -------------------------------------------------------------------------- */

GCString gc_str(const char* content, gc_status* out_status)
{
    if(content == NULL)
    {
        GC_RETURN(_STR_EMPTY, out_status, GC_SUCCESS);
    }

    gc_status _status;
    size_t len = strlen(content);
    GCString str = gc_str_(content, len, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(str, out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_RETURN(_STR_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_RETURN(_STR_EMPTY, out_status, GC_ERR_UNHANDLED);
    }
}
GCString gc_str_(const char* content, size_t content_len,
        gc_status* out_status)
{
    if((content == NULL) || (content_len == 0))
    {
        GC_RETURN(_STR_EMPTY, out_status, GC_SUCCESS);
    }

    void* str_data = malloc(content_len);
    if(str_data == NULL)
    {
        GC_RETURN(_STR_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
    }
    memcpy(str_data, content, content_len);

    GCString str = {
        ._data = str_data,
        ._len = content_len,
        ._capacity = content_len
    };

    GC_RETURN(str, out_status, GC_SUCCESS);
}

GCString gc_str_fv(const GCStringView* view,
        gc_status* out_status)
{
    if(view == NULL)
    {
        GC_RETURN(_STR_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }

    char* data = malloc(view->_len);

    if(data == NULL)
    {
        GC_RETURN(_STR_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
    }
    else
    {
        GCString str = {
            ._data = data,
            ._len = view->_len,
            ._capacity = view->_len
        };
        GC_RETURN(str, out_status, GC_SUCCESS);
    }
}

GCString gc_str_empty()
{
    return _STR_EMPTY;
}

/* ------------------------------------------------------ */

static const GCStringView _STR_VIEW_EMPTY = {0};

GCStringView gc_str_str(const GCStringView* str, gc_status* out_status)
{
    if(str == NULL)
    {
        GC_RETURN(_STR_VIEW_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }
    else
    {
        GCStringView view = {
            ._data = str->_data,
            ._len = str->_len
        };
        GC_RETURN(view, out_status, GC_ERR_INVALID_ARG);
    }
}

GCStringView gc_str_substr(const GCStringView* str, size_t start_pos,
        ssize_t end_pos, gc_status* out_status)
{
    if(end_pos == GC_STR_SUBSTR_STR_END)
        end_pos = str->_len;

    if((str == NULL) || (start_pos > end_pos) || (start_pos >= str->_len) ||
            (end_pos > str->_len))
    {
        GC_RETURN(_STR_VIEW_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }

    GCStringView view = {
        ._data = str->_data + start_pos,
        ._len = end_pos - start_pos
    };
    
    GC_RETURN(view, out_status, GC_ERR_INVALID_ARG);
}

void gc_str_cat(GCString* str1, const GCStringView* str2,
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
            case _EXPAND_SUCCESS:
                break;
            case _EXPAND_ALLOC_FAIL:
                GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
            default:
                GC_VRETURN(out_status, GC_ERR_UNHANDLED);
        }
    }

    memmove(_STR_END(str1), str2->_data, str2->_len);

    str1->_len += str2->_len;

    GC_VRETURN(out_status, GC_SUCCESS);
}

/* ------------------------------------------------------ */

void gc_str_cpy(GCString* dest, const GCStringView* src, gc_status* out_status)
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
            case _EXPAND_SUCCESS:
                break;
            case _EXPAND_ALLOC_FAIL:
                GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
            default:
                GC_VRETURN(out_status, GC_ERR_UNHANDLED);
        }
    }

    memmove(dest->_data, src->_data, src->_len);

    dest->_len = src->_len;
}

/* ------------------------------------------------------ */

static const struct GCStringFindObject _STR_FIND_OBJ_EMPTY = {
    .str_pos = GC_STR_FIND_NOT_FOUND,
    .needle_idx = GC_STR_FIND_NOT_FOUND
};

struct GCStringFindObject _str_find(const GCStringView* haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive)
{
    const char* it_str;
    gc_str_diff it_diff;

    ssize_t i, j;
    for(i = 0; i < haystack->_len; i++)
    {
        it_str = haystack->_data + i;
        for(j = 0; j < needle_count; j++)
        {
            if(i + needles[j]._len > haystack->_len) continue;

            it_diff = _str_cmp(it_str, needles[j]._len,
                    needles[j]._data, needles[j]._len,
                    case_sensitive);

            if(it_diff == GC_STR_DIFF_EQUAL)
            {
                struct GCStringFindObject ret = {
                    .str_pos = i,
                    .needle_idx = j
                };
                return ret;
            }
        }
    }

    return _STR_FIND_OBJ_EMPTY;
}

struct GCStringFindObject gc_str_find(const GCStringView* haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    if((haystack == NULL) || (needle_count == 0))
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack->_len == 0)
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_SUCCESS);
    }

    struct GCStringFindObject find_object =
        _str_find(haystack, needles, needle_count, case_sensitive);

    GC_RETURN(find_object, out_status, GC_SUCCESS);
}

struct GCStringFindObject gc_str_rfind(const GCStringView* haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    if((haystack == NULL) || (needle_count == 0))
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack->_len == 0)
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_SUCCESS);
    }

    ssize_t i, j;

    const char* it_str;
    gc_str_diff it_diff;

    for(i = haystack->_len - 1; i >= 0; i--)
    {
        for(j = 0; j < needle_count; j++)
        {
            it_str = ((i + needles[j]._len) <= haystack->_len) ?
                (haystack->_data + i) : NULL;

            if(it_str == NULL) continue;

            it_diff = _str_cmp(it_str, needles[j]._len,
                    needles[j]._data, needles[j]._len,
                    case_sensitive);

            if(it_diff == GC_STR_DIFF_EQUAL)
            {
                struct GCStringFindObject ret = {
                    .str_pos = i,
                    .needle_idx = j
                };
                GC_RETURN(ret, out_status, GC_SUCCESS);
            }
        }
    }

    GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_SUCCESS);

}

static const struct GCStringFindAllObject _STR_FIND_ALL_OBJ_EMPTY = {0};

struct GCStringFindAllObject gc_str_find_all(const GCStringView* haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    gc_status _status;

    if((haystack == NULL) || (needle_count == 0))
    {
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack->_len == 0)
    {
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_SUCCESS);
    }

    GCVVector vec;
    gc_vec_init_val(&vec, 10, struct GCStringFindObject, &_status);
    if(_status == GC_ERR_ALLOC_FAIL)
    {
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
    }

    size_t offset = 0;
    while(true)
    {
        GCStringView it_sv = {
            ._data = haystack->_data + offset,
            ._len = haystack->_len - offset
        };

        struct GCStringFindObject find_res = _str_find((GCStringView*)&it_sv,
                needles, needle_count, case_sensitive);

        if(find_res.str_pos == GC_STR_FIND_NOT_FOUND) break;

        gc_vec_push_back_val(&vec, &find_res, &_status);
        if(_status == GC_ERR_ALLOC_FAIL)
        {
            gc_vec_destroy(&vec, &_status);
            GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
        }

        offset += find_res.str_pos;
    }

    if(gc_vec_size(&vec) == 0)
    {
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_SUCCESS);
    }
    else
    {
        struct GCStringFindAllObject ret = {
            .find_objects = _gc_vec_data(&vec),
            .count = gc_vec_size(&vec)
        };

        GC_RETURN(ret, out_status, GC_SUCCESS);
    }
}


/* ------------------------------------------------------ */

static const struct GCStringSepObject _STRING_SEP_OBJ_EMPTY = {0};

struct GCStringSepObject gc_str_sep(const GCStringView* str,
        GCStringView sep[], size_t sep_count,
        bool case_sensitive, gc_status* out_status)
{
    if((str == NULL) || (sep_count == 0))
    {
        GC_RETURN(_STRING_SEP_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }

    ssize_t i;
    gc_status _status;

    struct GCStringFindAllObject find_all_res = 
        gc_str_find_all(str, sep, sep_count, case_sensitive, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            break;
        case GC_ERR_ALLOC_FAIL:
            GC_RETURN(_STRING_SEP_OBJ_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_RETURN(_STRING_SEP_OBJ_EMPTY, out_status, GC_ERR_UNHANDLED);
    }

    if(find_all_res.count == 0)
    {
        GCStringView* ret_sv = (GCStringView*)malloc(sizeof(GCStringView));
        ret_sv->_data = str->_data;
        ret_sv->_len = str->_len;

        struct GCStringSepObject ret = {
            .views = ret_sv,
            .count = 1
        };

        GC_RETURN(ret, out_status, GC_SUCCESS);
    }

    GCStringView* svs = (struct GCStringView*)
        malloc(find_all_res.count * sizeof(struct GCStringView));
    if(svs == NULL)
    {
        GC_RETURN(_STRING_SEP_OBJ_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
    }

    for(i = 0; i < find_all_res.count + 1; i++)
    {
        if(i == 0)
        {
            svs[0] = (GCStringView) {
                ._data = str->_data,
                ._len = find_all_res.find_objects[0].str_pos
            };
        }
        else if(i == find_all_res.count)
        {
            size_t final_seg_start_offset =
                find_all_res.find_objects[i - 1].str_pos +
                sep[find_all_res.find_objects[i - 1].needle_idx]._len;

            svs[find_all_res.count] = (GCStringView) {
                ._data = str->_data + final_seg_start_offset,
                ._len = str->_len - final_seg_start_offset
            };
        }
        else
        {
            size_t curr_seg_start_offset = 
                find_all_res.find_objects[i - 1].str_pos +
                sep[find_all_res.find_objects[i - 1].needle_idx]._len;

            size_t curr_seg_len =
                find_all_res.find_objects[i].str_pos -
                curr_seg_start_offset;

            svs[find_all_res.count] = (GCStringView) {
                ._data = str->_data + curr_seg_start_offset,
                    ._len = curr_seg_len
            };
        }
    }

    struct GCStringSepObject ret = {
        .count = find_all_res.count + 1,
        .views = svs
    };

    GC_RETURN(ret, out_status, GC_SUCCESS);
}

/* ------------------------------------------------------ */

gc_str_diff gc_str_cmp(const GCStringView* str1, const GCStringView* str2,
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
