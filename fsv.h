#ifndef FLEXHEEPLE_STRING_VIEW_H_
#define FLEXHEEPLE_STRING_VIEW_H_

#include <ctype.h>
#include <stdbool.h>

/* Usage
 *  fsv string_view = ...;
 *  printf("string_view = " fsv_fmt "\n", fsv_arg(string_view));
 */
#define fsv_fmt     "%.*s"
#define fsv_arg(sv) (int) (sv).length, (sv).data

typedef struct fstring_view {
    union {
        size_t size;
        size_t length;
    };
    const char *data;
} fsv_t;

fsv_t fsv_from_cstr(const char *string);
fsv_t fsv_from_partial_cstr(const char *string, const size_t length);

fsv_t fsv_trim(fsv_t sv);
fsv_t fsv_trim_left(fsv_t sv);
fsv_t fsv_trim_right(fsv_t sv);

int    fsv_index_of(fsv_t sv, char c);
size_t fsv_strlen(const char *string);

bool fsv_eq(fsv_t sv1, fsv_t sv2, bool ignore_case);
bool fsv_ends_with(fsv_t sv, fsv_t suffix, bool ignore_case);
bool fsv_starts_with(fsv_t sv, fsv_t prefix, bool ignore_case);

bool fsv_eq_cstr(fsv_t sv1, const char *string, bool ignore_case);
bool fsv_ends_with_cstr(fsv_t sv, const char *suffix, bool ignore_case);
bool fsv_starts_with_cstr(fsv_t sv, const char *prefix, bool ignore_case);

#ifdef FSV_USE_TMP_BUFFER

// Stolen from Tsoding nob.h: https://github.com/tsoding/nob.h
#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    ifdef __MINGW_PRINTF_FORMAT
#        define FSV_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define FSV_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
//   TODO: implement FSV_PRINTF_FORMAT for MSVC
#    define FSV_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

void  *fsv_tmp_alloc(size_t bytes);
void   fsv_tmp_reset(void);
size_t fsv_tmp_save_point(void);
void   fsv_tmp_rewind(size_t checkpoint);
char  *fsv_tmp_sprintf(const char *format, ...) FSV_PRINTF_FORMAT(1, 2);

// TODO: Implement `fsv_tmp_concat` and `fsv_tmp_concat_cstr`
fsv_t fsv_tmp_concat(fsv_t sv1, fsv_t sv2);
fsv_t fsv_tmp_concat_cstr(fsv_t sv1, const char *str);
#endif // FSV_USE_TMP_BUFFER

#ifdef FSV_IMPLEMENTATION

#include <stdio.h>
#include <assert.h>

fsv_t fsv_from_cstr(const char *string) {
    return fsv_from_partial_cstr(string, fsv_strlen(string));
}

fsv_t fsv_from_partial_cstr(const char *string, const size_t length) {
    fsv_t sv = {
        .data = string,
        .length = length,
    };
    return sv;
}

fsv_t fsv_trim(fsv_t sv) {
    return fsv_trim_left(fsv_trim_right(sv));
}

fsv_t fsv_trim_left(fsv_t sv) {
    size_t i = 0;
    while (i < sv.length && isspace(sv.data[i])) {
        i++;
    }
    return fsv_from_partial_cstr(sv.data + i, sv.length - i);
}

fsv_t fsv_trim_right(fsv_t sv) {
    size_t i = 0;
    while (i < sv.length && isspace(sv.data[sv.length - i - 1])) {
        i++;
    }
    return fsv_from_partial_cstr(sv.data, sv.length - i);
}

int fsv_index_of(fsv_t sv, char c) {
    size_t index = 0;
    while (index < sv.length && sv.data[index] != c) {
        index++;
    }
    if (index < sv.length) return index;
    return -1;
}

size_t fsv_strlen(const char *string) {
    const char *str = string;
    if (str == NULL) return 0;
    while (*str != '\0') str++;
    return str - string;
}

