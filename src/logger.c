#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static FILE *logFile = NULL;

void initLogger(const char *logFilePath) {
    logFile = fopen(logFilePath, "w");
    if (!logFile) {
        fprintf(stderr, "Failed to open log file: %s\n", logFilePath);
    }
}

void closeLogger() {
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
}

void logMessage(LogLevel level, const char *format, va_list args) {
    if (!logfile) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", t);

    const char *lecelStr;
    switch (level) {
        case LOG_LEVEL_ERROR: levelStr = "ERROR"; break;
        case LOG_LEVEL_WARNING: levelStr = "WARNING"; break;
        case LOG_LEVEL_INFO: levelStr = "INFO"; break;
        case LOG_LEVEL_DEBUG: levelStr = "DEBUG"; break;
        default: levelStr = "UNKNOWN"; break;
    }

    fprintf(logFile, "[%s] [%s] ", timeStr, levelStr);
    vfprintf(logFile, format, args);
    fprintf(logFile, "\n");
    fflush(logFile);
}

void logError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_ERROR, format, args);
    va_end(args);
}

void logWarning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_WARNING, format, args);
    va_end(args);
}

void logInfo(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_INFO, format, args);
    va_end(args);
}

void logDebug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}
