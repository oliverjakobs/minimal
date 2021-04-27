#include "MinimalUtil.h"

/* --------------------------| input |----------------------------------- */
MinimalBool MinimalKeycodeValid(uint32_t keycode)
{
    return keycode >= MINIMAL_KEY_FIRST && keycode <= MINIMAL_KEY_LAST;
}

MinimalBool MinimalMouseButtonValid(uint32_t buttoncode)
{
    return buttoncode >= MINIMAL_MOUSE_BUTTON_1 && buttoncode <= MINIMAL_MOUSE_BUTTON_LAST;
}

/* --------------------------| logging |--------------------------------- */
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

#define MINIMAL_LOG_RESET       "\x1b[0m" /* no color */

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
    va_list arg;
    va_start(arg, fmt);
    MinimalLoggerPrintV(stream, level, fmt, arg);
    va_end(arg);
}

void MinimalLoggerPrintV(FILE* const stream, MinimalLogLevel level, const char* fmt, va_list args)
{
    fprintf(stream, MinimalLoggerGetLevelStr(level));
    vfprintf(stream, fmt, args);
    fprintf(stream, "\n");
}

/* --------------------------| error callback |-------------------------- */
static void MinimalDefaultErrorCallback(MinimalLogLevel level, const char* modul, const char* fmt, va_list args)
{
    printf(MinimalLoggerGetLevelStr(level));
    printf("[%s] ", modul);
    vprintf(fmt, args);
    printf("\n");
}

static MinimalErrorCB _minimal_error_callback = MinimalDefaultErrorCallback;

void MinimalSetErrorCallback(MinimalErrorCB callback)
{
    _minimal_error_callback = callback;
}

void MinimalErrorCallback(MinimalLogLevel level, const char* modul, const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    _minimal_error_callback(level, modul, fmt, arg);
    va_end(arg);
}

/* --------------------------| timer |----------------------------------- */
void MinimalTimerReset(MinimalTimer* timer)
{
    timer->seconds = 0.0;
    timer->frames = 0;
    timer->fps = 0;

    timer->deltatime = 0.0;
    timer->lastframe = 0.0;
}

void MinimalTimerStart(MinimalTimer* timer, double seconds)
{
    timer->deltatime = seconds - timer->lastframe;
    timer->lastframe = seconds;
}

void MinimalTimerEnd(MinimalTimer* timer, double seconds)
{
    timer->frames++;
    if ((seconds - timer->seconds) > 1.0)
    {
        timer->seconds += 1.0;
        timer->fps = timer->frames;
        timer->frames = 0;
    }
}

/* --------------------------| memory |---------------------------------- */
static void* _minimal_allocator = NULL;
static MinimalAllocCB _minimal_alloc = NULL;
static MinimalFreeCB _minimal_free = NULL;

void MinimalSetAllocator(void* allocator, MinimalAllocCB alloc_cb, MinimalFreeCB free_cb)
{
    _minimal_allocator = allocator;
    _minimal_alloc = alloc_cb;
    _minimal_free = free_cb;
}

void* MinimalAlloc(size_t size)
{
    if (_minimal_alloc)	return _minimal_alloc(_minimal_allocator, size);
    else				return malloc(size);
}

void MinimalFree(void* block)
{
    if (_minimal_free)	_minimal_free(_minimal_allocator, block);
    else				free(block);
}