#ifndef MINIMAL_UTIL_H
#define MINIMAL_UTIL_H

#define MINIMAL_FAIL    0
#define MINIMAL_OK      1

#if _DEBUG
#define MINIMAL_ENABLE_LOGGING
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

typedef struct MinimalWindow MinimalWindow;

/*
 * -----------------------------------------------------------------------------------
 *                               Debug Logging
 * -----------------------------------------------------------------------------------
 */
#ifdef MINIMAL_ENABLE_LOGGING

#define MINIMAL_TRACE(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_TRACE, s, __VA_ARGS__)
#define MINIMAL_INFO(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_INFO, s, __VA_ARGS__)
#define MINIMAL_WARN(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_WARN, s, __VA_ARGS__)
#define MINIMAL_ERROR(s, ...)		MinimalLoggerPrint(stdout, MINIMAL_LOG_ERROR, s, __VA_ARGS__)
#define MINIMAL_CRITICAL(s, ...)	MinimalLoggerPrint(stdout, MINIMAL_LOG_CRITICAL, s, __VA_ARGS__)

#else

#define MINIMAL_TRACE(s, ...)
#define MINIMAL_INFO(s, ...)
#define MINIMAL_WARN(s, ...)
#define MINIMAL_ERROR(s, ...)
#define MINIMAL_CRITICAL(s, ...)

#endif

typedef enum
{
    MINIMAL_LOG_TRACE,
    MINIMAL_LOG_INFO,
    MINIMAL_LOG_WARN,
    MINIMAL_LOG_ERROR,
    MINIMAL_LOG_CRITICAL
} MinimalLogLevel;

void MinimalLoggerPrint(FILE* const stream, MinimalLogLevel level, const char* fmt, ...);

/*
 * -----------------------------------------------------------------------------------
 *                               Timer
 * -----------------------------------------------------------------------------------
 */
#include <time.h>

typedef struct
{
    uint32_t frames;
    uint32_t fps;

    clock_t deltatime;
    clock_t start;
} MinimalTimer;

void MinimalTimerReset(MinimalTimer* timer);
void MinimalTimerStart(MinimalTimer* timer);
void MinimalTimerEnd(MinimalTimer* timer);

#endif // !MINIMAL_UTIL_H
