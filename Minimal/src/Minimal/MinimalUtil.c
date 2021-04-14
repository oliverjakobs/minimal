#include "MinimalUtil.h"

/*
 * -----------------------------------------------------------------------------------
 *                               Debug Logging
 * -----------------------------------------------------------------------------------
 */
#define MINIMAL_LOG_BLACK       "\x1b[30m"
#define MINIMAL_LOG_RED         "\x1b[31m"
#define MINIMAL_LOG_GREEN       "\x1b[32m"
#define MINIMAL_LOG_YELLOW      "\x1b[33m"
#define MINIMAL_LOG_BLUE        "\x1b[34m"
#define MINIMAL_LOG_MAGENTA     "\x1b[35m"
#define MINIMAL_LOG_CYAN        "\x1b[36m"
#define MINIMAL_LOG_WHITE       "\x1b[37m"

#define MINIMAL_LOG_BG_BLACK    "\x1b[40m"
#define MINIMAL_LOG_BG_RED      "\x1b[41m"
#define MINIMAL_LOG_BG_GREEN    "\x1b[42m"
#define MINIMAL_LOG_BG_YELLOW   "\x1b[43m"
#define MINIMAL_LOG_BG_BLUE     "\x1b[44m"
#define MINIMAL_LOG_BG_MAGENTA  "\x1b[45m"
#define MINIMAL_LOG_BG_CYAN     "\x1b[46m"
#define MINIMAL_LOG_BG_WHITE    "\x1b[47m"

#define MINIMAL_LOG_RESET       "\x1b[0m" /* No color */

static const char* MinimalLoggerGetLevelStr(MinimalLogLevel level)
{
    switch (level)
    {
    case MINIMAL_LOG_TRACE:     return MINIMAL_LOG_WHITE "[TRACE]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_INFO:      return MINIMAL_LOG_GREEN "[INFO]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_WARN:      return MINIMAL_LOG_YELLOW "[WARN]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_ERROR:     return MINIMAL_LOG_RED "[ERROR]" MINIMAL_LOG_RESET " ";
    case MINIMAL_LOG_CRITICAL:  return MINIMAL_LOG_WHITE MINIMAL_LOG_BG_RED "[CRITICAL]" MINIMAL_LOG_RESET " ";
    default: return "";
    }
}

void MinimalLoggerPrint(FILE* const stream, MinimalLogLevel level, const char* fmt, ...)
{
    fprintf(stream, MinimalLoggerGetLevelStr(level));

    va_list arg;
    va_start(arg, fmt);
    vfprintf(stream, fmt, arg);
    va_end(arg);

    fprintf(stream, "\n");
}

/*
 * -----------------------------------------------------------------------------------
 *                               Timer
 * -----------------------------------------------------------------------------------
 */


void MinimalTimerReset(MinimalTimer* timer)
{
    timer->frames = 0;
    timer->fps = 0;

    timer->deltatime = 0;
    timer->start = 0;
}

double MinimalClockToMS(clock_t ticks)
{
    // units/(units/time) => time (seconds) * 1000 = milliseconds
    return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
}

void MinimalTimerStart(MinimalTimer* timer)
{
    timer->start = clock();
}

void MinimalTimerEnd(MinimalTimer* timer)
{
    clock_t end = clock();

    timer->deltatime += end - timer->start;
    timer->frames++;

    if (MinimalClockToMS(timer->deltatime) > 1000.0) //every second
    {
        timer->fps = (double)timer->frames; //more stable
        timer->frames = 0;
        timer->deltatime -= CLOCKS_PER_SEC;
    }
}