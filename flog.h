#ifndef FLEXHEEPLE_LOG_H_
#define FLEXHEEPLE_LOG_H_

typedef enum {
    FLOG_LEVEL_VERBOSE,
    FLOG_LEVEL_DEBUG,
    FLOG_LEVEL_INFO,
    FLOG_LEVEL_WARN,
    FLOG_LEVEL_ERROR,
    FLOG_LEVEL_FATAL,
    FLOG_LEVEL_COUNT
} flog_level_t;

void _FLOG_COMMON_(const flog_level_t level, const char *function, const int line, const char *message, ...);

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif // __clang__

#define FLOGF(msg, ...) _FLOG_COMMON_(FLOG_LEVEL_FATAL,   __func__, __LINE__, msg, ##__VA_ARGS__)
#define FLOGE(msg, ...) _FLOG_COMMON_(FLOG_LEVEL_ERROR,   __func__, __LINE__, msg, ##__VA_ARGS__)
#define FLOGW(msg, ...) _FLOG_COMMON_(FLOG_LEVEL_WARN,    __func__, __LINE__, msg, ##__VA_ARGS__)
#define FLOGI(msg, ...) _FLOG_COMMON_(FLOG_LEVEL_INFO,    __func__, __LINE__, msg, ##__VA_ARGS__)
#define FLOGD(msg, ...) _FLOG_COMMON_(FLOG_LEVEL_DEBUG,   __func__, __LINE__, msg, ##__VA_ARGS__)
#define FLOGV(msg, ...) _FLOG_COMMON_(FLOG_LEVEL_VERBOSE, __func__, __LINE__, msg, ##__VA_ARGS__)

#ifdef __clang__
    #pragma clang diagnostic pop
#endif // __clang__

#ifndef FLOG_LEVEL
#ifdef _DEBUG
    #define FLOG_LEVEL (FLOG_LEVEL_VERBOSE)
#else
    #define FLOG_LEVEL (FLOG_LEVEL_INFO)
#endif // _DEBUG
#endif // FLOG_LEVEL

#ifdef FLOG_IMPLEMENTATION

#include <stdio.h>
#include <stdarg.h>

#ifndef FLOG_DISABLE_TIMESTAMP
#include <string.h>
#include <errno.h>
#include <time.h>

#define FLOG_TIMESTAMP_BUFFER_SIZE (size_t) (31)
#endif

#ifndef FLOG_BUFFER_SIZE
#define FLOG_BUFFER_SIZE (size_t) (1024)
#endif // FLOG_BUFFER_SIZE

static const char log_levels[FLOG_LEVEL_COUNT] = {
    /* FLOG_LEVEL_VERBOSE */ 'V',
    /* FLOG_LEVEL_DEBUG   */ 'D',
    /* FLOG_LEVEL_INFO    */ 'I',
    /* FLOG_LEVEL_WARN    */ 'W',
    /* FLOG_LEVEL_ERROR   */ 'E',
    /* FLOG_LEVEL_FATAL   */ 'F',
};

void _FLOG_COMMON_(const flog_level_t level, const char *function, const int line, const char *message, ...) {
    va_list arg;
    int ret = 0;
    int offset = 0;
    char prefix[FLOG_BUFFER_SIZE >> 2] = {0};
    char buffer[FLOG_BUFFER_SIZE] = {0};
    FILE *output = stdout;

    if (level >= FLOG_LEVEL_ERROR) {
        output = stderr;
    }

    if (level < FLOG_LEVEL) return;
    va_start(arg, message);
    ret = vsnprintf(buffer, FLOG_BUFFER_SIZE, message, arg);
    if (ret < 0) {
        fprintf(stderr, "[FLOG] [WARN]: `vsnprintf` failed, ret(%d) buffer size(%zu)", ret, FLOG_BUFFER_SIZE);
        return;
    }
    va_end(arg);

    // https://stackoverflow.com/questions/8304259/formatting-struct-timespec
#ifndef FLOG_DISABLE_TIMESTAMP
    struct tm tm;
    char time_buffer[FLOG_TIMESTAMP_BUFFER_SIZE] = {0};

#ifdef __MINGW32__
    // TODO: I may never be using Window any more so this might not work
    time_t t = time(NULL);
    tm = *(localtime(&t));
    offset += strftime(time_buffer, FLOG_TIMESTAMP_BUFFER_SIZE, "%Y-%m-%dT%H:%M:%S", &tm);
#else
    struct timespec now;
    ret = clock_gettime(CLOCK_REALTIME, &now);
    if (ret < 0) {
        fprintf(stderr, "[FLOG] [WARN]: `clock_gettime` failed. %s", strerror(errno));
        return;
    }
    gmtime_r(&now.tv_sec, &tm);
    offset += strftime(time_buffer, FLOG_TIMESTAMP_BUFFER_SIZE, "%Y-%m-%dT%H:%M:%S", &tm);
    offset += sprintf(time_buffer + offset, ".%09luZ", now.tv_nsec);
#endif // __MINGW32__
    offset = sprintf(prefix, "%s ", time_buffer);
#endif // FLOG_DISABLE_TIMESTAMP

    offset += sprintf(prefix + offset, "[%c] ", log_levels[(size_t)level]);
#ifdef FLOG_DISABLE_LOCATION
    (void) function;
    (void) line;
#else
    offset += sprintf(prefix + offset, "[%s:%d] ", function, line);
#endif // FLOG_DISABLE_LOCATION
    fprintf(output, "%s%s\n", prefix, buffer);
}

#endif // FLOG_IMPLEMENTATION
#endif // FLEXHEEPLE_LOG_H_
