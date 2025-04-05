#ifndef _GC_STRING_H_
#define _GC_STRING_H_

#include "gc_shared.h"

#include <stdlib.h>
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

/* ------------------------------------------------------ */

GCString gc_str_create(const char* content, size_t content_len,
        gc_status* out_status);

GCString gc_str_create_noalloc(char* content, size_t content_len,
        gc_status* out_status);

GCString gc_str_create_from_view(const GCStringView* view,
        gc_status* out_status);

GCString gc_str_create_empty();

#define GC_STR_VIEW_LEN_MAX -1
GCStringView gc_str_view_create(const GCStringLike* str, size_t offset,
        ssize_t len, gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cat(GCString* str1, const GCStringLike* str2,
        gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cpy(GCString* dest, const GCStringLike* src, gc_status* out_status);

/* ------------------------------------------------------ */

#define GC_STR_FIND_NOT_FOUND -1
#define GC_STR_FIND_START_POS_DEFAULT -1

struct GCStringFindResult
{
    ssize_t str_pos;
    ssize_t needle_idx;
};

struct GCStringFindResult gc_str_find(const GCStringLike* haystack,
        size_t haystack_start_pos, 
        GCStringLike* needles, size_t needle_count,
        bool case_sensitive, gc_status* out_status);

struct GCStringFindResult gc_str_rfind(const GCStringLike* haystack,
        size_t haystack_start_pos, 
        GCStringLike* needles, size_t needle_count,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

struct GCStringSeparateResult
{
    GCStringView* views;
    size_t count;
};

struct GCStringSeparateResult gc_str_sep(const GCStringLike* str,
        size_t start_pos, GCStringLike* separators, size_t separator_count,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

typedef int gc_str_diff;

#define GC_STR_DIFF_STR1_LONGER 300
#define GC_STR_DIFF_STR2_LONGER -300
#define GC_STR_DIFF_EQUAL 0

gc_str_diff gc_str_cmp(const GCStringLike* str1, const GCStringLike* str2,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_reserve(GCString* str, size_t capacity, gc_status* out_status);

void gc_str_fit(GCString* str, gc_status* out_status);

/* ------------------------------------------------------ */

/* If not letter, the same char is returned. */
#define gc_str_get_lowercase_letter(c) \
    ( ((c >= 'A' && c <= 'Z')) ? c + ('a' - 'A') : c )

#define gc_str_get_uppercase_letter(c) \
    ( ((c >= 'a' && c <= 'z')) ? c - ('a' - 'A') : c )

/* ------------------------------------------------------ */

#endif // _GC_STRING_H_
