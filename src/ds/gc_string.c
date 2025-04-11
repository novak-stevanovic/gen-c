#include "ds/gc_string.h"

#include <stdio.h>
#include <stdlib.h>

#include "_gc_shared.h"
#include "ds/gc_vector.h"

#define _CAPACITY_FACTOR 1.75

#define _STR_END(str) (str->data + str->len)

struct _GCString
{
    char* data;
    size_t len;
    size_t capacity;
};

static void _expand_string(GCString str, size_t new_capacity,
        gc_status* out_status);

/* CREATE/DESTROY ---------------------------------------------------------- */

/* GCString --------------------------------------------- */

GCString gc_str_create(const char* content, gc_status* out_status)
{
    gc_status _status;
    size_t len = (content != NULL) ? strlen(content) : 0;

    GCString str = gc_str_create_(content, len, &_status);

    switch(_status)
    {
        case GC_SUCCESS:
            GC_RETURN(str, out_status, GC_SUCCESS);
        case GC_ERR_ALLOC_FAIL:
            GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
        default:
            GC_RETURN(NULL, out_status, GC_ERR_UNHANDLED);
    }
}

GCString gc_str_create_(const char* content, size_t len,
        gc_status* out_status)
{
    GCString str = (GCString)malloc(sizeof(struct _GCString));
    if(str == NULL)
    {
        GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
    }

    void* str_data = malloc(len + 1);
    if(str_data == NULL)
    {
        free(str);
        GC_RETURN(NULL, out_status, GC_ERR_ALLOC_FAIL);
    }

    if(content != NULL)
        memcpy(str_data, content, len);

    str->data = str_data;
    str->len = len;
    str->capacity = len;

    str->data[len] = '\0';

    GC_RETURN(str, out_status, GC_SUCCESS);
}

GCString gc_str_create_from_sv(GCStringView sv, gc_status* out_status)
{
    return gc_str_create_(sv._data, sv._len, out_status);
}

