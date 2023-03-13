#ifndef MINIMAL_UTILS_H
#define MINIMAL_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>

#define MINIMAL_FAIL    0
#define MINIMAL_OK      1

#ifndef _DEBUG
#define MINIMAL_DISABLE_LOGGING
#define MINIMAL_DISABLE_ASSERT
#endif

/* minimal version numbers */
#define MINIMAL_VERSION_MAJOR       1
#define MINIMAL_VERSION_MINOR       3
#define MINIMAL_VERSION_REVISION    3

void minimalGetVersion(int* major, int* minor, int* rev);
const char* minimalGetVersionString();

typedef struct MinimalEvent MinimalEvent;
typedef struct MinimalApp MinimalApp;

/* --------------------------| logging |--------------------------------- */
#ifndef MINIMAL_DISABLE_LOGGING

#define MINIMAL_TRACE(s, ...)     minimalLoggerPrint(stdout, MINIMAL_LOG_TRACE, s, __VA_ARGS__)
#define MINIMAL_INFO(s, ...)      minimalLoggerPrint(stdout, MINIMAL_LOG_INFO, s, __VA_ARGS__)
#define MINIMAL_WARN(s, ...)      minimalLoggerPrint(stdout, MINIMAL_LOG_WARN, s, __VA_ARGS__)
#define MINIMAL_ERROR(s, ...)     minimalLoggerPrint(stdout, MINIMAL_LOG_ERROR, s, __VA_ARGS__)
#define MINIMAL_CRITICAL(s, ...)  minimalLoggerPrint(stdout, MINIMAL_LOG_CRITICAL, s, __VA_ARGS__)

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

void minimalLoggerPrint(FILE* const stream, MinimalLogLevel level, const char* fmt, ...);
void minimalLoggerPrintV(FILE* const stream, MinimalLogLevel level, const char* fmt, va_list args);


/* --------------------------| assert |---------------------------------- */
#ifndef MINIMAL_DISABLE_ASSERT

#include <assert.h>

#define MINIMAL_ASSERT(expr, msg) assert(((void)(msg), (expr)))

#else

#define MINIMAL_ASSERT(expr, msg) 

#endif


/* --------------------------| timer |----------------------------------- */
typedef struct
{
    uint32_t frames;
    uint32_t fps;

    double seconds;
    double deltatime;
    double lastframe;
} MinimalTimer;

void minimalTimerReset(MinimalTimer* timer);

void minimalTimerStart(MinimalTimer* timer, double seconds);
void minimalTimerEnd(MinimalTimer* timer, double seconds);

uint32_t minimalGetFps(const MinimalApp* app);

#endif // !MINIMAL_UTILS_H
