#ifndef FLEXHEEPLE_STRING_VIEW_H_
#define FLEXHEEPLE_STRING_VIEW_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#ifndef FSV_DEF
#    define FSV_DEF
#endif // FSV_DEF

/* Usage
 *  fsv_t string_view = ...;
 *  printf("string_view = " fsv_fmt "\n", fsv_arg(string_view));
 *  // Can also be used for fsb_t aka fstring_builder
 */
#define fsv_fmt     "%.*s"
#define fsv_arg(sv) (int) (sv).length, (sv).datas

#ifdef FSV_ENABLE_DEBUG
#    include <string.h>
#endif // FSV_ENABLE_DEBUG

#ifndef FSV_REALLOC
#    include <stdlib.h>
#    define FSV_REALLOC realloc
#endif // FSV_REALLOC

#ifndef FSV_FREE
#    include <stdlib.h>
#    define FSV_FREE free
#endif // FSV_FREE

#ifndef FSV_ASSERT
#    include <assert.h>
#    define FSV_ASSERT assert
#endif // FSV_ASSERT

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

///////////////////////// String View /////////////////////////

typedef struct fstring_view {
    union { size_t size; size_t length; };
    const char *datas;
} fsv_t;

FSV_DEF fsv_t fsv_from_cstr(const char *string);
FSV_DEF fsv_t fsv_from_partial_cstr(const char *string, const size_t length);
FSV_DEF fsv_t fsv_from_sv(const fsv_t sv, const size_t length);

FSV_DEF fsv_t fsv_trim(fsv_t sv);
FSV_DEF fsv_t fsv_trim_left(fsv_t sv);
FSV_DEF fsv_t fsv_trim_right(fsv_t sv);

FSV_DEF size_t fsv_strlen(const char *string);
FSV_DEF int    fsv_index_of(fsv_t sv, char c);
FSV_DEF char   fsv_lower(char c);
FSV_DEF char   fsv_upper(char c);
FSV_DEF bool   fsv_is_space(char c);
FSV_DEF bool   fsv_is_digit(char c);
FSV_DEF bool   fsv_is_character(char c);
FSV_DEF bool   fsv_is_alphanumeric(char c);

FSV_DEF bool fsv_eq(fsv_t sv1, fsv_t sv2, bool ignore_case);
FSV_DEF bool fsv_ends_with(fsv_t sv, fsv_t suffix, bool ignore_case);
FSV_DEF bool fsv_starts_with(fsv_t sv, fsv_t prefix, bool ignore_case);

FSV_DEF bool fsv_eq_cstr(fsv_t sv1, const char *string, bool ignore_case);
FSV_DEF bool fsv_ends_with_cstr(fsv_t sv, const char *suffix, bool ignore_case);
FSV_DEF bool fsv_starts_with_cstr(fsv_t sv, const char *prefix, bool ignore_case);

FSV_DEF bool fsv_split(fsv_t *sv, fsv_t *out);
FSV_DEF bool fsv_split_by_delim(fsv_t *sv, char delim, fsv_t *out);
FSV_DEF bool fsv_split_by_sv(fsv_t *sv, fsv_t delim, bool ignore_case, fsv_t *out);
FSV_DEF bool fsv_split_by_cstr(fsv_t *sv, const char *delim, bool ignore_case, fsv_t *out);
FSV_DEF bool fsv_split_by_pair(fsv_t *right, const char *pair, fsv_t *middle, fsv_t *left);

///////////////////////// End of String View /////////////////////////

///////////////////////// String Builder /////////////////////////
#define FSB_INITIAL_CAPACITY (2)

#ifndef fda_realloc
#define fda_realloc(da, item_added)                                                                                  \
    do {                                                                                                             \
        if ((da)->capacity == 0) { (da)->capacity = FSB_INITIAL_CAPACITY; }                                          \
        while ((da)->size + (item_added) > (da)->capacity) {                                                         \
            (da)->capacity *= 2;                                                                                     \
        }                                                                                                            \
        (da)->datas = (__typeof__(*((da)->datas))*) FSV_REALLOC((da)->datas, (da)->capacity * sizeof(*(da)->datas)); \
        FSV_ASSERT((da)->datas != NULL && "Out of Memory!!!");                                                       \
    } while (0)
