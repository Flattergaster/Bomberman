#include "utils.h"

void log_message(FILE *fd, int level, const char *format, va_list args) {
    char message[MAX_LOG_MSG_SIZE];

    memset(message, 0, MAX_LOG_MSG_SIZE);

    switch (level) {
        case E_NOTICE:
            strncat(message, "[NOTICE] ", 9);
            break;

        case E_WARNING:
            strncat(message, "[WARNING] ", 10);
            break;

        case E_ERROR:
            strncat(message, "[ERROR] ", 8);
            break;
    }

    strncat(message, format, 500);
    strcat(message, "\n");

    vfprintf(fd, message, args);
    va_end(args);
}

void log_notice(FILE *fd, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log_message(fd, E_NOTICE, format, args);

    va_end(args);
}

void log_warn(FILE *fd, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log_message(fd, E_WARNING, format, args);

    va_end(args);
}

void log_error(FILE *fd, const char *format, ...) {
    va_list args;
    va_start(args, format);

    log_message(fd, E_ERROR, format, args);

    va_end(args);
}
