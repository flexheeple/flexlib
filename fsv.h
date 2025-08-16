#ifndef FLEXHEEPLE_STRING_VIEW_H_
#define FLEXHEEPLE_STRING_VIEW_H_

#include <stdint.h>
#include <stdbool.h>

/* Usage
 *  fsv string_view = ...;
 *  printf("string_view = " fsv_fmt "\n", fsv_arg(string_view));
 *  // Can also be used for fsb_t aka fstring_builder
 */
#define fsv_fmt     "%.*s"
#define fsv_arg(sv) (int) (sv).length, (sv).datas

///////////////////////// String View /////////////////////////

typedef struct fstring_view {
    union { size_t size; size_t length; };
    const char *datas;
} fsv_t;

fsv_t fsv_from_cstr(const char *string);
fsv_t fsv_from_partial_cstr(const char *string, const size_t length);
fsv_t fsv_from_sv(const fsv_t sv, const size_t length);

fsv_t fsv_trim(fsv_t sv);
fsv_t fsv_trim_left(fsv_t sv);
fsv_t fsv_trim_right(fsv_t sv);

size_t fsv_strlen(const char *string);
int    fsv_index_of(fsv_t sv, char c);
char   fsv_lower(char c);
char   fsv_upper(char c);
bool   fsv_is_space(char c);
bool   fsv_is_digit(char c);
bool   fsv_is_character(char c);
bool   fsv_is_alphanumeric(char c);

bool fsv_eq(fsv_t sv1, fsv_t sv2, bool ignore_case);
bool fsv_ends_with(fsv_t sv, fsv_t suffix, bool ignore_case);
bool fsv_starts_with(fsv_t sv, fsv_t prefix, bool ignore_case);

bool fsv_eq_cstr(fsv_t sv1, const char *string, bool ignore_case);
bool fsv_ends_with_cstr(fsv_t sv, const char *suffix, bool ignore_case);
bool fsv_starts_with_cstr(fsv_t sv, const char *prefix, bool ignore_case);

bool fsv_split(fsv_t *sv, fsv_t *out);
bool fsv_split_by_delim(fsv_t *sv, char delim, fsv_t *out);

///////////////////////// End of String View /////////////////////////

///////////////////////// String Builder /////////////////////////

typedef struct fstring_builder {
    union { size_t size; size_t length; };
    size_t capacity;
    char *datas;
} fsb_t;

typedef struct f_file_entity {
    fsb_t name;
    bool is_dir;
} ffe_t;

typedef struct f_file_path {
    union { size_t size; size_t length; };
    size_t capacity;
    ffe_t *datas;
} ffp_t;

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

bool fsb_read_entire_file(const char *file_path, fsb_t *sb);
bool fsb_read_entire_dir(const char *parent, ffp_t *children, bool recursive);
int  fsb_append_strf(fsb_t *sb, const char *fmt, ...) FSB_PRINTF_FORMAT(2, 3);
void fsb_free(fsb_t *sb);
void ffp_free(ffp_t *fp);

fsv_t fsv_from_sb(const fsb_t sb);
fsb_t fsb_from_sv(const fsv_t sv);

///////////////////////// End of String Builder /////////////////////////

///////////////////////// Temporary Buffer /////////////////////////
#ifndef FSV_DISABLE_TMP_BUFFER

void  *fsv_tmp_alloc(size_t bytes);
void   fsv_tmp_reset(void);
size_t fsv_tmp_save_point(void);
void   fsv_tmp_rewind(size_t checkpoint);
char  *fsv_tmp_strdup(const char *cstr);
char  *fsv_tmp_sprintf(const char *format, ...) FSB_PRINTF_FORMAT(1, 2);
fsv_t  fsv_tmp_concat(fsv_t sv1, fsv_t sv2);
fsv_t  fsv_tmp_concat_cstr(fsv_t sv1, const char *str);

#endif // FSV_DISABLE_TMP_BUFFER

///////////////////////// End of Temporary Buffer /////////////////////////