bool fsv_eq(fsv_t sv1, fsv_t sv2, bool ignore_case) {
    if (sv1.length != sv2.length) return false;

    char a, b;
    for (size_t i = 0; i < sv1.length; ++i) {
        if (ignore_case) {
            a = tolower(sv1.data[i]);
            b = tolower(sv2.data[i]);
        } else {
            a = sv1.data[i];
            b = sv2.data[i];
        }
        if (a != b) return false;
    }
    return true;
}

bool fsv_eq_cstr(fsv_t sv1, const char *string, bool ignore_case) {
    return fsv_eq(sv1, fsv_from_cstr(string), ignore_case);
}

bool fsv_starts_with(fsv_t sv, fsv_t prefix, bool ignore_case) {
    if (prefix.length > sv.length) return false;
    fsv_t new_sv = fsv_from_partial_cstr(sv.data, prefix.length);
    return fsv_eq(new_sv, prefix, ignore_case);
}

bool fsv_starts_with_cstr(fsv_t sv, const char *prefix, bool ignore_case) {
    return fsv_starts_with(sv, fsv_from_cstr(prefix), ignore_case);
}

bool fsv_ends_with(fsv_t sv, fsv_t suffix, bool ignore_case) {
    if (suffix.length > sv.length) return false;
    fsv_t new_sv = fsv_from_partial_cstr(sv.data + (sv.length - suffix.length), suffix.length);
    return fsv_eq(new_sv, suffix, ignore_case);
}

bool fsv_ends_with_cstr(fsv_t sv, const char *suffix, bool ignore_case) {
    return fsv_ends_with(sv, fsv_from_cstr(suffix), ignore_case);
}

#ifdef FSV_USE_TMP_BUFFER

#include <stdio.h>

#ifndef FSV_TMP_CAPACITY
#define FSV_TMP_CAPACITY (8*1024)
#endif // FSV_TMP_CAPACITY

static char fsv_tmp_buffer[FSV_TMP_CAPACITY] = {0};
static size_t fsv_tmp_size = 0;

void *fsv_tmp_alloc(size_t bytes) {
    size_t word_size = sizeof(uintptr_t);
    size_t size = (bytes + word_size - 1)/word_size*word_size;
    if (fsv_tmp_size + size > FSV_TMP_CAPACITY) return NULL;
    void *ret = &fsv_tmp_buffer[fsv_tmp_size];
    fsv_tmp_size += size;
    return ret;
}

void fsv_tmp_reset(void) {
    fsv_tmp_size = 0;
}

size_t fsv_tmp_save_point(void) {
    return fsv_tmp_size;
}

void fsv_tmp_rewind(size_t checkpoint) {
    fsv_tmp_size = checkpoint;
}

char *fsv_tmp_sprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int n = vsnprintf(NULL, 0, format, args);
    va_end(args);

    assert(n >= 0);
    char *ret = (char*) fsv_tmp_alloc(n + 1);
    assert(ret != NULL && "Extend the size of temporary allocator");

    va_start(args, format);
    vsnprintf(ret, n + 1, format, args);
    va_end(args);

    return ret;
}

fsv_t fsv_tmp_concat(fsv_t sv1, fsv_t sv2) {
    size_t len = sv1.length + sv2.length;
    char *buffer = fsv_tmp_alloc(len);
    fsv_t ret = {
        .data = buffer,
        .length = len
    };

    size_t index = 0;
    for (size_t i = 0; i < sv1.length; ++i) {
        buffer[index++] = sv1.data[i];
    }
    for (size_t i = 0; i < sv2.length; ++i) {
        buffer[index++] = sv2.data[i];
    }

    return ret;
}

fsv_t fsv_tmp_concat_cstr(fsv_t sv1, const char *str) {
    return fsv_tmp_concat(sv1, fsv_from_cstr(str));
}

#endif // FSV_USE_TMP_BUFFER

#endif // FSV_IMPLEMENTATION

#endif // FLEXHEEPLE_STRING_VIEW_H_
