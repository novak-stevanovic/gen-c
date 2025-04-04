#ifndef _GC_STRING_H_
#define _GC_STRING_H_

#include "gc_shared.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct GCString
{
    char* _data;
    size_t _len; // excluding \0
    size_t _capacity; // excluding \0
} GCString;

typedef struct GCStringView
{
    const char* _data;
    const size_t _len; // excluding \0
} GCStringView;

/* Compatible type with GCString and GCStringView -> Casting allowed. */
typedef struct GCStringLike GCStringLike;

typedef int gc_str_diff;

/* ------------------------------------------------------ */

GCString gc_str_create(const char* content, size_t content_len,
        gc_status* out_status);

// GCString gc_str_create_noalloc(char* content, size_t content_len,
//         gc_status* out_status);

GCString gc_str_create_from_view(const GCStringView* view,
        gc_status* out_status);

GCString gc_str_create_empty();

GCStringView gc_str_view_create(const GCStringLike* str, size_t offset,
        gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cat(GCString* str1, const GCStringLike* str2,
        gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cpy(GCString* dest, const GCStringLike* src);

/* ------------------------------------------------------ */

size_t gc_str_find(const GCStringLike* haystack,
        const GCStringLike* needle);

/* ------------------------------------------------------ */

gc_str_diff gc_str_cmp(const GCStringLike* str1, const GCStringLike* str2,
        bool case_sensitive);

/* ------------------------------------------------------ */

void gc_str_reserve(GCString* str, size_t capacity, gc_status* out_status);

void gc_str_shrink(GCString* str, gc_status* out_status);

/* ------------------------------------------------------ */

#endif // _GC_STRING_H_