#endif // fda_realloc

#ifndef fda_append
#define fda_append(da, data)                    \
    do {                                        \
        if ((da)->size + 1 >= (da)->capacity) { \
            fda_realloc((da), 1);               \
        }                                       \
        (da)->datas[(da)->size++] = data;       \
    } while (0)
#endif // fda_append

#ifndef fda_append_many
#define fda_append_many(da, _datas, _size)             \
    do {                                               \
        if ((da)->size + (_size) >= (da)->capacity) {  \
            fda_realloc((da), (_size));                \
        }                                              \
        for (size_t i = 0; i < (size_t)(_size); ++i) { \
            (da)->datas[(da)->size++] = (_datas)[i];   \
        }                                              \
    } while (0)
#endif // fda_append_many

#ifndef fda_reserve
#define fda_reserve(da, cap)                                                                                \
    do {                                                                                                    \
        if ((da)->capacity >= (cap)) { break; }                                                             \
        (da)->datas = (__typeof__(*((da)->datas))*) FSV_REALLOC((da)->datas, (cap) * sizeof(*(da)->datas)); \
        FSV_ASSERT((da)->datas != NULL && "Out of Memory!!!");                                              \
        (da)->capacity = (cap);                                                                             \
    } while (0)
#endif // fda_reserve

#ifndef fda_free
#define fda_free(da)               \
    do {                           \
        if ((da)->datas != NULL) { \
            FSV_FREE((da)->datas); \
        }                          \
        (da)->datas    = NULL;     \
        (da)->size     = 0;        \
        (da)->capacity = 0;        \
    } while (0)
#endif // fda_free

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

FSV_DEF bool fsb_read_entire_file(const char *file_path, fsb_t *sb);
FSV_DEF bool fsb_read_entire_dir(const char *parent, ffp_t *children, bool recursive);
FSV_DEF int  fsb_append_strf(fsb_t *sb, const char *fmt, ...) FSV_PRINTF_FORMAT(2, 3);
FSV_DEF void fsb_free(fsb_t *sb);
FSV_DEF void ffp_free(ffp_t *fp);

FSV_DEF fsv_t fsv_from_sb(const fsb_t sb);
FSV_DEF fsb_t fsb_from_sv(const fsv_t sv);

///////////////////////// End of String Builder /////////////////////////

///////////////////////// Temporary Buffer /////////////////////////
#ifndef FSV_DISABLE_TMP_BUFFER

FSV_DEF char  *fsv_tmp_alloc(size_t how_many_chars);
FSV_DEF void   fsv_tmp_reset(void);
FSV_DEF size_t fsv_tmp_save_point(void);
FSV_DEF void   fsv_tmp_rewind(size_t checkpoint);
FSV_DEF char  *fsv_tmp_strdup(const char *cstr);
FSV_DEF char  *fsv_tmp_sv_to_cstr(fsv_t sv);
FSV_DEF char  *fsv_tmp_sprintf(const char *format, ...) FSV_PRINTF_FORMAT(1, 2);
FSV_DEF fsv_t  fsv_tmp_concat(fsv_t sv1, fsv_t sv2);
FSV_DEF fsv_t  fsv_tmp_concat_cstr(fsv_t sv1, const char *str);
// Same as `fsv_tmp_concat` and `fsv_tmp_concat_cstr`
// But memory allocated in here is continuous
FSV_DEF fsv_t  fsv_tmp_concat_continuous(fsv_t sv1, fsv_t sv2);
FSV_DEF fsv_t  fsv_tmp_concat_continuous_cstr(fsv_t sv1, const char *str);

#endif // FSV_DISABLE_TMP_BUFFER

///////////////////////// End of Temporary Buffer /////////////////////////

///////////////////////// Logging for debug /////////////////////////
typedef enum {
    FSV_LOG_INFO,
    FSV_LOG_ERROR
} fsv_log_level_t;

FSV_DEF void fsv_log(
        const char *file, int line,
        const fsv_log_level_t level,
        const char *msg, ...
) FSV_PRINTF_FORMAT(4, 5);

