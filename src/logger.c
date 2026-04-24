#include "logger.h"
#include <stdio.h>

void log_error(const char* msg,ErrorSeverity severity)
{
    switch(severity)
    {
        case SEVERITY_CRITICAL:
            printf("[CRITICAL] %s\n",msg);
            break;
        case SEVERITY_WARNING:
            printf("[WARNING] %s\n",msg);
            break;
        case SEVERITY_INFO:
            printf("[INFO] %s\n",msg);
            break;
    }
}