///////////////////////// Implementation /////////////////////////
#ifdef FSV_IMPLEMENTATION

#include <stdio.h>

///////////////////////// String View /////////////////////////

fsv_t fsv_from_cstr(const char *string) {
    return fsv_from_partial_cstr(string, fsv_strlen(string));
}

fsv_t fsv_from_partial_cstr(const char *string, const size_t length) {
    return (fsv_t) { .length = length, .datas = string };
}

fsv_t fsv_from_sv(const fsv_t sv, const size_t length) {
    return (fsv_t) { .length = length, .datas = sv.datas };
}

fsv_t fsv_trim(fsv_t sv) {
    return fsv_trim_left(fsv_trim_right(sv));
}

fsv_t fsv_trim_left(fsv_t sv) {
    size_t i = 0;
    while (i < sv.length && fsv_is_space(sv.datas[i])) {
        i++;
    }
    return fsv_from_partial_cstr(sv.datas + i, sv.length - i);
}

fsv_t fsv_trim_right(fsv_t sv) {
    size_t i = 0;
    while (i < sv.length && fsv_is_space(sv.datas[sv.length - i - 1])) {
        i++;
    }
    return fsv_from_partial_cstr(sv.datas, sv.length - i);
}

int fsv_index_of(fsv_t sv, char c) {
    size_t index = 0;
    while (index < sv.length && sv.datas[index] != c) {
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

char fsv_lower(char c) {
    if ('A' <= c && c <= 'Z') return c - 'A';
    return c;
}

char fsv_upper(char c) {
    if ('a' <= c && c <= 'z') return c - ('a' - 'A');
    return c;
}

bool fsv_is_space(char c) {
    return c == ' ' || c == '\n' || c == '\t';
}

bool fsv_is_digit(char c) {
    return '0' <= c && c <= '9';
}

bool fsv_is_character(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

bool fsv_is_alphanumeric(char c) {
    return fsv_is_digit(c) || fsv_is_character(c);
}

bool fsv_eq(fsv_t sv1, fsv_t sv2, bool ignore_case) {
    if (sv1.length != sv2.length) return false;

    char a, b;
    for (size_t i = 0; i < sv1.length; ++i) {
        if (ignore_case) {
            a = fsv_lower(sv1.datas[i]);
            b = fsv_lower(sv2.datas[i]);
        } else {
            a = sv1.datas[i];
            b = sv2.datas[i];
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
    fsv_t new_sv = fsv_from_partial_cstr(sv.datas, prefix.length);
    return fsv_eq(new_sv, prefix, ignore_case);
}

bool fsv_starts_with_cstr(fsv_t sv, const char *prefix, bool ignore_case) {
    return fsv_starts_with(sv, fsv_from_cstr(prefix), ignore_case);
}

bool fsv_ends_with(fsv_t sv, fsv_t suffix, bool ignore_case) {
    if (suffix.length > sv.length) return false;
    fsv_t new_sv = fsv_from_partial_cstr(sv.datas + (sv.length - suffix.length), suffix.length);
    return fsv_eq(new_sv, suffix, ignore_case);
}

bool fsv_ends_with_cstr(fsv_t sv, const char *suffix, bool ignore_case) {
    return fsv_ends_with(sv, fsv_from_cstr(suffix), ignore_case);
}

bool fsv_split(fsv_t *sv, fsv_t *out) {
    if (sv->length == 0 || sv->datas == NULL) return false;

    size_t index = 0;
    while (true) {
        if (index >= sv->length) break;
        if (fsv_is_space(sv->datas[index])) break;
        index++;
    }
    if (index == sv->length) {
        out->datas   = sv->datas;
        out->length  = sv->length;
        sv->datas    = NULL;
        sv->length   = 0;
    } else {
        out->datas   = sv->datas;
        out->length  = index;
        sv->datas   += index + 1;
        sv->length  -= index + 1;
    }
    return true;
}

bool fsv_split_by_delim(fsv_t *sv, char delim, fsv_t *out) {
    if (sv->length == 0 || sv->datas == NULL) return false;

    size_t index = 0;
    while (true) {
        if (index >= sv->length) break;
        if (sv->datas[index] == delim) break;
        index++;
    }
    if (index == sv->length) {
        out->datas   = sv->datas;
        out->length  = sv->length;
        sv->datas    = NULL;
        sv->length   = 0;
    } else {
        out->datas   = sv->datas;
        out->length  = index;
        sv->datas   += index + 1;
        sv->length  -= index + 1;
    }
    return true;
}

///////////////////////// End of String View /////////////////////////

///////////////////////// String Builder /////////////////////////

#include <stdarg.h>

#include <dirent.h>   // fsb_read_entire_dir
#include <errno.h>    // fsb_read_entire_dir
#include <sys/stat.h> // fsb_read_entire_dir

#ifndef FSB_REALLOC
#    include <stdlib.h>
#    define FSB_REALLOC realloc
#endif // FSB_REALLOC

#ifndef FSB_FREE
#    include <stdlib.h>
#    define FSB_FREE free
#endif // FSB_FREE

#ifndef FSB_ASSERT
#    include <assert.h>
#    define FSB_ASSERT assert
#endif // FSB_ASSERT

#define FSB_INITIAL_CAPACITY (2)

#undef  fda_realloc
#define fda_realloc(da, item_added)                                                    \
    do {                                                                               \
        if ((da)->capacity == 0) { (da)->capacity = FSB_INITIAL_CAPACITY; }            \
        while ((da)->size + (item_added) > (da)->capacity) {                           \
            (da)->capacity *= 2;                                                       \
        }                                                                              \
        (da)->datas = FSB_REALLOC((da)->datas, (da)->capacity * sizeof(*(da)->datas)); \
        FSB_ASSERT((da)->datas != NULL && "Out of Memory!!!");                         \
    } while (0)

#undef  fda_append
#define fda_append(da, data)                      \
    do {                                          \
        if ((da)->size + 1 >= (da)->capacity) {   \
            fda_realloc((da), 1);                 \
        }                                         \
        (da)->datas[(da)->size++] = data;         \
    } while (0)

#undef  fda_append_many
#define fda_append_many(da, _datas, size)              \
    do {                                               \
        if ((da)->size + (size) >= (da)->capacity) {   \
            fda_realloc((da), (size));                 \
        }                                              \
        for (int i = 0; i < (size); ++i) {             \
            (da)->datas[(da)->size++] = (_datas)[i];   \
        }                                              \
    } while (0)

#undef  fda_reserve
#define fda_reserve(da, cap)                                                  \
    do {                                                                      \
        if ((da)->capacity >= (cap)) { break; }                               \
        (da)->datas = FSB_REALLOC((da)->datas, (cap) * sizeof(*(da)->datas)); \
        FSB_ASSERT((da)->datas != NULL && "Out of Memory!!!");                \
    } while (0)

#undef fda_free
#define fda_free(da)               \
    do {                           \
        if ((da)->datas != NULL) { \
            FSB_FREE((da)->datas); \
        }                          \
        (da)->datas    = NULL;     \
        (da)->size     = 0;        \
        (da)->capacity = 0;        \
    } while (0)

bool fsb_read_entire_file(const char *file_path, fsb_t *sb) {
    bool ret = true;
    FILE *file = fopen(file_path, "rb");

#ifndef _WIN32
    long file_size = 0;
#else
    long long file_size = 0;
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

    fda_reserve(sb, sb->length + file_size);
    fread(sb->datas + sb->length, file_size, 1, file);
    if (ferror(file)) { ret = false; goto result; }
    sb->length += file_size;

result:
    if (file != NULL) fclose(file);
    return ret;
}

bool fsb_read_entire_dir(const char *parent, ffp_t *children, bool recursive) {
    bool ret             = true;
    DIR *dir             = NULL;
    struct dirent *ent   = NULL;
    struct stat ent_stat = {};
    fsv_t curr_ent       = {};
    ffe_t file           = {};
    size_t save_point    = fsv_tmp_save_point();
    fsv_t full_path      = fsv_from_cstr(parent);

    dir = opendir(parent);
    if (dir == NULL) { ret = false; goto result; }
    if (!fsv_ends_with_cstr(full_path, "/", false)) {
        full_path = fsv_tmp_concat_cstr(full_path, "/");
    }

    errno = 0;
    ent = readdir(dir);
    while (ent != NULL) {
        curr_ent = fsv_from_cstr(ent->d_name);
        if (fsv_ends_with_cstr(curr_ent, ".", false)
            || fsv_ends_with_cstr(curr_ent, "..", false)) {
            ent = readdir(dir);
            continue;
        }

        fsv_tmp_rewind(save_point);
        curr_ent = fsv_tmp_concat_cstr(full_path, ent->d_name);
        if (lstat(curr_ent.datas, &ent_stat) < 0) { ret = false; goto result; }
        file.is_dir = S_ISDIR(ent_stat.st_mode);

        if (!recursive) {
            file.name = fsb_from_sv(curr_ent);
            fda_append(children, file);
        } else {
            if (!file.is_dir) {
                file.name = fsb_from_sv(curr_ent);
                fda_append(children, file);
            } else {
                if (!fsb_read_entire_dir(curr_ent.datas, children, recursive)) {
                    ret = false; goto result;
                }
            }
        }
        ent = readdir(dir);
    }

    if (errno != 0) { ret = false; goto result; }
result:
    (void) recursive;
    if (dir) closedir(dir);
    return ret;
}

int fsb_append_strf(fsb_t *sb, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    fda_reserve(sb, sb->length + n + 1);
    char *dest = sb->datas + sb->length;
    va_start(args, fmt);
    vsnprintf(dest, n + 1, fmt, args);
    va_end(args);

    sb->length += n;
    return n;
}

void fsb_free(fsb_t *sb) {
    if (sb->datas != NULL) FSB_FREE(sb->datas);
    sb->length   = 0;
    sb->capacity = 0;
    sb->datas     = NULL;
}

void ffp_free(ffp_t *fp) {
    if (fp->datas == NULL) return;
    ffe_t *fe = NULL;
    for (size_t i = 0; i < fp->length; ++i) {
        fe = &fp->datas[i];
        fsb_free(&fe->name);
        fe->is_dir = false;
    }
    fda_free(fp);
}

fsv_t fsv_from_sb(const fsb_t sb) {
    return fsv_from_partial_cstr(sb.datas, sb.length);
}

fsb_t fsb_from_sv(const fsv_t sv) {
    fsb_t ret = {};
    fda_reserve(&ret, sv.length);
    for (size_t i = 0; i < sv.length; ++i) {
        fda_append(&ret, sv.datas[i]);
    }
    return ret;
}

///////////////////////// End of String Builder /////////////////////////

///////////////////////// Temporary Buffer /////////////////////////
#ifndef FSV_DISABLE_TMP_BUFFER

#ifndef FSV_TMP_CAPACITY
#define FSV_TMP_CAPACITY (10*1024)
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

char *fsv_tmp_strdup(const char *cstr) {
    size_t len = fsv_strlen(cstr);
    char *ret = (char*)fsv_tmp_alloc(len + 1);
    FSB_ASSERT(ret != NULL && "Extend the size of temporary allocator");

    for (size_t i = 0; i < len; ++i) {
        ret[i] = cstr[i];
    }
    ret[len] = '\0';

    return ret;
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
    char *buffer = (char*) fsv_tmp_alloc(length + 1);
    fsv_t ret = { .length = length, .datas = buffer };
    FSB_ASSERT(buffer != NULL && "Extend the size of temporary allocator");

    size_t index = 0;
    if (sv1.length > 0) {
        for (size_t i = 0; i < sv1.length; ++i) {
            buffer[index++] = sv1.datas[i];
        }
    }
    if (sv2.length > 0) {
        for (size_t i = 0; i < sv2.length; ++i) {
            buffer[index++] = sv2.datas[i];
        }
    }
    buffer[length] = '\0';

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
