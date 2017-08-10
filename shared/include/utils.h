#ifndef BOMBERMAN_UTIL_H
#define BOMBERMAN_UTIL_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define E_NOTICE    0
#define E_WARNING   1
#define E_ERROR     2
#define E_ALL       3

#define MAX_LOG_MSG_SIZE 512

void log_message(FILE * fd, int level, const char * format, va_list args);

void log_notice(FILE *fd, const char *format, ...);
void log_warn(FILE *fd, const char *format, ...);
void log_error(FILE *fd, const char *format, ...);

#endif  /* BOMBERMAN_UTIL_H */