#ifdef FSV_ENABLE_DEBUG
#    define FSV_LOGI(msg, ...) fsv_log(__FILE__, __LINE__, FSV_LOG_INFO, msg, ##__VA_ARGS__)
#    define FSV_LOGE(msg, ...) fsv_log(__FILE__, __LINE__, FSV_LOG_ERROR, msg, ##__VA_ARGS__)
#else
#    define FSV_LOGI(msg, ...)
#    define FSV_LOGE(msg, ...)
#endif // FSV_ENABLE_DEBUG

///////////////////////// End of Logging for debug /////////////////////////

///////////////////////// Implementation /////////////////////////

#ifdef FSV_IMPLEMENTATION

#include <stdio.h>

///////////////////////// String View /////////////////////////

FSV_DEF fsv_t fsv_from_cstr(const char *string) {
    return fsv_from_partial_cstr(string, fsv_strlen(string));
}

FSV_DEF fsv_t fsv_from_partial_cstr(const char *string, const size_t length) {
    fsv_t ret = {};
    if (length == 0) return ret;
    size_t len = fsv_strlen(string);
    ret.length = length > len ? len : length;
    ret.datas = string;

    return ret;
}

FSV_DEF fsv_t fsv_from_sv(const fsv_t sv, const size_t length) {
    return fsv_from_partial_cstr(sv.datas, length);
}

FSV_DEF fsv_t fsv_trim(fsv_t sv) {
    return fsv_trim_left(fsv_trim_right(sv));
}

FSV_DEF fsv_t fsv_trim_left(fsv_t sv) {
    size_t i = 0;
    while (i < sv.length && fsv_is_space(sv.datas[i])) {
        i++;
    }
    return fsv_from_partial_cstr(sv.datas + i, sv.length - i);
}

FSV_DEF fsv_t fsv_trim_right(fsv_t sv) {
    size_t i = 0;
    while (i < sv.length && fsv_is_space(sv.datas[sv.length - i - 1])) {
        i++;
    }
    return fsv_from_partial_cstr(sv.datas, sv.length - i);
}

FSV_DEF int fsv_index_of(fsv_t sv, char c) {
    int index = 0;
    while (index < sv.length && sv.datas[index] != c) {
        index++;
    }
    if (index < sv.length) return index;
    return -1;
}

FSV_DEF size_t fsv_strlen(const char *string) {
    const char *str = string;
    if (str == NULL) return 0;
    while (*str != '\0') str++;
    return (size_t) (str - string);
}

FSV_DEF char fsv_lower(char c) {
    if ('A' <= c && c <= 'Z') return c + ('a' - 'A');
    return c;
}

FSV_DEF char fsv_upper(char c) {
    if ('a' <= c && c <= 'z') return c - ('a' - 'A');
    return c;
}

// https://stackoverflow.com/questions/30033582/what-is-the-symbol-for-whitespace-in-c
FSV_DEF bool fsv_is_space(char c) {
    return c == ' '
        || c == '\n'
        || c == '\t'
        || c == '\v'
        || c == '\f'
        || c == '\r';
}

FSV_DEF bool fsv_is_digit(char c) {
    return '0' <= c && c <= '9';
}

