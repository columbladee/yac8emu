#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>

//Log levels

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} LogLevel;

void initLogger(const char *logPath);
void logInfo(const char *message, ...);
void logWarning(const char *message, ...);
void logError(const char *message, ...);
void logDebug(const char *message, ...);
void closeLogger();

#endif
