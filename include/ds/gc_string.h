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
    size_t _len; // excluding \0
} GCStringView;

/* ------------------------------------------------------ */

GCString gc_str(const char* content, gc_status* out_status);
GCString gc_str_(const char* content, size_t content_len,
        gc_status* out_status);

GCString gc_str_fv(const GCStringView* view,
        gc_status* out_status);

GCString gc_str_empty();

/* ------------------------------------------------------ */

#define gc_str_to_sv(str_ptr) ((GCStringView) {                                \
        ._data = (str_ptr)->_data,                                               \
        ._len = (str_ptr)->_len                                                  \
        })                                                                     \

#define GC_STR_SUBSTR_STR_END -1
GCStringView gc_str_substr(const GCStringView* str, size_t start_pos,
        ssize_t end_pos, gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cat(GCString* str1, const GCStringView* str2,
        gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cpy(GCString* dest, const GCStringView* src, gc_status* out_status);

/* ------------------------------------------------------ */

#define GC_STR_FIND_NOT_FOUND -1
#define GC_STR_FIND_START_POS_DEFAULT -1

struct GCStringFindObject
{
    ssize_t str_pos;
    ssize_t needle_idx;
};

struct GCStringFindObject gc_str_find(const GCStringView* haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

struct GCStringFindObject gc_str_rfind(const GCStringView* haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

struct GCStringFindAllObject
{
    struct GCStringFindObject* find_objects;
    size_t count;
};

struct GCStringFindAllObject gc_str_find_all(const GCStringView* haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

struct GCStringSepObject
{
    GCStringView* views;
    size_t count;
};

struct GCStringSepObject gc_str_sep(const GCStringView* str,
        GCStringView sep[], size_t sep_count,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

typedef int gc_str_diff;

#define GC_STR_DIFF_STR1_LONGER 300
#define GC_STR_DIFF_STR2_LONGER -300
#define GC_STR_DIFF_EQUAL 0

gc_str_diff gc_str_cmp(const GCStringView* str1, const GCStringView* str2,
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
