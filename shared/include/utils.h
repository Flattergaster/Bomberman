#ifndef BOMBERMAN_UTIL_H
#define BOMBERMAN_UTIL_H

#define SERVER_PORT 1337

#define E_NOTICE 0
#define E_WARNING 1
#define E_ERROR 2
#define E_ALL 3

#define KEY_L 1
#define KEY_R 2
#define KEY_U 3
#define KEY_D 4
#define KEY_S 5
#define KEY_E 6

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void log_message(FILE * fd, int level, const char * format, va_list args);

void log_notice(FILE *fd, const char *format, ...);
void log_warn(FILE *fd, const char *format, ...);
void log_error(FILE *fd, const char *format, ...);

#endif  /* BOMBERMAN_UTIL_H */
