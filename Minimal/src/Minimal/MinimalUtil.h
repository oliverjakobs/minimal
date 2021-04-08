#ifndef MINIMAL_UTIL_H
#define MINIMAL_UTIL_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#define MINIMAL_FAIL    0
#define MINIMAL_OK      1

#if _DEBUG
#define MINIMAL_ENABLE_LOGGING
#endif

#include <stdio.h>
#include <stdarg.h>

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
typedef struct
{
    double seconds;
    int frames;
    int fps;

    double deltatime;
    double lastframe;
} MinimalTimer;

void MinimalTimerReset(MinimalTimer* timer);
void MinimalTimerStart(MinimalTimer* timer, double seconds);
void MinimalTimerEnd(MinimalTimer* timer, double seconds);

#endif // !MINIMAL_UTIL_H
