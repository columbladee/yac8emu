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

void initLogger(const char *logFilePath);
void closeLogger();

void logInfo(const char *format, ...);
void logWarning(const char *format, ...);
void logError(const char *format, ...);
void logDebug(const char *format, ...);


#endif
