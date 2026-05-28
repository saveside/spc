#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

#define RED     "\x1b[31m"
#define YELLOW  "\x1b[33m"
#define GREEN   "\x1b[32m"
#define NONE    "\x1b[0m"

static inline const char* clean_errno(void) {
  return errno == 0 ? "None" : strerror(errno);
};

#define log_info(M, ...) \
    do { \
        fprintf(stderr, \
                GREEN "[INFO]   " \
                "%s (%s:%d) " NONE M "\n", \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__); \
    } while (0)

#define log_warn(M, ...) \
    do { \
        fprintf(stderr, \
                YELLOW "[WARN]   " \
                "%s (%s:%d) " NONE M YELLOW " errno: %s\n" NONE, \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__, clean_errno()); \
    } while (0)

#define log_err(M, ...) \
    do { \
        fprintf(stderr, \
                RED "[ERROR]  " \
                "%s (%s:%d) " NONE M YELLOW " errno: %s\n" NONE, \
                __func__, __FILE__, __LINE__, ##__VA_ARGS__, clean_errno()); \
    } while (0)

#endif // LOGGER_H