FSV_DEF bool fsv_is_character(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

FSV_DEF bool fsv_is_alphanumeric(char c) {
    return fsv_is_digit(c) || fsv_is_character(c);
}

FSV_DEF bool fsv_eq(fsv_t sv1, fsv_t sv2, bool ignore_case) {
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

FSV_DEF bool fsv_eq_cstr(fsv_t sv1, const char *string, bool ignore_case) {
    return fsv_eq(sv1, fsv_from_cstr(string), ignore_case);
}

FSV_DEF bool fsv_starts_with(fsv_t sv, fsv_t prefix, bool ignore_case) {
    if (prefix.length > sv.length) return false;
    fsv_t new_sv = fsv_from_partial_cstr(sv.datas, prefix.length);
    return fsv_eq(new_sv, prefix, ignore_case);
}

FSV_DEF bool fsv_starts_with_cstr(fsv_t sv, const char *prefix, bool ignore_case) {
    return fsv_starts_with(sv, fsv_from_cstr(prefix), ignore_case);
}

FSV_DEF bool fsv_ends_with(fsv_t sv, fsv_t suffix, bool ignore_case) {
    if (suffix.length > sv.length) return false;
    fsv_t new_sv = fsv_from_partial_cstr(sv.datas + (sv.length - suffix.length), suffix.length);
    return fsv_eq(new_sv, suffix, ignore_case);
}

FSV_DEF bool fsv_ends_with_cstr(fsv_t sv, const char *suffix, bool ignore_case) {
    return fsv_ends_with(sv, fsv_from_cstr(suffix), ignore_case);
}

FSV_DEF bool fsv_split(fsv_t *sv, fsv_t *out) {
    for (size_t i = 0; i < sv->length; ++i) {
        if (fsv_is_space(sv->datas[i]))
            return fsv_split_by_delim(sv, sv->datas[i], out);
    }
    return false;
}

FSV_DEF bool fsv_split_by_delim(fsv_t *sv, char delim, fsv_t *out) {
    // This is basically:
    //     return fsv_split_by_sv(sv, fsv_from_partial_cstr(&delim, 1), false, out);
    // But I'm currently too dumb to know why valgrind doesn' t like this approach
    size_t save_point = fsv_tmp_save_point();
    char *datas = fsv_tmp_alloc(1); datas[0] = delim;
    bool ret = fsv_split_by_sv(sv, fsv_from_partial_cstr(datas, 1), false, out);

    fsv_tmp_rewind(save_point);
    return ret;
}

FSV_DEF bool fsv_split_by_sv(fsv_t *sv, fsv_t delim, bool ignore_case, fsv_t *out) {
    if (sv->length == 0 || sv->datas == NULL) return false;
    if (delim.length == 0 || delim.datas == NULL) return false;

    size_t index = 0;
    bool found = false;
    fsv_t window = fsv_from_partial_cstr(sv->datas, delim.length);
    while (true) {
        if (index + delim.length > sv->length) break;
        if (fsv_eq(delim, window, ignore_case)) { found = true; break; }
        index++; window.datas++;
    }

    if (found) {
        out->datas  = sv->datas;
        out->length = index;
        sv->datas  += index + delim.length;
        sv->length -= index + delim.length;
        return true;
    }
    return false;
}

FSV_DEF bool fsv_split_by_cstr(fsv_t *sv, const char *delim, bool ignore_case, fsv_t *out) {
    return fsv_split_by_sv(sv, fsv_from_cstr(delim), ignore_case, out);
}

FSV_DEF bool fsv_split_by_pair(fsv_t *right, const char *pair, fsv_t *middle, fsv_t *left) {
    FSV_ASSERT(fsv_strlen(pair) == 2);

    bool first = false;
    bool second = false;
    for (size_t i = 0; i < right->length; ++i) {
        if (right->datas[i] == pair[0]) first = true;
        if (right->datas[i] == pair[1]) second = true;
        if (first && second) break;
    }
    if (!first || !second) return false;

    fsv_split_by_sv(right, fsv_from_partial_cstr(pair, 1), false, left);
    fsv_split_by_sv(right, fsv_from_partial_cstr(pair + 1, 1), false, middle);

    return true;
}

///////////////////////// End of String View /////////////////////////

///////////////////////// String Builder /////////////////////////

#include <stdarg.h>

#ifdef _WIN32
#    include <windows.h>
#else
#    include <dirent.h>
#    include <errno.h>
#    include <sys/stat.h>
#endif // _WIN32

FSV_DEF bool fsb_read_entire_file(const char *file_path, fsb_t *sb) {
    bool ret = true;
    size_t byte_read = 0;
    FILE *file = fopen(file_path, "rb");

#ifndef _WIN32
    long file_size = 0;
#else
    long long file_size = 0;
#endif // _WIN32

    if (file == NULL) {
        ret = false;
        FSV_LOGE("Could not fopen file `%s`. %s", file_path, strerror(errno));
        goto result;
    }
    if (fseek(file, 0, SEEK_END) < 0) {
        ret = false;
        FSV_LOGE("Could not fseek to end of file `%s`. %s", file_path, strerror(errno));
        goto result;
    }
#ifndef _WIN32
    file_size = ftell(file);
#else
    file_size = _ftelli64(file);
#endif // _WIN32
    if (file_size < 0) {
        ret = false;
        FSV_LOGE("Could not ftell file `%s`. %s", file_path, strerror(errno));
        goto result;
    }
    if (fseek(file, 0, SEEK_SET) < 0) {
        ret = false;
        FSV_LOGE("Could not fseek to set of file `%s`. %s", file_path, strerror(errno));
        goto result;
    }

    fda_reserve(sb, sb->length + file_size + 1);
    byte_read = fread(sb->datas + sb->length, 1, file_size, file);
    FSV_ASSERT(byte_read == (size_t)file_size);
    if (ferror(file)) {
        ret = false;
        FSV_LOGE("Could not fread file `%s`. %s", file_path, strerror(errno));
        goto result;
    }
    sb->length += file_size;
    sb->datas[sb->length] = '\0';

result:
    if (file != NULL) fclose(file);
    return ret;
}

#ifndef _WIN32
bool fsb_read_entire_dir_posix(const char *parent, ffp_t *children, bool recursive) {
    bool ret             = true;
    struct dirent *ent   = NULL;
    struct stat ent_stat = {};
    fsv_t curr_ent       = {};
    ffe_t file           = {};
    size_t save_point    = fsv_tmp_save_point();
    fsv_t full_path      = fsv_from_cstr(parent);
    DIR *dir             = opendir(parent);

    if (dir == NULL) {
        ret = false;
        FSV_LOGE("Could not open folder `%s`. %s", parent, strerror(errno));
        goto result;
    }
    if (!fsv_ends_with_cstr(full_path, "/", false)) {
        full_path = fsv_tmp_concat_cstr(full_path, "/");
    }

    errno = 0;
    while (true) {
        ent = readdir(dir);
        if (ent == NULL) break;
        curr_ent = fsv_from_cstr(ent->d_name);
        if (fsv_ends_with_cstr(curr_ent, ".", false)
            || fsv_ends_with_cstr(curr_ent, "..", false)) {
            continue;
        }

        fsv_tmp_rewind(save_point);
        curr_ent = fsv_tmp_concat_cstr(full_path, ent->d_name);
        if (lstat(curr_ent.datas, &ent_stat) < 0) {
            ret = false;
            FSV_LOGE("Could not lstat entity `" fsv_fmt "`. %s",
                    fsv_arg(curr_ent), strerror(errno));
            goto result;
        }
        file.is_dir = S_ISDIR(ent_stat.st_mode);

        if (!recursive) {
            file.name = fsb_from_sv(curr_ent);
            fda_append(children, file);
        } else {
            if (!file.is_dir) {
                file.name = fsb_from_sv(curr_ent);
                fda_append(children, file);
            } else {
                if (!fsb_read_entire_dir_posix(curr_ent.datas, children, recursive)) {
                    FSV_LOGE("Could not list contents of subdir `" fsv_fmt "`. %s",
                            fsv_arg(curr_ent), strerror(errno));
                    continue;
                }
            }
        }
    }

    if (errno != 0) {
        ret = false;
        FSV_LOGE("Could not read directory `%s`. %s", parent, strerror(errno));
        goto result;
    }
result:
    fsv_tmp_rewind(save_point);
    if (dir) closedir(dir);
    return ret;
}
#endif // _WIN32

#ifdef _WIN32

const char *fsv_tmp_get_last_errmsg() {
    DWORD err = GetLastError();
    LPSTR msg_buffer = NULL;
    DWORD size = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            err,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            msg_buffer,
            0,
            NULL);

    const char *ret = fsv_tmp_strdup(msg_buffer);
    LocalFree(msg_buffer);
    return ret;
}

// https://stackoverflow.com/questions/2314542/listing-directory-contents-using-c-and-windows
bool fsb_read_entire_dir_windows(const char *parent, ffp_t *children, bool recursive) {
    bool ret           = true;
    WIN32_FIND_DATA fd = {};
    HANDLE find_handle = NULL;
    size_t save_point  = fsv_tmp_save_point();
    fsv_t root         = fsv_from_cstr(parent);
    fsv_t file_path    = fsv_tmp_concat_cstr(root, "\\*.*");
    ffe_t file         = {};
    fsv_t curr_ent     = {};

    find_handle = FindFirstFile(file_path.datas, &fd);
    if (find_handle == INVALID_HANDLE_VALUE) {
        ret = false;
        FSV_LOGE("Could not open folder `%s`. %s", parent, fsv_tmp_get_last_errmsg());
        goto result;
    }

    do {
        curr_ent = fsv_tmp_concat_continuous_cstr(root, "/");
        curr_ent = fsv_tmp_concat_cstr(curr_ent, fd.cFileName);
        if (fsv_ends_with_cstr(curr_ent, ".", false)
            || fsv_ends_with_cstr(curr_ent, "..", false)) {
            continue;
        }
        file.is_dir = fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

        if (!recursive) {
            file.name = fsb_from_sv(curr_ent);
            fda_append(children, file);
        } else {
            if (!file.is_dir) {
                file.name = fsb_from_sv(curr_ent);
                fda_append(children, file);
            } else {
                if (!fsb_read_entire_dir_windows(curr_ent.datas, children, recursive)) {
                    FSV_LOGE("Could not list contents of subdir `" fsv_fmt "`. %s",
                            fsv_arg(curr_ent), strerror(errno));
                    continue;
                }
            }
        }
    } while (FindNextFile(find_handle, &fd));

result:
    FindClose(find_handle);
    fsv_tmp_rewind(save_point);
    return ret;
}
#endif // _WIN32

FSV_DEF bool fsb_read_entire_dir(const char *parent, ffp_t *children, bool recursive) {
#ifndef _WIN32
    return fsb_read_entire_dir_posix(parent, children, recursive);
#else
    return fsb_read_entire_dir_windows(parent, children, recursive);
#endif // _WIN32
}

FSV_DEF int fsb_append_strf(fsb_t *sb, const char *fmt, ...) {
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

FSV_DEF void fsb_free(fsb_t *sb) {
    fda_free(sb);
}

FSV_DEF void ffp_free(ffp_t *fp) {
    if (fp->datas == NULL) return;
    ffe_t *fe = NULL;
    for (size_t i = 0; i < fp->length; ++i) {
        fe = &fp->datas[i];
        fsb_free(&fe->name);
        fe->is_dir = false;
    }
    fda_free(fp);
}

FSV_DEF fsv_t fsv_from_sb(const fsb_t sb) {
    return fsv_from_partial_cstr(sb.datas, sb.length);
}

FSV_DEF fsb_t fsb_from_sv(const fsv_t sv) {
    fsb_t ret = {};
    fda_reserve(&ret, sv.length + 1);
    for (size_t i = 0; i < sv.length; ++i) {
        fda_append(&ret, sv.datas[i]);
    }
    // Set null-terminated for passing into
    // `fsb_read_entire_dir` or `fsb_read_entire_file`
    // while doesn't make `fsv_eq` produce wrong result
    ret.datas[sv.length] = '\0';
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

FSV_DEF char *fsv_tmp_alloc(size_t how_many_chars) {
    if (fsv_tmp_size + how_many_chars >= FSV_TMP_CAPACITY) {
        FSV_ASSERT(false && "Extend the size of temporary allocator");
        return NULL;
    }
    char *ret = &fsv_tmp_buffer[fsv_tmp_size];
    fsv_tmp_size += how_many_chars;
    return ret;
}

FSV_DEF void fsv_tmp_reset(void) {
    fsv_tmp_size = 0;
}

FSV_DEF size_t fsv_tmp_save_point(void) {
    return fsv_tmp_size;
}

FSV_DEF void fsv_tmp_rewind(size_t checkpoint) {
    fsv_tmp_size = checkpoint;
}

FSV_DEF char *fsv_tmp_strdup(const char *cstr) {
    size_t len = fsv_strlen(cstr);
    char *ret = (char*)fsv_tmp_alloc(len + 1);
    for (size_t i = 0; i < len; ++i) {
        ret[i] = cstr[i];
    }
    ret[len] = '\0';

    return ret;
}

FSV_DEF char *fsv_tmp_sprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int n = vsnprintf(NULL, 0, format, args);
    va_end(args);

    FSV_ASSERT(n >= 0);
    char *ret = (char*) fsv_tmp_alloc(n + 1);

    va_start(args, format);
    vsnprintf(ret, n + 1, format, args);
    va_end(args);

    return ret;
}

FSV_DEF char *fsv_tmp_sv_to_cstr(fsv_t sv) {
    if (sv.length == 0 || sv.datas == NULL) return NULL;
    char *ret = (char*)fsv_tmp_alloc(sv.length + 1);

    for (size_t i = 0; i < sv.length; ++i) {
        ret[i] = sv.datas[i];
    }
    ret[sv.length] = '\0';

    return ret;
}

FSV_DEF fsv_t fsv_tmp_concat(fsv_t sv1, fsv_t sv2) {
    fsv_t ret;
    size_t length = sv1.length + sv2.length;
    char *buffer = (char*) fsv_tmp_alloc(length);
    ret.length = length;
    ret.datas = buffer;

    size_t index = 0;
    if (sv1.length > 0 || sv1.datas != NULL) {
        for (size_t i = 0; i < sv1.length; ++i) {
            buffer[index++] = sv1.datas[i];
        }
    }
    if (sv2.length > 0 || sv2.datas != NULL) {
        for (size_t i = 0; i < sv2.length; ++i) {
            buffer[index++] = sv2.datas[i];
        }
    }
    if (fsv_tmp_save_point() + 1 < FSV_TMP_CAPACITY)
        buffer[length] = '\0';

    return ret;
}

FSV_DEF fsv_t fsv_tmp_concat_cstr(fsv_t sv1, const char *str) {
    return fsv_tmp_concat(sv1, fsv_from_cstr(str));
}

FSV_DEF fsv_t fsv_tmp_concat_continuous(fsv_t sv1, fsv_t sv2) {
    size_t save_point = fsv_tmp_save_point();
    fsv_t ret = fsv_tmp_concat(sv1, sv2);
    fsv_tmp_rewind(save_point);
    return ret;
}

FSV_DEF fsv_t fsv_tmp_concat_continuous_cstr(fsv_t sv1, const char *str) {
    return fsv_tmp_concat_continuous(sv1, fsv_from_cstr(str));
}

#endif // FSV_DISABLE_TMP_BUFFER
///////////////////////// End of Temporary Buffer /////////////////////////

FSV_DEF void fsv_log(const char *file, int line, const fsv_log_level_t level, const char *msg, ...) {
    FILE *output   = NULL;
    va_list arg    = {};
    int msg_length = 0;

    if (level == FSV_LOG_INFO) output = stdout;
    else if (level == FSV_LOG_ERROR) output = stderr;
    FSV_ASSERT(output != NULL);

    va_start(arg, msg);
    msg_length = vsnprintf(NULL, 0, msg, arg);
    va_end(arg);

    if ((int)fsv_tmp_save_point() < FSV_TMP_CAPACITY - msg_length - 1) {
        /*                                     [  msg_length  ]
         * [---------------->------------------<--------------]
         * [ tmp_save_point ]
         */
        char *buffer = fsv_tmp_buffer + (FSV_TMP_CAPACITY - msg_length - 1);
        va_start(arg, msg);
        vsnprintf(buffer, msg_length + 1, msg, arg);
        va_end(arg);

        buffer[msg_length] = '\0';
        fprintf(output, "%s:%d: %s\n", file, line, buffer);
    } else {
        /*               [           msg_length               ]
         * [-------------<-->---------------------------------]
         * [ tmp_save_point ]
         */
        fsb_t sb = {};
        fda_reserve(&sb, (size_t)(msg_length + 1));
        FSV_ASSERT(sb.datas != NULL);

        va_start(arg, msg);
        vsnprintf(sb.datas, msg_length + 1, msg, arg);
        va_end(arg);

        sb.datas[msg_length] = '\0';
        fprintf(output, "%s:%d: %s\n", file, line, sb.datas);

        fsb_free(&sb);
    }
}

#endif // FSV_IMPLEMENTATION
///////////////////////// End of Implementation /////////////////////////

#endif // FLEXHEEPLE_STRING_VIEW_H_
