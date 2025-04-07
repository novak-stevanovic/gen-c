#ifndef _GC_STRING_H_
#define _GC_STRING_H_

#include "gc_shared.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct _GCString
{
    char* data;
    size_t len;
    size_t capacity;
} _GCString;

typedef _GCString* GCString;

/* GC Static String - Statically allocated 'data' */
typedef struct _GCString_
{
    char* data;
    size_t len;
} _GCString_;

typedef _GCString_* GCString_;

typedef struct GCStringView
{
    const char* data;
    size_t len;
} GCStringView;

/* ------------------------------------------------------ */

GCString gc_str(const char* content, gc_status* out_status);
GCString gc_strl(const char* content, size_t len, gc_status* out_status);

#define gc_str_(str)                                                           \
    &((_GCString_) {                                                           \
        .data = str,                                                           \
        .len = strlen(str)                                                     \
    })                                                                         \

#define gc_strl_(str, len)                                                     \
    &((_GCString_) {                                                           \
        .data = str,                                                           \
        .len = len                                                             \
    })                                                                         \

void gc_str_destroy(GCString str, gc_status* out_status);

/* ------------------------------------------------------ */

#define gc_sv(str) ((GCStringView) {                                           \
        .data = (str)->data,                                                   \
        .len = (str)->len                                                      \
    })                                                                         \

#define GC_STR_SUBSTR_STR_END -1
GCStringView gc_str_substr(GCStringView str, size_t start_pos,
        ssize_t end_pos, gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cat(GCString str1, GCStringView str2,
        gc_status* out_status);

/* ------------------------------------------------------ */

void gc_str_cpy(GCString dest, GCStringView src, gc_status* out_status);

/* ------------------------------------------------------ */

typedef int gc_str_diff;

#define GC_STR_DIFF_STR1_LONGER 300
#define GC_STR_DIFF_STR2_LONGER -300
#define GC_STR_DIFF_EQUAL 0

gc_str_diff gc_str_cmp(GCStringView str1, GCStringView str2,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

#define GC_STR_FIND_NOT_FOUND -1

struct GCStringFindObject
{
    ssize_t str_pos;
    ssize_t needle_idx;
};

struct GCStringFindObject gc_str_find(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

struct GCStringFindObject gc_str_rfind(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

struct GCStringFindAllObject
{
    struct GCStringFindObject* find_objects;
    size_t count;
};

struct GCStringFindAllObject gc_str_find_all(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

void gc_str_find_all_obj_destroy(struct GCStringFindAllObject* find_all_obj);

/* ------------------------------------------------------ */

struct GCStringSepObject
{
    GCStringView* views;
    size_t count;
};

struct GCStringSepObject gc_str_sep(GCStringView str,
        GCStringView sep[], size_t sep_count,
        bool case_sensitive, gc_status* out_status);

void gc_str_sep_obj_destroy(struct GCStringSepObject* sep_obj);

/* ------------------------------------------------------ */

void gc_str_reserve(GCString str, size_t capacity, gc_status* out_status);

void gc_str_fit(GCString str, gc_status* out_status);

/* ------------------------------------------------------ */

/* If not letter, the same char is returned. */
#define gc_str_get_lowercase_letter(c) \
    ( ((c >= 'A' && c <= 'Z')) ? c + ('a' - 'A') : c )

#define gc_str_get_uppercase_letter(c) \
    ( ((c >= 'a' && c <= 'z')) ? c - ('a' - 'A') : c )

void gc_str_to_upper(GCString str, gc_status* out_status);

void gc_str_to_lower(GCString str, gc_status* out_status);

/* ------------------------------------------------------ */

#endif // _GC_STRING_H_
