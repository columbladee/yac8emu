#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static FILE *logFile = NULL;

/**
 * Recursively creates directories in given path.
 * @param path - directory path to create
 * @return 0 on success, -1 on failure
 */

static int createDirectory(const char *path) {
    char temp[256]; // Temporary buffer to store path
    char *p = NULL; // Pointer to iterate over path
    size_t len; // Length of path

    //Copy path
    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if (temp[len - 1] == '/') { 
        temp[len - 1] = '\0'; // Remove trailing slash
    }

    //Iterate over path
    for (p = temp +1; *p; p++) {
        if (*p == '/') {
            *p = '\0'; // Replace '/' with null character
            if (mkdir(temp, S_IRWXU) != 0) { // Create directory with read, write, execute permissions for user
                if (errno != EEXIST) { // If directory already exists, continue
                    return -1; // Return -1 on failure
                }
            }
            *p = '/'; // Replace null character with '/'
        }
    }

    if (mkdir(temp, S_IRWXU) != 0) { // Create directory with read, write, execute permissions for user
        if (errno != EEXIST) { // If directory already exists, continue
            fprintf(stderr, "Failed to create directory: %s with error %s\n", path, strerror(errno));
            return -1; 
        }
    }
}

// Example usage
//                  initLogger("logs/2024/10/chip8.log");
//                  logInfo("This is an info message");
//                  logWarning("This is a warning message");
//                  logError("This is an error message");
//                  logDebug("This is a debug message");
//                  closeLogger();


void initLogger(const char *logPath) {
    if (logFile != NULL) {
        fclose(logFile); // Close existing log file
    }

    // Create directories if they DNE
    char *logPathCopy = strdup(logPath); // Copy log path
    if (logPathCopy == NULL) {
        fprintf(stderr, "Error duplicating log path \n");
        exit(EXIT_FAILURE);
    }

    char *lastSlash = strrchr(logPathCopy, '/'); // Find last slash in path
    if (lastSlash != NULL) {
        *lastSlash = '\0'; // Replace last slash with null character
        if (createDirectory(logPathCopy) != 0) {
            free(logPathCopy); // Free memory
            exit(EXIT_FAILURE);
        }
    }
    free (logPathCopy); // Free memory

    logFile = fopen(logPath, "a"); // Open log file in append mode
    if (logFile == NULL) {
        fprinf(stderr, "Failed to open log file: '%s': %s\n", logPath, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

static void logMessage(LogLevel level, const char *format, va_list args) {
    if (logFile == NULL) {
        fprintf(stderr, "Logger not initialized. Call initLogger() first\n");
        return;
    }
    //Get current time
    time_t now = time(NULL);
    struct tm *localTime = localtime(&now);

    //Log level as string
    const char *levelStr;
    switch (level) {
        case LOG_INFO: levelStr = "INFO"; break;
        case LOG_WARNING: levelStr = "WARNING"; break;
        case LOG_ERROR: levelStr = "ERROR"; break;
        case LOG_DEBUG: levelStr = "DEBUG"; break;
        default: levelStr = "UNKNOWN"; break;
    }

    //print timestamp and log level
    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s: ", 
            localTime->tm_year + 1900,
            localTime->tm_mon + 1,
            localTime->tm_mday,
            localTime->tm_hour,
            localTime->tm_min,
            localTime->tm_sec,
            levelStr);

    //print formatted log message
    vfprintf(logFile, format, args);
    fprintf(logFile, "\n");
    fflush(logFile); // Flush buffer - ensure message writes to files

}

void logInfo(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_INFO, format, args);
    va_end(args);
}

void logWarning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_WARNING, format, args);
    va_end(args);
}

void logError(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_ERROR, format, args);
    va_end(args);
}

void logDebug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    logMessage(LOG_DEBUG, format, args);
    va_end(args);
}

void closeLogger() {
    if (logFile != NULL) {
        fclose(logFile);
        logFile = NULL;
    }
}