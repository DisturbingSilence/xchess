#ifndef XCHESS_LOGGER_H
#define XCHESS_LOGGER_H

typedef enum
{
    SEVERITY_CRITICAL,
    SEVERITY_WARNING,
    SEVERITY_INFO
} ErrorSeverity;

#define LOG_CRITICAL(msg) (log_error(msg,SEVERITY_CRITICAL))
#define LOG_WARNING(msg) (log_error(msg,SEVERITY_WARNING))
#define LOG_INFO(msg) (log_error(msg,SEVERITY_INFO))

void log_error(const char* msg,ErrorSeverity severity);

#endif