void gc_str_destroy(GCString str, gc_status* out_status)
{
    if(str == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    if(str->data != NULL) free(str->data);
    str->capacity = 0;
    str->len = 0;
    free(str);

    GC_VRETURN(out_status, GC_SUCCESS);
}

/* GCStringView ----------------------------------------- */

GCStringView gc_str_sv(GCString str)
{
    return (GCStringView) {
        ._data = (const char*)str->data,
        ._len = str->len
    };

}

GCStringView gc_strst_sv(GCStringSt str)
{
    return (GCStringView) {
        ._data = str.data,
        ._len = strlen(str.data)
    };
}

/* GCStringSt ------------------------------------------- */

GCStringSt gc_strst(char* text)
{
    return (GCStringSt) {
        .data = text,
        .len = strlen(text)
    };
}

GCStringSt gc_strst_(char* text, size_t len)
{
    return (GCStringSt) {
        .data = text,
        .len = len
    };
}

/* GETTERS ----------------------------------------------------------------- */ 

// GCString --------------------------------------------- */

char* gc_str_data(GCString str)
{
    return (str != NULL) ? str->data : NULL;
}

size_t gc_str_len(GCString str)
{
    return (str != NULL) ? str->len : 0;
}

size_t gc_str_capacity(GCString str)
{
    return (str != NULL) ? str->capacity : 0;
}

/* GCStringView ----------------------------------------- */

const char* gc_sv_data(GCStringView sv)
{
    return sv._data;
}

size_t gc_sv_len(GCStringView sv)
{
    return sv._len;
}

/* GCStringSt ------------------------------------------- */

char* gc_strst_data(GCStringSt str)
{
    return str.data;
}

size_t gc_strst_len(GCStringSt str)
{
    return str.len;
}

/* MAIN API ----------------------------------------------------------------- */

static const GCStringView _STR_VIEW_EMPTY = {0};

GCStringView gc_str_substr(GCStringView str, size_t start_pos,
        ssize_t end_pos, gc_status* out_status)
{
    if(end_pos == GC_STR_SUBSTR_STR_END)
        end_pos = str._len;

    if((start_pos > end_pos) || (start_pos >= str._len) ||
            (end_pos > str._len))
    {
        GC_RETURN(_STR_VIEW_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }

    GCStringView view = {
        ._data = str._data + start_pos,
        ._len = end_pos - start_pos
    };
    
    GC_RETURN(view, out_status, GC_SUCCESS);
}

/* -------------------------------------------------------------------------- */

void gc_str_cat(GCString str1, GCStringView str2, gc_status* out_status)
{
    if(str1 == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(str2._len == 0)
    {
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    /* ------------------------------------------------------ */

    // Allocate more memory if needed

    size_t total_len_needed = str1->len + str2._len;
    size_t available_len = str1->capacity;

    if(total_len_needed > available_len)
    {
        gc_status _status;

        _expand_string(str1, total_len_needed * _CAPACITY_FACTOR, &_status);

        switch(_status)
        {
            case GC_SUCCESS:
                break;
            case GC_ERR_ALLOC_FAIL:
                GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
            default:
                GC_VRETURN(out_status, GC_ERR_UNHANDLED);
        }
    }

    /* ------------------------------------------------------ */

    // Move the actual contents of str2 to the end of str1

    memmove(_STR_END(str1), str2._data, str2._len);

    str1->len += str2._len;

    str1->data[str1->len] = '\0';

    GC_VRETURN(out_status, GC_SUCCESS);
}

/* -------------------------------------------------------------------------- */

void gc_str_cpy(GCString dest, GCStringView src, gc_status* out_status)
{
    if(dest == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(src._len == 0)
    {
        dest->len = 0;
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    /* ------------------------------------------------------ */

    // Allocate more memory if needed

    size_t dest_mem_needed = src._len;

    if(dest_mem_needed >= dest->capacity)
    {
        gc_status _status;
        _expand_string(dest, dest_mem_needed, &_status);

        switch(_status)
        {
            case GC_SUCCESS:
                break;
            case GC_ERR_ALLOC_FAIL:
                GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
            default:
                GC_VRETURN(out_status, GC_ERR_UNHANDLED);
        }
    }

    /* ------------------------------------------------------ */

    // Move contents of src string to the beginning of dest string

    memmove(dest->data, src._data, src._len);

    dest->len = src._len;

    dest->data[dest->len] = '\0';

    GC_VRETURN(out_status, GC_SUCCESS);
}

/* -------------------------------------------------------------------------- */

static gc_str_diff _str_cmp(const char* str1, size_t str1_len,
        const char* str2, size_t str2_len, bool case_sensitive)
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
            c1 = (!case_sensitive ? gc_str_lowerc(str1[i]) : str1[i]);
            c2 = (!case_sensitive ? gc_str_lowerc(str2[i]) : str2[i]);
            if(c1 != c2)
                return (c2 - c1);
        }
        return GC_STR_DIFF_EQUAL;
    }
}


gc_str_diff gc_str_cmp(GCStringView str1, GCStringView str2,
        bool case_sensitive)
{
    return _str_cmp(str1._data, str1._len,
            str2._data, str2._len,
            case_sensitive);
}

/* -------------------------------------------------------------------------- */

static const struct GCStringFindObject _STR_FIND_OBJ_EMPTY = {
    .str_pos = GC_STR_FIND_NOT_FOUND,
    .needle_idx = GC_STR_FIND_NOT_FOUND
};

struct GCStringFindObject _str_find(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive)
{
    const char* it_str;
    gc_str_diff it_diff;

    ssize_t i, j;
    for(i = 0; i < haystack._len; i++)
    {
        it_str = haystack._data + i;
        for(j = 0; j < needle_count; j++) // loop through the needles
        {
            // If the needle has no length, it is not valid
            if(needles[j]._len == 0) continue;

            // Out of bounds - can't be found
            if(i + needles[j]._len > haystack._len) continue;

            // Compare the strings, naively
            it_diff = _str_cmp(it_str, needles[j]._len,
                    needles[j]._data, needles[j]._len,
                    case_sensitive);

            // Found - return
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

    // Return empty object
    return _STR_FIND_OBJ_EMPTY;
}

struct GCStringFindObject gc_str_find(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    if(needle_count == 0)
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack._len == 0)
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_SUCCESS);
    }

    struct GCStringFindObject find_object =
        _str_find(haystack, needles, needle_count, case_sensitive);

    GC_RETURN(find_object, out_status, GC_SUCCESS);
}

static struct GCStringFindObject _str_rfind(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive)
{
    ssize_t i, j;

    const char* it_str;
    gc_str_diff it_diff;

    for(i = haystack._len - 1; i >= 0; i--)
    {
        for(j = 0; j < needle_count; j++)
        {
            // If the needle has no length, it is not valid
            if(needles[j]._len == 0) continue;

            // Determine start of it_str - if the condition is not met,
            // it is out of bounds for haystack
            it_str = ((i + needles[j]._len) <= haystack._len) ?
                (haystack._data + i) : NULL;

            if(it_str == NULL) continue;

            // Compare the strings, naively
            it_diff = _str_cmp(it_str, needles[j]._len,
                    needles[j]._data, needles[j]._len,
                    case_sensitive);

            // Found - return
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

struct GCStringFindObject gc_str_rfind(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    if(needle_count == 0)
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack._len == 0)
    {
        GC_RETURN(_STR_FIND_OBJ_EMPTY, out_status, GC_SUCCESS);
    }

    struct GCStringFindObject ret =
        _str_rfind(haystack, needles, needle_count, case_sensitive);

    GC_RETURN(ret, out_status, GC_SUCCESS);
}

void gc_str_find_obj_destroy(struct GCStringFindObject* find_obj)
{
    find_obj->needle_idx = GC_STR_FIND_NOT_FOUND;
    find_obj->str_pos = GC_STR_FIND_NOT_FOUND;
}

static const struct GCStringFindAllObject _STR_FIND_ALL_OBJ_EMPTY = {0};

struct GCStringFindAllObject gc_str_find_all(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status)
{
    gc_status _status;

    if(needle_count == 0)
    {
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }
    if(haystack._len == 0)
    {
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_SUCCESS);
    }

    // We don't know how many matches we're going to get - we use a vector

    GCVVector vec = gc_vec_create_val(10, struct GCStringFindObject, &_status);
    if(_status == GC_ERR_ALLOC_FAIL)
    {
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
    }

    size_t offset = 0;
    while(true)
    {
        // Current part of string that we're examining
        // At the 0th iteration, it is the whole string
        GCStringView it_sv = {
            ._data = haystack._data + offset,
            ._len = haystack._len - offset
        };

        // Find result inside the current part of the string
        struct GCStringFindObject find_res = _str_find(it_sv,
                needles, needle_count, case_sensitive);

        // When we stop finding matches, we break out of the while loop
        if(find_res.str_pos == GC_STR_FIND_NOT_FOUND) break;

        /* Because the _str_find() function returns position based on
        * the current "start" of the string, we have to adjust the
        * find result so that it starts the indexing at the beginning of
        * the 'haystack' str */

        find_res.str_pos += offset;

        // Add the result to the vector
        gc_vec_push_back_val(vec, &find_res, &_status);
        if(_status == GC_ERR_ALLOC_FAIL) // handle vector realloc failure
        {
            gc_vec_destroy(vec, &_status);
            GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
        }

        // Set the offset - this will move the current part of the string we're
        // examining just after the current match
        offset = find_res.str_pos + 1;
    }

    if(gc_vec_size(vec) == 0)
    {
        gc_vec_destroy(vec, &_status);
        GC_RETURN(_STR_FIND_ALL_OBJ_EMPTY, out_status, GC_SUCCESS);
    }
    else
    {
        gc_vec_fit(vec, &_status);

        struct GCStringFindAllObject ret = {
            .find_objects = _gc_vec_data(vec),
            .count = gc_vec_size(vec),
            .__vec = vec
        };

        GC_RETURN(ret, out_status, GC_SUCCESS);
    }
}

void gc_str_find_all_obj_destroy(struct GCStringFindAllObject* find_all_obj)
{
    if(find_all_obj == NULL) return;

    find_all_obj->find_objects = NULL;

    find_all_obj->count = 0;

    if(find_all_obj->__vec != NULL)
        gc_vec_destroy((_GCVector)find_all_obj->__vec, NULL);
}

/* -------------------------------------------------------------------------- */

static const struct GCStringSepObject _STRING_SEP_OBJ_EMPTY = {0};

struct GCStringSepObject gc_str_sep(GCStringView str,
        GCStringView sep[], size_t sep_count,
        bool case_sensitive, gc_status* out_status)
{
    if(sep_count == 0)
    {
        GC_RETURN(_STRING_SEP_OBJ_EMPTY, out_status, GC_ERR_INVALID_ARG);
    }

    ssize_t i;
    gc_status _status;

    // Find all occurrences

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

    // Allocate return object

    GCStringView* svs = (struct GCStringView*)
        malloc((find_all_res.count + 1) * sizeof(struct GCStringView));
    if(svs == NULL)
    {
        GC_RETURN(_STRING_SEP_OBJ_EMPTY, out_status, GC_ERR_ALLOC_FAIL);
    }

    struct GCStringSepObject ret = {
        .count = find_all_res.count + 1,
        .views = svs
    };

    // Handle special case

    if(find_all_res.count == 0)
    {
        svs[0] = str;
        GC_RETURN(ret, out_status, GC_SUCCESS);
    }

    // Handle non-special cases

    for(i = 0; i < find_all_res.count + 1; i++)
    {
        if(i == 0)
        {
            svs[0] = (GCStringView) {
                ._data = str._data,
                ._len = find_all_res.find_objects[0].str_pos
            };
        }
        else if(i == find_all_res.count)
        {
            size_t final_seg_start_offset =
                find_all_res.find_objects[i - 1].str_pos +
                sep[find_all_res.find_objects[i - 1].needle_idx]._len;

            svs[find_all_res.count] = (GCStringView) {
                ._data = str._data + final_seg_start_offset,
                ._len = str._len - final_seg_start_offset
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

            svs[i] = (GCStringView) {
                ._data = str._data + curr_seg_start_offset,
                    ._len = curr_seg_len
            };
        }
    }

    // Destroy the GCStringFindAllObject
    gc_str_find_all_obj_destroy(&find_all_res);

    GC_RETURN(ret, out_status, GC_SUCCESS);
}

void gc_str_sep_obj_destroy(struct GCStringSepObject* sep_obj)
{
    if(sep_obj == NULL) return;

    if(sep_obj->views != NULL)
        free(sep_obj->views);

    sep_obj->count = 0;
}

/* -------------------------------------------------------------------------- */

void gc_str_reserve(GCString str, size_t capacity, gc_status* out_status)
{
    if((str == NULL) || (capacity < str->len))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(capacity == str->len)
    {
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    void* new_data = realloc(str->data, capacity + 1);

    if(new_data == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
    }
    else
    {
        str->data = new_data;
        str->capacity = capacity;
        GC_VRETURN(out_status, GC_SUCCESS);
    }
}

/* ------------------------------------------------------ */

void gc_str_fit(GCString str, gc_status* out_status)
{
    if(str == NULL)
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }

    gc_status _status;
    gc_str_reserve(str, str->len, &_status);

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

/* -------------------------------------------------------------------------- */

void gc_str_to_upper(GCString str)
{
    if(str == NULL) return;

    size_t i;
    for(i = 0; i < str->len; i++)
        str->data[i] = gc_str_upperc(str->data[i]);
}

void gc_str_to_lower(GCString str)
{
    if(str == NULL) return;

    size_t i;
    for(i = 0; i < str->len; i++)
        str->data[i] = gc_str_lowerc(str->data[i]);
}

/* -------------------------------------------------------------------------- */

static void _expand_string(GCString str, size_t new_capacity,
        gc_status* out_status)
{
    if((str == NULL) || (str->len > new_capacity))
    {
        GC_VRETURN(out_status, GC_ERR_INVALID_ARG);
    }
    if(str->len == new_capacity)
    {
        GC_VRETURN(out_status, GC_SUCCESS);
    }

    void* new_data = realloc(str->data, new_capacity + 1);

    if(new_data != NULL)
    {
        str->data = new_data;
        str->capacity = new_capacity;
        GC_VRETURN(out_status, GC_SUCCESS);
    }
    else
    {
        GC_VRETURN(out_status, GC_ERR_ALLOC_FAIL);
    }
}
