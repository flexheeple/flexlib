#ifndef FLEXHEEPLE_STRING_VIEW_H_
#define FLEXHEEPLE_STRING_VIEW_H_

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Usage
 *  fsv string_view = ...;
 *  printf("string_view = " fsv_fmt "\n", fsv_arg(string_view));
 *  // Can also be used for fsb_t aka fstring_builder
 */
#define fsv_fmt     "%.*s"
#define fsv_arg(sv) (int) (sv).length, (sv).data

///////////////////////// String View /////////////////////////

typedef struct fstring_view {
    union { size_t size; size_t length; };
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

///////////////////////// End of String View /////////////////////////

///////////////////////// String Builder /////////////////////////

typedef struct fstring_builder {
    union { size_t size; size_t length; };
    size_t capacity;
    char *data;
} fsb_t;

// Stolen from Tsoding nob.h: https://github.com/tsoding/nob.h
#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    ifdef __MINGW_PRINTF_FORMAT
#        define FSB_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define FSB_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
//   TODO: implement FSV_PRINTF_FORMAT for MSVC
#    define FSB_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

bool fsb_need_resize(const fsb_t *sb, size_t item_added);
bool fsb_reserve(fsb_t *sb, size_t capacity);
bool fsb_append(fsb_t *sb, char character);
bool fsb_append_many(fsb_t *sb, const char *datas, size_t size);
bool fsb_read_entire_file(const char *file_path, fsb_t *sb);
int  fsb_append_strf(fsb_t *sb, const char *fmt, ...) FSB_PRINTF_FORMAT(2, 3);
void fsb_free(fsb_t *sb);

fsv_t fsb_to_fsv(const fsb_t *sb);

///////////////////////// End of String Builder /////////////////////////

///////////////////////// Temporary Buffer /////////////////////////
#ifndef FSV_DISABLE_TMP_BUFFER

void  *fsv_tmp_alloc(size_t bytes);
void   fsv_tmp_reset(void);
size_t fsv_tmp_save_point(void);
void   fsv_tmp_rewind(size_t checkpoint);
char  *fsv_tmp_sprintf(const char *format, ...) FSB_PRINTF_FORMAT(1, 2);

// TODO: Implement `fsv_tmp_concat` and `fsv_tmp_concat_cstr`
fsv_t fsv_tmp_concat(fsv_t sv1, fsv_t sv2);
fsv_t fsv_tmp_concat_cstr(fsv_t sv1, const char *str);
#endif // FSV_DISABLE_TMP_BUFFER

///////////////////////// End of Temporary Buffer /////////////////////////

///////////////////////// Implementation /////////////////////////
#ifdef FSV_IMPLEMENTATION

#include <stdio.h>
#include <assert.h>

///////////////////////// String View /////////////////////////

fsv_t fsv_from_cstr(const char *string) {
    return fsv_from_partial_cstr(string, fsv_strlen(string));
}

fsv_t fsv_from_partial_cstr(const char *string, const size_t length) {
    return (fsv_t) { .length = length, .data = string };
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
///////////////////////// End of String View /////////////////////////

///////////////////////// String Builder /////////////////////////

#include <stdio.h>
#include <stdarg.h>

#ifndef FSB_REALLOC
    #include <stdlib.h>
    #define FSB_REALLOC realloc
#endif // FSB_REALLOC

#ifndef FSB_FREE
    #include <stdlib.h>
    #define FSB_FREE free
#endif // FSB_FREE

#ifndef FSB_ASSERT
    #include <assert.h>
    #define FSB_ASSERT assert
#endif // FSB_ASSERT

#define FSB_INITIAL_CAPACITY (2)

bool fsb_need_resize(const fsb_t *sb, size_t item_added) {
    return sb->length + item_added >= sb->capacity;
}

bool fsb_realloc(fsb_t *sb, size_t added_items) {
    if (sb->capacity == 0) sb->capacity = FSB_INITIAL_CAPACITY;
    while (sb->length + added_items > sb->capacity) sb->capacity *= 2;
    sb->data = (char*) FSB_REALLOC(sb->data, sb->capacity*sizeof(char *));
    return sb->data != NULL;
}

bool fsb_reserve(fsb_t *sb, size_t capacity) {
    if (sb->capacity >= capacity) return true;
    sb->data = (char*) FSB_REALLOC(sb->data, capacity * sizeof(char *));
    return sb->data != NULL;
}

bool fsb_append(fsb_t *sb, char character) {
    if (fsb_need_resize(sb, 1)) {
        FSB_ASSERT(fsb_realloc(sb, 1) == true && "Out of memory!!!");
    }
    sb->data[sb->length++] = character;
    return true;
}

bool fsb_append_many(fsb_t *sb, const char *datas, size_t size) {
    if (fsb_need_resize(sb, size)) {
        FSB_ASSERT(fsb_realloc(sb, size) == true && "Out of memory!!!");
    }
    for (size_t i = 0; i < size; ++i) {
        sb->data[sb->length++] = datas[i];
    }
    return true;
}

bool fsb_read_entire_file(const char *file_path, fsb_t *sb) {
    bool ret = true;
    FILE *file = fopen(file_path, "rb");

#ifndef _WIN32
    long file_size;
#else
    long long file_size;
#endif // _WIN32

    if (file == NULL) { ret = false; goto result; }
    if (fseek(file, 0, SEEK_END) < 0) { ret = false; goto result; }
#ifndef _WIN32
    file_size = ftell(file);
#else
    file_size = _ftelli64(file);
#endif // _WIN32
    if (file_size < 0) { ret = false; goto result; }
    if (fseek(file, 0, SEEK_SET) < 0) { ret = false; goto result; }

    if (!fsb_reserve(sb, sb->length + file_size)) { ret = false; goto result; }
    fread(sb->data + sb->length, file_size, 1, file);
    if (ferror(file)) { ret = false; goto result; }
    sb->length += file_size;

result:
    fclose(file);
    return ret;
}

int fsb_append_strf(fsb_t *sb, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    fsb_reserve(sb, sb->length + n + 1);
    char *dest = sb->data + sb->length;
    va_start(args, fmt);
    vsnprintf(dest, n + 1, fmt, args);
    va_end(args);

    sb->length += n;
    return n;
}

void fsb_free(fsb_t *sb) {
    if (sb->data != NULL) {
        FSB_FREE(sb->data);
        sb->length   = 0;
        sb->capacity = 0;
        sb->data     = NULL;
    }
}

fsv_t fsb_to_fsv(const fsb_t *sb) {
    return fsv_from_partial_cstr(sb->data, sb->length);
}

///////////////////////// End of String Builder /////////////////////////

///////////////////////// Temporary Buffer /////////////////////////
#ifndef FSV_DISABLE_TMP_BUFFER

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

    FSB_ASSERT(n >= 0);
    char *ret = (char*) fsv_tmp_alloc(n + 1);
    FSB_ASSERT(ret != NULL && "Extend the size of temporary allocator");

    va_start(args, format);
    vsnprintf(ret, n + 1, format, args);
    va_end(args);

    return ret;
}

fsv_t fsv_tmp_concat(fsv_t sv1, fsv_t sv2) {
    size_t length = sv1.length + sv2.length;
    char *buffer = (char*) fsv_tmp_alloc(length);
    fsv_t ret = { .length = length, .data = buffer };
    FSB_ASSERT(buffer != NULL && "Extend the size of temporary allocator");

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

#endif // FSV_DISABLE_TMP_BUFFER
///////////////////////// End of Temporary Buffer /////////////////////////

#endif // FSV_IMPLEMENTATION
///////////////////////// End of Implementation /////////////////////////

#endif // FLEXHEEPLE_STRING_VIEW_H_
