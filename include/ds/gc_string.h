#ifndef _GC_STRING_H_
#define _GC_STRING_H_

#include "gc_shared.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct _GCString* GCString;

typedef struct GCStringSt
{
    char* data;
    size_t len;
} GCStringSt;

typedef struct GCStringView
{
    const char* _data;
    size_t _len;
} GCStringView;

/* CREATE/DESTROY ---------------------------------------------------------- */

/* GCString --------------------------------------------- */

/* Default method to create GCStrings. Finds the length of 'content' with
 * strlen(). Performs a call to gc_str_create_().
 * An empty string may be created if 'content' == NULL.
 *
 * For RETURN VALUE and STATUS CODES, see gc_str_create_(). */

GCString gc_str_create(const char* content, gc_status* out_status);

/* ---------------------------------- */

/* Creates a GCString by copying the bytes from 'content' to a new, dynamically
 * allocated chunk of memory. The size of the chunk depends on 'len'.
 * An empty string may be created if 'content' == NULL or 'len' == 0.
 *
 *
 * RETURN VALUE:
 *   ON SUCCESS: Address of dynamically allocated opaque underlying struct;
 *   ON FAILURE: NULL.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_ALLOC_FAIL: Failure to allocate memory for internal stuct
 *   _GCString or for the char array.*/

GCString gc_str_create_(const char* content, size_t len, gc_status* out_status);

/* ---------------------------------- */

/* Creates GCString from an existing view. This is done by calling:
 * gc_str_create_(gc_sv_data(sv), gc_sv_len(sv), &_status);
 *
 * For RETURN VALUE and STATUS CODES, see gc_str_create_(). */

GCString gc_str_create_from_sv(GCStringView sv, gc_status* out_status);

/* ---------------------------------- */

/* Destroys a GCString. This frees all the dynamically allocated memory for
 * both the underlying struct and the char*.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'str' is NULL. */

void gc_str_destroy(GCString str, gc_status* out_status);

/* GCStringView ----------------------------------------- */

/* Creates a GCStringView out of a GCString. */
GCStringView gc_str_sv(GCString str);

/* ---------------------------------- */

/* Creates a GCStringView out of a GCStringSt */
GCStringView gc_strst_sv(GCStringSt str);

/* GCStringSt ------------------------------------------- */

/* Creates a GCStringSt with its data field pointing to 'text'. The length is
 * determined by strlen(). */
GCStringSt gc_strst(char* text);

/* Creates a GCStringSt with its data field pointing to 'text'. The length is
 * provided by 'len'. */
GCStringSt gc_strst_(char* text, size_t len);

/* GETTERS ----------------------------------------------------------------- */ 

// GCString --------------------------------------------- */

char* gc_str_data(GCString str);
size_t gc_str_len(GCString str);
size_t gc_str_capacity(GCString str);

/* GCStringView ----------------------------------------- */

const char* gc_sv_data(GCStringView sv);
size_t gc_sv_len(GCStringView sv);

/* GCStringSt ------------------------------------------- */

char* gc_strst_data(GCStringSt str);
size_t gc_strst_len(GCStringSt str);

/* MAIN API ----------------------------------------------------------------- */

/* The string functions below use GCStringView as one/more of the string 
 * parameters. To work with a GCString/GCStringSt, use:
 * gc_str_sv(GCString)/gc_strst_sv(GCStringSt);

 * GCString s1 = gc_str_create("a", NULL);
 * GCString s2 = gc_str_create("b", NULL);
 * gc_str_cat(s1, gc_str_sv(s2), NULL); */

/* -------------------------------------------------------------------------- */

/* Creates a "substring" out of provided GCStringView. The new substring will
 * in fact be a view into the specified part of the 'str'.
 *
 * NOTES:
 * - 'start_pos' is included in the substring.
 * - 'end_pos' is not included in the substring.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'str' is NULL, 'start_pos' >= string's length,
 *   'end_pos' > string's length. */

