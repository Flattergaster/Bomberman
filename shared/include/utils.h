#ifndef BOMBERMAN_UTIL_H
#define BOMBERMAN_UTIL_H

#define SERVER_PORT 1337

#define E_NOTICE 0
#define E_WARNING 1
#define E_ERROR 2
#define E_ALL 3

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void log_message(FILE * fd, int level, const char * format, va_list args);

void log_notice(FILE *fd, const char *format, ...);
void log_warn(FILE *fd, const char *format, ...);
void log_error(FILE *fd, const char *format, ...);

#endif //BOMBERMAN_UTIL_H
