#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

typedef enum {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

extern LogLevel current_log_level;

#define LOG(level, fmt, ...) \
    do { \
        if (level >= current_log_level) { \
            fprintf(stderr, "[%s] " fmt "\n", #level, ##__VA_ARGS__); \
        } \
    } while (0)

#define TRACE_LOG(fmt, ...) LOG(LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)
#define DEBUG_LOG(fmt, ...) LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define INFO_LOG(fmt, ...) LOG(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define WARN_LOG(fmt, ...) LOG(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define ERROR_LOG(fmt, ...) LOG(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

void set_log_level(LogLevel level);

#endif // LOGGING_H
