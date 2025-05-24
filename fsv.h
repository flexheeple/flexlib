#ifndef FLEXHEEPLE_STRING_VIEW_H_
#define FLEXHEEPLE_STRING_VIEW_H_

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

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

int  fsv_index_of(fsv_t sv, char c);

bool fsv_eq(fsv_t sv1, fsv_t sv2, bool ignore_case);
bool fsv_ends_with(fsv_t sv, fsv_t suffix, bool ignore_case);
bool fsv_starts_with(fsv_t sv, fsv_t prefix, bool ignore_case);

bool fsv_eq_cstr(fsv_t sv1, const char *string, bool ignore_case);
bool fsv_ends_with_cstr(fsv_t sv, const char *suffix, bool ignore_case);
bool fsv_starts_with_cstr(fsv_t sv, const char *prefix, bool ignore_case);

#ifdef FSV_IMPLEMENTATION
fsv_t fsv_from_cstr(const char *string) {
    return fsv_from_partial_cstr(string, strlen(string));
}

fsv_t fsv_from_partial_cstr(const char *string, const size_t length) {
    fsv_t sv;
    sv.data = string;
    sv.length = length;
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

#endif // FSV_IMPLEMENTATION

#endif // FLEXHEEPLE_STRING_VIEW_H_