#define GC_STR_SUBSTR_STR_END -1

/* GC_STR_SUBSTR_STR_END can be used as 'end_pos' if the desired behavior is
 * for the substring to span from 'start_pos' to the string's end. */

GCStringView gc_str_substr(GCStringView str, size_t start_pos,
        ssize_t end_pos, gc_status* out_status);

/* -------------------------------------------------------------------------- */

 /* Appends str2 to str1 by copying str2’s data onto the end of str1.
 * If the new length exceeds str1’s capacity, the buffer is reallocated
 * with extra space to reduce future reallocations. Call gc_str_fit()
 * afterward to trim any unused capacity.
 *
 * This also appends \0 to the end of the newly-formed string. 
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'str1' is NULL;
 *   3. GC_ERR_ALLOC_FAIL: The string needed more memory and the allocation
 *   failed. */

void gc_str_cat(GCString str1, GCStringView str2, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* Replaces content of dest's data by the content of src's data.
 * 'dest' string's length will match the length of the source string after
 * the operation is finished.
 *
 * This also appends \0 to the end of the newly-formed string.
 *
 * Unlike in gc_str_cat, the string will allocate just enough memory to
 * fit the src's data.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'str1' is NULL;
 *   3. GC_ERR_ALLOC_FAIL: The string needed more memory and the allocation
 *   failed. */

void gc_str_cpy(GCString dest, GCStringView src, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* Compares two strings byte-by-byte.
 * 
 * RETURN VALUE:
 *   The return value of this function matches that of libc's strcmp(). The only
 *   exception is when there is a length difference between the strings -
 *
 *   1. When 'str1' is longer, GC_STR_DIFF_STR1_LONGER is returned;
 *   2. When 'str2' is longer, GC_STR_DIFF_STR2_LONGER is returned. 
 *
 *   If two empty strings are compared, GC_STR_DIFF_EQUAL is returned. */

typedef int gc_str_diff;

#define GC_STR_DIFF_STR1_LONGER 300
#define GC_STR_DIFF_STR2_LONGER -300
#define GC_STR_DIFF_EQUAL 0

gc_str_diff gc_str_cmp(GCStringView str1, GCStringView str2,
        bool case_sensitive);

/* -------------------------------------------------------------------------- */

#define GC_STR_FIND_NOT_FOUND -1

struct GCStringFindObject
{
    ssize_t str_pos;
    ssize_t needle_idx;
};

/* Searches the string and returns the index of the first occurrence of any
 * needle specified in 'needles'.
 *
 * RETURN VALUE:
 *   GCStringFindObject describes the position(index) inside of the first
 *   occurrence(inside the string) of needle with 'needle_idx' inside the
 *   'needles' array.
 *
 *   If the function fails or doesn't find a match, a GCStringFindObject with
 *   str_pos = GC_STR_FIND_NOT_FOUND and needle_idx = GC_STR_FIND_NOT_FOUND
 *   is returned.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'needle_count' is 0. */

struct GCStringFindObject gc_str_find(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

/* Searches the string, starting from the end.
 *
 * For RETURN VALUE and STATUS CODES, see gc_str_find(). */
struct GCStringFindObject gc_str_rfind(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

/* ------------------------------------------------------ */

struct GCStringFindAllObject
{
    struct GCStringFindObject* find_objects;
    size_t count;

    void* __vec;
};

/* Finds all occurrences of all needles inside 'needles'. This is done
 * by performing gc_str_find() iteratively(while shortening the 'haystack'
 * GCStringView) until the original GCStringView is exhausted.
 *
 * RETURN VALUE:
 *   A GCStringFindAllObject that describes the result of the gc_str_find_all()
 *   operation. The object itself contains:
 *     1. A dynamically allocated array of GCStringFindObjects that describe
 *     individual occurences,
 *     2. count of entries inside the find_objects array.
 *
 *   If the function fails or doesn't find a match, a GCStringFindAllObject with
 *   find_objects = NULL and count = 0 is returned.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'needle_count' is 0;
 *   3. GC_ERR_ALLOC_FAIL: Allocation for internal vector failed.
 *
 * NOTES:
 *   If the gc_str_find_all() finds any matches, a call to
 *   gc_str_find_all_obj_destroy() must be performed(after the object has served
 *   its purpose, of course) to free the dynamically allocated memory
 *   by the function. */

struct GCStringFindAllObject gc_str_find_all(GCStringView haystack,
        GCStringView needles[], size_t needle_count,
        bool case_sensitive, gc_status* out_status);

/* Destroys the GCStringFindAllObject to avoid memory leaks. */
void gc_str_find_all_obj_destroy(struct GCStringFindAllObject* find_all_obj);

/* -------------------------------------------------------------------------- */

struct GCStringSepObject
{
    GCStringView* views;
    size_t count;
};

/* The function finds all GCStringViews that are separated by any of the
 * separators specified in 'sep'.
 *
 * RETURN VALUE:
 *   A GCStringSepObject that describes the result of the gc_str_sep()
 *   operation. The object itself contains:
 *     1. address of dynamically allocated array of GCStringViews,
 *     2. count of entries inside the views array.
 *
 *   If the function fails, a GCStringSepObject with views = NULL and count = 0
 *   is returned.
 *
 *   If the function causes no separations, a GCStringSepObject will be returned.
 *   This object's views array will have a single view inside it - a view into
 *   the whole 'str'. count will be equal to 1.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'needle_count' is 0;
 *   3. GC_ERR_ALLOC_FAIL: Allocation for internal vector failed.
 *
 * NOTES:
 *   When the GCStringSepObject serves its purpose, a call to 
 *   gc_str_sep_obj_destroy() must be made in order to free the dynamically
 *   allocated memory by the function. */

struct GCStringSepObject gc_str_sep(GCStringView str,
        GCStringView sep[], size_t sep_count,
        bool case_sensitive, gc_status* out_status);

void gc_str_sep_obj_destroy(struct GCStringSepObject* sep_obj);

/* -------------------------------------------------------------------------- */

/* Performs a realloc() call so that the GCString's data is able to store
 * ('capacity' + 1) bytes (the extra byte comes from \0).
 * This effectively means that the GCString can store 'capacity' chars.
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'str' is NULL or 'capacity' < length of 'str';
 *   3. GC_ERR_ALLOC_FAIL: realloc() failed. This should not happen
 *   because realloc() should not fail when requesting a smaller amount of
 *   memory. */
void gc_str_reserve(GCString str, size_t capacity, gc_status* out_status);

/* ------------------------------------------------------ */

/* Performs a realloc() call for the GCString's data in order to conserve memory.
 * The new size of GCString's data will be equal to str's length + 1(for \0).
 *
 * STATUS CODES:
 *   1. GC_SUCCESS: Function call was successful;
 *   2. GC_ERR_INVALID_ARG: 'str' is NULL.
 *   3. GC_ERR_ALLOC_FAIL: realloc() failed. This should not happen
 *   because realloc() should not fail when requesting a smaller amount of
 *   memory. */
void gc_str_fit(GCString str, gc_status* out_status);

/* -------------------------------------------------------------------------- */

/* If not letter, the same char is returned. */
#define gc_str_lowerc(c) \
    ( ((c >= 'A' && c <= 'Z')) ? c + ('a' - 'A') : c )

#define gc_str_upperc(c) \
    ( ((c >= 'a' && c <= 'z')) ? c - ('a' - 'A') : c )

/* ------------------------------------------------------ */

/* Applies gc_str_lowerc() to each byte of 'str' */
void gc_str_to_lower(GCString str);

/* Applies gc_str_upperc() to each byte of 'str' */
void gc_str_to_upper(GCString str);

/* -------------------------------------------------------------------------- */

#endif // _GC_STRING_H_
